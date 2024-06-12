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

    std::cout << "Setting up shared memory... \n";
    // currently creating buffer of one matrix + two semaphores
    const int memSize = sizeof(Matrix) * NUM_MATRICES;
    std::vector<std::pair<int, void*>> worker_fds(NUM_WORKERS);
    for (int i = 0; i < NUM_WORKERS; ++i) {
        std::cout << "worker " << i << ": \n";
        std::string name = "/worker";
        name += std::to_string(i);
        int fd = shm_open(name.c_str(), O_RDWR | O_CREAT , 0666);
        if (fd == -1) {
            std::cout << "shm_open for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }

        if (ftruncate(fd, memSize) == -1) {
            std::cout << "ftruncate for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }

        void* ptr = mmap(0, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            std::cout << "mmap for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }
        worker_fds[i] = {fd, ptr};

        // sem_t* mut1 = (sem_t*)((char*)ptr + sizeof(Matrix));
        // sem_t* mut2 = (sem_t*)((char*)ptr + sizeof(Matrix) + sizeof(sem_t));
        // if (sem_init(mut1, 1, 0) == -1) {
        //     std::cout << "sem_init for " << name << " failed with " << strerror(errno) << std::endl;
        //     return 1;
        // }
        // if (sem_init(mut2, 1, 0) == -1) {
        //     std::cout << "sem_init for " << name << " failed with " << strerror(errno) << std::endl;
        //     return 1;
        // }

        // generateMatrix((Matrix*)ptr);
    }

    std::cout << "After launching " << NUM_WORKERS << " workers, press enter to continue.";
    getchar(); 

    // // generate matrices, send, then wait
    // for (int i = 0; i < NUM_WORKERS; ++i) {
    //     std::string name = "/worker";
    //     name += std::to_string(i);
    //     int fd = worker_fds[i].first;
    //     void* ptr = worker_fds[i].second;
    //     generateMatrix((Matrix*)ptr);
    //     ((Matrix*)ptr)->print();
    //     sem_t* mut1 = (sem_t*)((char*)ptr + sizeof(Matrix));
    //     sem_t* mut2 = (sem_t*)((char*)ptr + sizeof(Matrix) + sizeof(sem_t));
    //     if (sem_post(mut1) == -1) {
    //         std::cout << "sem_post for " << name << " failed with " << strerror(errno) << std::endl;
    //         return 1;
    //     }

    //     int semval;
    //     if (sem_getvalue(mut1, &semval) == -1) {
    //         std::cout << "sem_getvalue for " << name << " failed with " << strerror(errno) << std::endl;
    //         return 1;
    //     }
    //     std::cout << "sem val: " << semval << std::endl;
    // }

    // for (int i = 0; i < NUM_WORKERS; ++i) {
    //     std::string name = "/worker";
    //     name += std::to_string(i);
    //     int fd = worker_fds[i].first;
    //     void* ptr = worker_fds[i].second;
    //     sem_t* mut1 = (sem_t*)((char*)ptr + sizeof(Matrix));
    //     sem_t* mut2 = (sem_t*)((char*)ptr + sizeof(Matrix) + sizeof(sem_t));
    //     if (sem_wait(mut2) == -1) {
    //         std::cout << "sem_wait for " << name << " failed with " << strerror(errno) << std::endl;
    //         return 1;
    //     }
    //     ((Matrix*)ptr)->print();
    // }

    // // shut down worker shared memories
    // for (int i = 0; i < NUM_WORKERS; ++i) {
    //     std::string name = "/worker";
    //     name += std::to_string(i);
    //     int fd = worker_fds[i].first;
    //     void* ptr = worker_fds[i].second;
    //     if (munmap(ptr, memSize) == -1) {
    //         std::cout << "munmap for " << name << " failed with " << strerror(errno) << std::endl;
    //         return 1;
    //     }
    //     if (close(fd) == -1) {
    //         std::cout << "close for " << name << " failed with " << strerror(errno) << std::endl;
    //         return 1;
    //     }
    //     if (shm_unlink(name.c_str()) == -1) {
    //         std::cout << "shm_unlink for " << name << " failed with " << strerror(errno) << std::endl;
    //         return 1;
    //     }
    // }
    
    // ShmQueue<int> buf("/name", 50);
    // buf.init();
    // int ctr = 0;
    // // std::cout << "p to push, q to quit: \n";
    // // char c;
    // // while (std::cin >> c) {
    // //     if (c == 'q') {
    // //         break;
    // //     }

    // //     if (!buf.push(ctr++)) {
    // //         std::cout << "--- full buffer ---\n";
    // //         continue;
    // //     }
    // // }

    // std::cout << "enter to start\n";
    // getchar();

    
    auto start = high_resolution_clock::now();
    while (duration_cast<seconds>(high_resolution_clock::now() - start).count() < 5 ) {

    //     if (!buf.push(ctr++)) {
    //         ctr--;
    //         usleep(250);
    //     }
    }

    // std::cout << "ctr: " << ctr << std::endl;

    
    

    return 0;
}
