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

int main(int argc, char** argv) {
    // std::string name = argv[1];
    // // currently creating buffer of one matrix + two semaphores
    // size_t memSize = sizeof(Matrix) + 2*sizeof(sem_t);
    // int fd = shm_open(name.c_str(), O_RDWR | O_EXCL, 0666);
    // if (fd == -1) {
    //     std::cout << "shm_open for " << name << " failed with " << strerror(errno) << std::endl;
    //     return 1;
    // }

    // void* ptr = mmap(0, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // if (ptr == MAP_FAILED) {
    //     std::cout << "mmap for " << name << " failed with " << strerror(errno) << std::endl;
    //     return 1;
    // }

    // Matrix* mat = (Matrix*)ptr;
    // sem_t* mut1 = (sem_t*)((char*)ptr + sizeof(Matrix));
    // sem_t* mut2 = (sem_t*)((char*)ptr + sizeof(Matrix) + sizeof(sem_t));
    // struct timespec ts;
    // if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
    //     std::cout << "clock_gettime for " << name << " failed with " << strerror(errno) << std::endl;
    //     return 1;
    // }
    // ts.tv_sec += 3;
    
    // if (sem_timedwait(mut1, &ts) == -1) {
    //     std::cout << "sem_wait for " << name << " failed with " << strerror(errno) << std::endl;
    //     return 1;
    // }
    // mat->print();
    // for (int i = 0; i < mat->dims[0]; ++i) {
    //     for (int j = 0; j < mat->dims[1]; ++j) {
    //         mat->data[i][j] = mat->data[i][j] * mat->data[i][j];
    //     }
    // }
    // if (sem_post(mut2) == -1) {
    //     std::cout << "sem_post for " << name << " failed with " << strerror(errno) << std::endl;
    //     return 1;
    // }

    // if (munmap(ptr, memSize) == -1) {
    //     std::cout << "munmap for " << name << " failed with " << strerror(errno) << std::endl;
    //     return 1;
    // }

    // if (close(fd) == -1) {
    //     std::cout << "close for " << name << " failed with " << strerror(errno) << std::endl;
    //     return 1;
    // }

    // ShmQueue<int> buf("/name", 50);
    // int ctr = 0;
    // int fails = 0;
    // // std::cout << "p to pop, q to quit: \n";
    // // char c;
    // // while (std::cin >> c) {
    // //     if (c == 'q') {
    // //         break;
    // //     }
    // //     auto opt = buf.pop();
    // //     if (opt == std::nullopt) {
    // //         std::cout << "--- empty buffer ---\n";
    // //         continue;
    // //     }
    // //     int val = opt.value();
    // //     std::cout << "--- got " << val << std::endl;
    // // }

    // auto start = high_resolution_clock::now();
    // while (duration_cast<seconds>(high_resolution_clock::now() - start).count() < 6 ) {
    //     auto opt = buf.wait(1);
    //     if (opt == std::nullopt) {
    //         // empty buffer
    //         // usleep(250);
    //         continue;
    //     } else {
    //         if (opt.value() != ctr) {
    //             std::cout << opt.value() << "vs " << ctr << std::endl;
    //             fails++;
    //         }
    //         ctr = opt.value() + 1;
    //     }
    // }

    // std::cout << "fails: " << fails << std::endl;
    // std::cout << "ctr: " << ctr << std::endl;

    return 0;
}
