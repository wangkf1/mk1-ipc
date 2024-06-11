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
#include <vector>

// interview specific headers/consts
#include "matrix.hpp"

constexpr int NUM_WORKERS = 2;

// random dims (up to 1024x1024) and random data
void generateMatrix(Matrix* matrix) {
    matrix->dims[0] = rand() % MAX_MATRIX_SIZE;
    matrix->dims[1] = rand() % MAX_MATRIX_SIZE;
    for (int i = 0; i < matrix->dims[0]; ++i) {
        for (int j = 0; j < matrix->dims[1]; ++j) {
            matrix->data[i][j] = rand() % 1024;
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

    std::cout << "Setting up shared memory... \n";
    std::vector<int> worker_fds(4);
    for (int i = 0; i < NUM_WORKERS; ++i) {
        std::cout << "worker " << i << ": \n";
        std::string name = "/worker";
        name += std::to_string(i);
        int fd = shm_open(name.c_str(), O_RDWR | O_CREAT , 0666);
        if (fd == -1) {
            std::cout << "shm_open for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }
        worker_fds[i] = fd;

        if (ftruncate(fd, sizeof(Matrix)) == -1) {
            std::cout << "ftruncate for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }

        void* ptr = mmap(0, sizeof(Matrix), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            std::cout << "mmap for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }

        generateMatrix((Matrix*)ptr);
    }

    std::cout << "After launching " << NUM_WORKERS << " workers, press enter to continue.";
    getchar(); 

    for (int i = 0; i < NUM_WORKERS; ++i) {
        std::string name = "shmem_worker";
        name += std::to_string(i);
        int fd = worker_fds[i];
        void* ptr = mmap(0, sizeof(Matrix), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            std::cout << "mmap for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }

        ((Matrix*)ptr)->print();


        if (munmap(ptr, sizeof(Matrix)) == -1) {
            std::cout << "munmap for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }
        if (close(fd) == -1) {
            std::cout << "close for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }
        if (shm_unlink(name.c_str()) == -1) {
            std::cout << "shm_unlink for " << name << " failed with " << strerror(errno) << std::endl;
            return 1;
        }
    }

    
    return 0;
}
