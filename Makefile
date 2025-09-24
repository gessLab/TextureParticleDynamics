LIBS = -lSDL3 -lGL -lGLEW

simulation: main.o TexDyn.o ThreadPool.o
	g++ *.o -o simulation $(LIBS)

main.o: TexDyn.hpp main.cpp
	g++ main.cpp -c

TexDyn.o: TexDyn.cpp TexDyn.hpp
	g++ TexDyn.cpp -c

ThreadPool.o: ThreadPool.cpp ThreadPool.hpp
	g++ ThreadPool.cpp -c

clean:
	rm *.o simulation
