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

#include "matrix.hpp"

int main(int argc, char** argv) {
    std::string name = argv[1];
    int fd = shm_open(name.c_str(), O_RDWR | O_EXCL, 0666);
    if (fd == -1) {
        std::cout << "shm_open for " << name << " failed with " << strerror(errno) << std::endl;
        return 1;
    }

    void* ptr = mmap(0, sizeof(Matrix), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        std::cout << "mmap for " << name << " failed with " << strerror(errno) << std::endl;
        return 1;
    }

    Matrix* mat = (Matrix*)ptr;
    mat->print();
    for (int i = 0; i < mat->dims[0]; ++i) {
        for (int j = 0; j < mat->dims[1]; ++j) {
            mat->data[i][j] = mat->data[i][j] * mat->data[i][j];
        }
    }

    if (munmap(ptr, sizeof(Matrix)) == -1) {
        std::cout << "munmap for " << name << " failed with " << strerror(errno) << std::endl;
        return 1;
    }

    if (close(fd) == -1) {
        std::cout << "close for " << name << " failed with " << strerror(errno) << std::endl;
        return 1;
    }
    return 0;
}
