#pragma once

#include "shm_queue.hpp"
#include "matrix.hpp"

#include <queue>
#include <optional>

// class instantiated once per worker that manages: 
//   * shared memory holding matrices
//   * consumer queue - indices to matrices for worker to process
//   * producer queue - indices to matrices returned to producer 
// 
// matrixTokens_ contains indices (tokens) to matrices in matrixMem_ that this actor can use. 
// Producer pops a token, writes to it, pushes it onto the consumerQueue
// Consumer pops from consumerQueue, pushes to matrixTokens_. 
//  then when ready, consumer pops from matrixTokens_, writes to it, pushes back onto producerQueue
class MatrixJobQueue {
public:

    MatrixJobQueue(const std::string& workername, uint32_t numItems, bool isProducer) 
            : producerQueue_(workername + "_producer_queue", numItems)
            , consumerQueue_(workername + "_consumer_queue", numItems)
            , matrixMem_(workername + "_matrixmem", sizeof(Matrix) * numItems)
            , numItems_(numItems)
    {
        // producer gets all tokens when starting out
        for (uint32_t i = 0; i < numItems_; ++i) {
            matrixTokens_.push(i);
        }
    }

    // get a matrix from available tokens (local to this process)
    std::optional<std::pair<uint32_t, Matrix*>> popMatrixToken() {
        if (matrixTokens_.empty()) {
            return std::nullopt;
        }

        uint32_t token = matrixTokens_.front();
        Matrix* matrix = (Matrix*)matrixMem_.ptr() + token;
        matrixTokens_.pop();
        return std::make_pair(token, matrix);
    }

    bool pushToConsumer(const std::pair<uint32_t, Matrix*>& token) {
       return consumerQueue_.push(token.first);
    }

    bool pushToProducer(const std::pair<uint32_t, Matrix*>& token) {
        return producerQueue_.push(token.first);
    }

    bool popConsumerQueue() {
        auto opt = consumerQueue_.pop();
        if (opt == std::nullopt) {
            return false;
        }

        matrixTokens_.push(opt.value());
        return true;
    }

    bool popProducerQueue() {
        auto opt = producerQueue_.pop();
        if (opt == std::nullopt) {
            return false;
        }

        matrixTokens_.push(opt.value());
        return true;
    }
private:
    ShmQueue<uint32_t> producerQueue_;
    ShmQueue<uint32_t> consumerQueue_;
    SharedMem matrixMem_;
    std::queue<uint32_t> matrixTokens_;
    uint32_t numItems_;
};
