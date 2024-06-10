CLANG := clang++ -std=c++17 -Wall

all: main worker

main: main.cpp
	${CLANG} main.cpp -o $@

worker: worker.cpp
	${CLANG} worker.cpp -o $@

clean:
	rm -f main worker