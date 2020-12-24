all: main.cpp 
	g++ -std=c++17 main.cpp -o bin/main -lncurses

run: all
	./bin/main

clean:
	rm bin/main
