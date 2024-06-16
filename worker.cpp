// linux headers
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <errno.h>
#include <string.h>
#include <unistd.h>

// std lib
#include <string>
#include <iostream>
#include <chrono>
using namespace std::chrono;

#include "matrix.hpp"
#include "shm_queue.hpp"
#include "matrix_job_queue.hpp"

// square matrix elements
void squareMatrixElems(Matrix* matrix) {
    for (int i = 0; i < matrix->dims[0]; ++i) {
        for (int j = 0; j < matrix->dims[1]; ++j) {
            matrix->data[i][j] = matrix->data[i][j] * matrix->data[i][j];
        }
    }
    matrix->print();
}

int main(int argc, char** argv) {
    MatrixJobQueue pool("/w1", 10, false);
    std::cout << "enter to push, q to quit: \n";
    char c;
    while (true) {
        c = getchar();
        if (c == 'q') {
            break;
        }

        if (!pool.popConsumerQueue()) {
            std::cout << "---- failed to pop consumer queue ---\n";
            continue;
        }

        auto optToken = pool.popMatrixToken();
        if (optToken == std::nullopt) {
            std::cout << "--- no tokens ---\n";
            continue;
        }

        auto [token, matrix] = optToken.value();
        squareMatrixElems(matrix);
        if (!pool.pushToProducer({token, matrix})) {
            std::cout << "--- failed to push to consumer queue ---\n";
            continue;
        }

    }
    return 0;
}
