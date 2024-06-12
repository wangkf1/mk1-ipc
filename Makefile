CLANG := clang++ -std=c++17 -Wall -g

all: main worker test_queue

main: main.cpp helpers
	${CLANG} main.cpp -o $@

worker: worker.cpp helpers
	${CLANG} worker.cpp -o $@

test_queue: test_queue.cpp shm_queue.hpp
	${CLANG} test_queue.cpp -o $@

helpers: shm_queue.hpp matrix.hpp shm_wrapper.hpp

clean:
	rm -f main worker