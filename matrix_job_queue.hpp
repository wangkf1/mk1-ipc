#pragma once
#include "shm_queue.hpp"
#include "matrix.hpp"

// class instantiated once per worker that manages: 
//   * shared memory holding matrices
//   * producer work queue - indices to matrices that are ready for the worker
//   * consumer done queue - indices to matrices that are done processing 
class MatrixJobQueue {
public:

    MatrixJobQueue(const std::string& workername, uint32_t numItems) 
            : producerQueue_(workername + "_producer_queue", numItems)
            , consumerQueue_(workername + "_consumer_queue", numItems)
            , matrixMem(workername + "_matrixmem", sizeof(Matrix) * numItems)
    {
        

    }
private:
    ShmQueue<int> producerQueue_;
    ShmQueue<int> consumerQueue_;
    SharedMem matrixMem;
};
