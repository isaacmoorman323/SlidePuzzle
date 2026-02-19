#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <random>
#include <cstring>
#include <chrono>
#include <unistd.h>
using namespace std;

class piece {
	public:
		int value;
		int position;
		piece *up;
		piece *down;
		piece *left;
		piece *right;
};

enum direction {north, south, east, west};
class puzzle {
	public:
		void print_board();
		puzzle(int height, int width);
		~puzzle();
		void scramble(int seed, int n);
		int move_hole(direction d);
		int move_position(int n);
		int output();

	private:
		int height;
		int width;
		piece *pieces;
		piece *hole;
		void swap(piece *a, piece *b);
};

puzzle::puzzle(int h, int w) {
	height = h;
	width = w;
	pieces = new piece[h*w];

	for (int i = 0; i < h*w; i++) {
		pieces[i].position = i;
		pieces[i].value = i;
	}
	hole = pieces+h*w-1;

	//set neighbors
	for (int i = 0; i < h*w; i++) {
		pieces[i].up = nullptr;
		pieces[i].down = nullptr;
		pieces[i].left = nullptr;
		pieces[i].right = nullptr;
		if ((i/width) != 0)
			pieces[i].up = pieces+i-width;
		if ((i/width) != height-1)
			pieces[i].down = pieces+i+width;
		if ((i%width) != 0)
			pieces[i].left = pieces+i-1;
		if ((i%width) != width-1)
			pieces[i].right = pieces+i+1;
	}
}

puzzle::~puzzle() {
	delete[] pieces;
}

//good
int puzzle::move_hole(direction d) {
	//check if the move is legal

	if ((d == north && hole->up == nullptr) ||
		 (d == south && hole->down == nullptr) ||
		 (d == west && hole->left == nullptr) ||
		 (d == east && hole->right == nullptr))
		return -1;

	if (d == north)
		swap(hole, hole->up);
	else if (d == south)
		swap(hole, hole->down);
	else if (d == west)
		swap(hole, hole->left);
	else
		swap(hole, hole->right);

	return 1;
}

int puzzle::move_position(int n) {
	piece *p = pieces+n;

	if (p->up == hole ||
		 p->down == hole ||
		 p->left == hole ||
		 p->right == hole)
	{
		swap(p, hole);
		return 1;
	}
	else
		return -1;
}


//good
void puzzle::swap(piece *a, piece *b) {
	int tmp = a->value;
	a->value = b->value;
	b->value = tmp;

	if (hole == a) {
		hole = b;
	}
	else if (hole == b) {
		hole = a;
	}
}

//good
int puzzle::output() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (pieces[i*width+j].value != height*width-1)
				printf("%-2d ", pieces[i*width+j].value);
			else
				printf("[] ");
		}
		printf("\n");
	} 

	//generate jgraph file
	FILE *f = NULL;
	f = fopen("src/jmush.jgr", "w");
	if (f == NULL) {
		perror("fopen");
		return -1;
	}

	fprintf(f, "newgraph\n");
	fprintf(f, "xaxis min %f max %f size 5.3 nodraw\n", 0.0f, (float) width);
	fprintf(f, "yaxis min %f max %f size 5.3 nodraw\n", -((float) height), 0.0f);

	//pieces
	for (int i = 0; i < height*width; i++) {
		int value = pieces[i].value;
		if (value == height*width -1)
			continue;
		int position = pieces[i].position;

		char obj[100];
		snprintf(obj, 100, "pieces/piece%d.ps", value);
		
		fprintf(f, "newcurve eps %s marksize %d %d pts %f %f \n\n",
				obj,
				1,
				1,
				0.5+position%width,
				-0.5-position/width);

	}

	//lines
	for (int i = 1; i < width; i++){
		fprintf(f, "newline linethickness 1 pts %f %f %f %f\n",
				(float) i, 0.0f,
				(float) i, -((float) height));

	}
	for (int i = 1; i < height; i++){
		fprintf(f, "newline linethickness 1 pts %f %f %f %f\n",
				0.0f, -((float) i),
				(float) width, -((float) i));
	}


	fclose(f);

	//run jgraph
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	}

	else if (pid == 0) {
		char *path = "/usr/bin/jgraph";
		char *arguments[] = {"jgraph", "src/jmush.jgr", NULL};

		int fd = open("out.ps", O_WRONLY | O_CREAT | O_TRUNC, 0666);
		dup2(fd, STDOUT_FILENO);
		execv(path, arguments);

		//error
		perror("execv");
		return -1;
	}
	wait(NULL);

	//run convert
	pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	}
	else if (pid == 0) {
		char *path = "/usr/bin/convert";
		char *arguments[] = {"convert", "-density", "300", "out.ps", "out.jpg", NULL};
		execv(path, arguments);
		perror("execv");
		return -1;
	}
	wait(NULL);
	return 0;
}

