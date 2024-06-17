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
#include <list>
using namespace std::chrono;

// interview specific headers/consts
#include "matrix.hpp"
#include "shm_queue.hpp"
#include "matrix_job_queue.hpp"
constexpr int NUM_WORKERS = 4;

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
    // matrix->print();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: ./main <num_secs>" << "\n";
        return 1;
    }

    int num_secs = -1;
    try {
        num_secs = std::stoi(argv[1]);
    } catch (std::exception e) {
        std::cout << "error: " << argv[1] << " is not a number!\n";
        return 1;
    }

    MatrixJobQueue mpools[NUM_WORKERS] = {
        {"/w1", true},
        {"/w2", true},
        {"/w3", true},
        {"/w4", true},
    };

    std::cout << "Press enter to begin... ";
    getchar();

    std::list<std::tuple<int, uint32_t, Matrix*>> retries;
    uint32_t returnedMatrices = 0;
    auto startTime = high_resolution_clock::now();
    while (duration_cast<seconds>(high_resolution_clock::now() - startTime).count() < num_secs) {
        for (int i = 0; i < NUM_WORKERS; ++i) {
            // std::cout << i << ":\n";
            while (mpools[i].popProducerQueue()) {
                ++returnedMatrices;
            }
        }

        // run through retries first
        auto it = retries.begin();
        while (it != retries.end()) {
            auto [worker, token, matrix] = retries.front();
            // try to push token onto consumer queue
            if (mpools[worker].pushToConsumer({token, matrix})) {
                // std::cout << "--- successfully retried " << token << " to " << worker << " --- \n";
                it = retries.erase(it);
            } else {
                ++it;
            }
        }

        int worker = rand() % NUM_WORKERS;
        // get a matrix token that we can write to
        auto optToken = mpools[worker].popMatrixToken();
        if (optToken == std::nullopt) {
            // std::cout << "--- no tokens ---\n";
            usleep(100000);
            continue;
        }

        auto [token, matrix] = optToken.value();
        generateMatrix(matrix);
        if (!mpools[worker].pushToConsumer({token, matrix})) {
            // std::cout << "--- failed to push to consumer queue " << token << " ---\n";
            retries.push_back({worker, token, matrix});
            continue;
        }

        // std::cout << "--- successfully sent " << token << " to " << worker << ", " << mpools[worker].matrixTokens_.size() << " tokens left ---\n";
        // usleep(100000);
    }
    std::cout << "Processed " << returnedMatrices << " matrices in " << num_secs << " seconds\n";

    return 0;
}
