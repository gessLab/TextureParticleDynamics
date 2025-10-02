IFLAGS= -I /opt/homebrew/include
LFLAGS= -L /opt/homebrew/lib
OFLAGS=  -framework OpenGL -lGLM -lGLEW -lSDL3
CPPTYPE= -std=c++11
DIR= .

all: TexDyn.cpp TexDyn.hpp ThreadPool.cpp ThreadPool.hpp main.cpp
	clang++ $(CPPTYPE) $(LFLAGS) $(IFLAGS) $(OFLAGS) main.cpp TexDyn.cpp ThreadPool.cpp -o main

clean:
	rm -f main
	rm -f *.o
