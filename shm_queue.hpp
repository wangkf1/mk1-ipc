#pragma once
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#include <string>
#include <atomic>
#include <stdexcept>
#include <optional>
#include <iostream>

#include "shm_wrapper.hpp"

// Queue that operates in shared memory
// atomic counters for tail/head
// todo: wait/timeout functionality
template <typename T>
class ShmQueue {
public:
    ShmQueue(const std::string& name, uint32_t numItems) 
            : maxSize_(numItems)
            , memSize_(sizeof(Internals) + sizeof(T) * numItems)
            , mem_(name, memSize_) 
    {
        ctrs_ = (Internals*)mem_.ptr();
        data_ = (T*)((char*)mem_.ptr() + sizeof(Internals));
        // std::cout << "required alignment " << std::atomic_ref<uint32_t>::required_alignment << "\n";
        std::cout << "sizeof internals " << sizeof(Internals) << ", sizeof items: " << sizeof(T) << " * num items (" << numItems << ")" << "\n";
    }
    void init() {
        ctrs_->head = 0;
        ctrs_->tail = 0;
        ctrs_->full = false;

        if (sem_init(&ctrs_->sem, 1, 0) == -1) {
            throw std::runtime_error(strerror(errno));
        }
    }

    bool full() const {
        return ctrs_->full;
    }

    bool push(const T& val) {
        if (ctrs_->full) {
            return false;
        }

        // set val
        data_[ctrs_->tail] = val;

        // increment tail
        ctrs_->tail = (ctrs_->tail+1) % maxSize_;
        ctrs_->full = (ctrs_->tail == ctrs_->head);

        if (sem_post(&ctrs_->sem) == -1) {
            throw std::runtime_error(strerror(errno));
        }
        // std::cout << "end of push: tail: " << ctrs_->tail << ", head: " << ctrs_->head << std::endl;
        return true;
    }

    bool empty() {
        return 
            !
            ctrs_->full && 
            (ctrs_->head == ctrs_->tail);
    }

    std::optional<T> wait(int64_t msecs) {
        if (msecs == 0) {
            if (sem_trywait(&ctrs_->sem) == -1) {
                return std::nullopt;
            }
        } else {
            struct timespec ts;
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                throw std::runtime_error(strerror(errno));
            }
            ts.tv_nsec += msecs * 1000000;
            if (sem_timedwait(&ctrs_->sem, &ts) == -1) {
                return std::nullopt;
            }
        }

        return pop();
    }
    std::optional<T> pop() {
        // std::cout << "start of pop: tail: " << ctrs_->tail << ", head: " << ctrs_->head << std::endl;
        if (empty()) {
            return std::nullopt;
        }

        // get val
        const T val = data_[ctrs_->head];

        // increment head
        ctrs_->full = false;
        ctrs_->head = (ctrs_->head+1) % maxSize_;

        return val;
    }

private:
    // data stored in shared mem
    struct Internals { 
        std::atomic_uint32_t head; // position of next item to be consumed
        bool full;
        char filler[11];           // buffer to reach 16 bytes alignment for atomics
        std::atomic_uint32_t tail; // position of next open slot to fill
        sem_t sem;                 // semaphore for count and waiting
    };
    Internals* ctrs_;
    T* data_;

    uint32_t maxSize_;
    size_t memSize_;
    SharedMem mem_;
};
