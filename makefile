all: src/puzzle.cpp
	g++ -Wno-write-strings -o bin/puzzle src/puzzle.cpp
	jgraph src/cathead.jgr > objects/cathead.ps
	jgraph src/pawn.jgr > objects/pawn.ps
	./bin/puzzle objects/cathead.ps 4 4


clean:
	rm bin/*
	rm objects/*
	rm pieces/*
	rm src/jmush.jgr
	rm src/jslice.jgr
