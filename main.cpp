// linux headers
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

// std lib
#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <chrono>
using namespace std::chrono;

// interview specific headers/consts
#include "matrix.hpp"
#include "shm_queue.hpp"
#include "matrix_job_queue.hpp"
constexpr int NUM_WORKERS = 1;

// random dims (up to 1024x1024) and random data
void generateMatrix(Matrix* matrix) {
    matrix->dims[0] = rand() % MAX_MATRIX_SIZE;
    matrix->dims[1] = rand() % MAX_MATRIX_SIZE;
    for (int i = 0; i < matrix->dims[0]; ++i) {
        for (int j = 0; j < matrix->dims[1]; ++j) {
            // matrix->data[i][j] = rand() % 1024;
            matrix->data[i][j] = i*matrix->dims[1] + j; 
        }
    }
    matrix->print();
}

int main(int argc, char** argv) {
    // if (argc < 2) {
    //     std::cout << "Usage: ./main <num_secs>" << "\n";
    //     return 1;
    // }

    // int num_secs = -1;
    // try {
    //     num_secs = std::stoi(argv[1]);
    // } catch (std::exception e) {
    //     std::cout << "error: " << argv[1] << " is not a number!\n";
    //     return 1;
    // }

    
    MatrixJobQueue pool("/w1", 10, true);
    std::cout << "enter to push, q to quit: \n";
    char c;
    while (true) {
        c = getchar();
        if (c == 'q') {
            break;
        }
        while (pool.popProducerQueue()) {
            std::cout << "--- got a matrix back ---\n";
        }

        auto optToken = pool.popMatrixToken();
        if (optToken == std::nullopt) {
            std::cout << "--- no tokens ---\n";
            continue;
        }

        auto [token, matrix] = optToken.value();
        generateMatrix(matrix);
        if (!pool.pushToConsumer({token, matrix})) {
            std::cout << "--- failed to push to consumer queue ---\n";
            continue;
        }

    }

    return 0;
}
