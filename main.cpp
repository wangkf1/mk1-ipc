// linux headers
#include <sys/mman.h>
#include <semaphore.h>

// std lib
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    sem_t sem;
    std::cout << "this is main " << sem_init(&sem, 0, 0) << "\n";
    return 0;
}
