#pragma once

#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <unistd.h>

#include <string>
#include <stdexcept>


// wrapper around shared memory. 
// if named memory does not exist, it will be created
// if it already exists, caller is expected to pass the correct numBytes
class SharedMem {
public:
    SharedMem(const std::string& name, size_t numBytes) : name_(name), numBytes_(numBytes + 16) {
        // set up shared mem for queue
        fd_ = shm_open(name.c_str(), O_RDWR | O_CREAT , 0666);
        if (fd_ == -1) {
            throw std::runtime_error(strerror(errno));
        }

        if (ftruncate(fd_, numBytes_) == -1) {
            throw std::runtime_error(strerror(errno));
        }

        mem_ = mmap(0, numBytes_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
        if (mem_ == MAP_FAILED) {
            throw std::runtime_error(strerror(errno));
        }

        // aligned ptr to 16 bytes
        ptr_ = (void*)(((uintptr_t)mem_ + 15) & ~15);
        std::cout << "memory is from " << ptr_ << " to " << (void*)((char*)ptr_ + numBytes) << "\n";
    }

    ~SharedMem() {
        munmap(mem_, numBytes_);
        close(fd_);
        shm_unlink(name_.c_str());
    }

    void* ptr() {
        // std::cout << "ptr value: " << ptr_ << std::endl;
        return ptr_;
    }
private:
    std::string name_;
    size_t numBytes_;
    void* ptr_;
    void* mem_;
    int fd_;

};
