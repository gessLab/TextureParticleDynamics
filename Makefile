# Author: Everton Albuquerque de Oliveira
# Date Created: September 24, 2025

LIBS = -lSDL3 -lGL -lGLEW
CC = g++

simulation: main.o TexDyn.o ThreadPool.o
	$(CC) *.o -o simulation $(LIBS)

main.o: TexDyn.hpp main.cpp
	$(CC) main.cpp -c

TexDyn.o: TexDyn.cpp TexDyn.hpp
	$(CC) TexDyn.cpp -c

ThreadPool.o: ThreadPool.cpp ThreadPool.hpp
	$(CC) ThreadPool.cpp -c

clean:
	rm *.o simulation
