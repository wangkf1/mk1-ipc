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
#include <list>
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
    // matrix->print();
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: ./main <worker_id> <num_secs>" << "\n";
        return 1;
    }
    int num_secs = -1;
    try {
        num_secs = std::stoi(argv[2]);
    } catch (std::exception e) {
        std::cout << "error: " << argv[2] << " is not a number!\n";
        return 1;
    }
    MatrixJobQueue pool(argv[1], false);
    std::list<std::pair<uint32_t, Matrix*>> retries;
    uint32_t returnedMatrices = 0;
    auto startTime = high_resolution_clock::now();
    while (duration_cast<seconds>(high_resolution_clock::now() - startTime).count() < num_secs) {
        // run through retries first
        auto it = retries.begin();
        while (it != retries.end()) {
            auto [token, matrix] = retries.front();
            // try to give producer a completed token/matrix
            if (pool.pushToProducer({token, matrix})) {
                // std::cout << "--- successfully retried " << token << " --- \n";
                it = retries.erase(it);
            } else {
                ++it;
            }
        }

        // wait for pool to take from token msg queue and add to token pool
        if (!pool.waitConsumerQueue(1)) {
            // std::cout << "---- failed to pop consumer queue ---\n";
            continue;
        }

        // move token to this loop
        auto optToken = pool.popMatrixToken();
        if (optToken == std::nullopt) {
            // std::cout << "--- no tokens ---\n";
            continue;
        }

        auto [token, matrix] = optToken.value();
        squareMatrixElems(matrix);
        // try to give producer a completed token/matrix
        if (!pool.pushToProducer({token, matrix})) {
            // std::cout << "--- failed to push to producer queue " << token << " ---\n";
            retries.push_back({token, matrix});
            continue;
        }
        ++returnedMatrices;
        // std::cout << "--- processed " << token << (pool.consumerQueue_.empty() ? ", empty queue " : "") <<  "---\n";
        // usleep(1000000);
    }

        std::cout << "Processed " << returnedMatrices << "\n";
    return 0;
}
