all: main.cpp 
	g++ -std=c++17 main.cpp -o main -lncurses

run: all
	./main

clean:
	rm main
