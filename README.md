# 

Chartmaker is a program that generates a knitting chart from a PPM
file using jgraph.

## Setup the repository
1. Clone this repository
2. Run setup script with command `sh setup.sh`

All set up!

## Using Chartmaker
Use the script `convert.sh` to easily utilize jgraph and chartmaker:
```
sh convert.sh <input file name> <output file name> <arguments>
```

This converts your input file to the specified output file and applies the arguments (outlined below). Specifying no arguments will output a chart of the same dimensions of the input file, with the same colors, and with gridlines and labels at increments of 10 across the bottom and left sides. Whatever the extension of the output file is, it will convert it to that file type. So, for example, if you specify .pdf, it will convert to PDF, and if you specify .jpg, it will convert to JPG. 

### Arguments
The following arguments are currently supported:
* `no_gridlines` : turns off bold gridlines and numbering.
* `grid_spacing X` : specifies increment of gridlines and number labeling. The default is every 10 blocks.
* `no_numbering` : turns off number labels along width and height. Will still print bold gridlines.
* `all_numbers` : will print a number on every box. Not recommended for very large charts. Will still print gridlines at default 10, or at the specified gridline.
* `tile X Y` : tile the input image, by the given dimensions, where X is the width and Y is the height.
* `pad X` : pads the overall image by X blocks on each edge. If used with tiling, this will pad the resulting image after the tiling and tile padding is performed.
* `background R G B` : specifies the color of the padding blocks as an RGB value with each field ranging 0 to 255. White by default.

Note: if you specify conflicting arguments, the program will take the most recent it has received, or the last argument specified, and ignore the previous ones.


### Making your images
Here are the ways I have created PPM files that can be used reasonably with this program:
* The website [Convertio](https://convertio.co/) is a good website to convert between different filetypes. So no matter what image you have, you can convert it to a PPM through this site. Note that with PNGs with a transparent background, this website gives it a black background as a PPM.
* Find a pixel art image I like online, and use Paint 3D (on Windows) to scale it down to the minimum size. You can choose the option "lock aspect ratio" and size down the number of pixels to the minimum possible. So, if you have a pixel art that is 10 pixels by 10 pixels, you would want to reduce that canvas size to that number of pixels.
* Use [Pixil Art](https://www.pixilart.com/draw) to make a custom pixel art image. As mentioned above, you might want to get rid of the clear background or download it as a .jpg.

### Using the executables directly
If you want to use the executables individually, you can do that as well. Chartmaker is ran with the following arguments:
`./chartmaker <filename> <a/p> <arguments>`

* filename: specifies the input file name
* a/p: specifies PPM file or ASCII art file (Note: ASCII art not yet implemented)

Chartmaker outputs the jgraph on standard out. To put this output into a file, you can use shell redirection and specify the name of your output file. Here's an example:
```
./chartmaker myfile.ppm p > myoutput.jgr
```
So, this converts the PPM file myfile to a jgraph file called myoutput. It will create the file myoutput.jgr if it does not exist, or if it does exist, it will overwrite it.

[Please see this webpage](http://web.eecs.utk.edu/~jplank/plank/jgraph/jgraph.html) for more info on how to use jgraph.