void puzzle::scramble(int seed, int n) {
	//set up generator
	default_random_engine gen(seed);
	uniform_int_distribution<int> distribution(0, 3);
	//generate a random move n times
	for(int i = 0; i < n; i++) {
		move_hole((direction) distribution(gen));
	}
}


int slicePicture(char *filename, int height, int width) {
	//check that the file exists
	struct stat st;
	if (stat(filename, &st) == -1) {
		perror("stat");
		return -1;
	}
	if ((st.st_mode & S_IFMT) != S_IFREG) {
		fprintf(stderr, "%s is not regular file.\n", filename);
		return -1;
	}
	//carve up the picture
	//generate jgraph file
	for (int i = 0; i < height*width; i++) {
		FILE *f = NULL;
		f = fopen("src/jslice.jgr", "w");
		if (f == NULL) {
			perror("fopen");
			return -1;
		}

		fprintf(f, "newgraph\n");
		fprintf(f, "xaxis min %f max %f size 5.3 nodraw\n", -((float)width / 2.0f), -((float)width / 2.0f)+1.0f);
		fprintf(f, "yaxis min %f max %f size 5.3 nodraw\n", ((float)height / 2.0f), ((float)height / 2.0f)-1.0f);
		fprintf(f, "clip\n");

		fprintf(f, "newcurve eps %s marksize %d %d pts %d %d \n\n",
				filename,
				width,
				height,
				-i%width,
				i/width);
		fclose(f);

		//run jgraph
		pid_t pid = fork();
		if (pid == -1) {
			perror("fork");
			return -1;
		}
		else if (pid == 0) {
			char *path = "/usr/bin/jgraph";
			char *arguments[] = {"jgraph", "src/jslice.jgr", NULL};
			char outfile[100];
			snprintf(outfile, 100, "pieces/piece%d.ps", i);

			int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			dup2(fd, STDOUT_FILENO);
			execv(path, arguments);

			//error
			perror("execv");
			return -1;
		}
		wait(NULL);
	}
	return 0;
}
int main(int argc, char **argv) {
	//read in board arguments
	int height, width;

	char usage[64];
	snprintf(usage, 64, "usage: %s filename.ps height width\n", argv[0]);
	if (argc != 4) {
		printf("%s", usage);
		return -1;
	}
	if (sscanf(argv[2], "%d", &height) < 0) {
		printf("%s", usage);
		return -1;
	}
	if (sscanf(argv[3], "%d", &width) < 0) {
		printf("%s", usage);
		return -1;
	}

	if (slicePicture(argv[1], height, width) < 0) {
		return -1;
	}

	//set up the board
	int seed = chrono::system_clock::now().time_since_epoch().count();
	puzzle *p = new puzzle(height, width);
	p->scramble(seed, height*width*10);
	int move_int;
	size_t size = 20;
	char *input = (char *) malloc(sizeof(char) * size);

	//open stdin

	bool ok = true;
	while(ok) {
		std::cout << "\x1b[2J\x1b[H";
		if (p->output() == -1) {
			free(input);
			delete p;
			return -1;
		}
		printf("\n(w, a, s, d, or q to quit)\n");
		printf("input move: ");

		//get input
		memset(input, '\0', size);
		input[::getline(&input, &size, stdin)] = '\0';
		cout << "\r\n" << endl;

		//char input
		if (sscanf(input, "%d", &move_int) == 0) 
		{
			switch(input[0]) {
				case 'w':
					p->move_hole(direction::north);
					break;
				case 'a':
					p->move_hole(direction::west);
					break;
				case 's':
					p->move_hole(direction::south);
					break;
				case 'd':
					p->move_hole(direction::east);
					break;
				default:
					ok = false;
					break;
			}
		}
		else 
		{
			//integer input
			p->move_position(move_int);
		}
	}

	//cleanup
	free(input);
	delete p;
	
}
