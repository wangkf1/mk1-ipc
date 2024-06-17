# MK1 - IPC + matrix ops

## Final Design

All communication and matrices are stored in shared memory. A consumer queue and producer queue is set up for each worker, also in shared memory. 

The messages being passed are "tokens", or matrix indices. In the beginning, the main process has all indices stored in its pool (not shared). It is free to write to matrices associated with the tokens. After writing to one, it pushes the token to a worker's consumer queue (randomly picked per the interview requirements). 

The workers pops tokens from the queue and stores it in its own local pool. Then, it can operate on the matrices however it likes. Same as the producer, to send it to the producer, it pushes the token to the producer's queue. 

Finally, the producer can pop the returned tokens and return them to its own pool again for reuse. 


## Pre-implementation (~3-4 hours)

### First steps

First thing I had to do was refresh my memory with shared memory. I wrote a few programs to test out my circular queue in shared memory, where the main process sent a message whenever I pressed enter. 

### Other potential designs

I considered networking, but given my experience in the past, it might not have been the fastest. Though the message passing would have been a lot cleaner to implement. 

I also played around with a POSIX semaphore as the main way to have inter process waiting on a variable. That didn't work well as I had to use round-robin queues and a bounded queue. So I went back to the atomic head/tail counters and used the semaphore to hold the size of the queue (tail - head). 

## Current implementation (~3-4 hours)

The current implementation does not work reliably. There is a shared memory bug somewhere that is causing a bus error. Maybe it's due to running it in an ubuntu docker on my macbook, but most likely some poor memory management on my end. 

** The bug starts appearing on larger matrix sizes, so for the rest of the doc, I'm going to use the working version where matrices are maximum of 128x128, not 1024x1024. **

### Results

```
Processed 10533055 matrices in 20 seconds
```

## Improvements on this design

Other than fixing the crashes: 

### Multithreading

Each process has a single pool of tokens which are available for processing. Instead of having a single main thread process those tokens serially as they come in, we can have threads pop them off and process them. The main process is spending more time sending out matrices than the workers, so it would benefit more. 

This should be relatively simple to extend as the token pool just needs to be made thread safe and any thread can acquire a token to do work on it. 

### Batching matrices together

Given the max matrix size is 128x128, if two generated matrices can fit inside that buffer, we can batch them together. Then the job queue wouldn't hold matrices, but rather matrix batches. The struct would look something like: 

```
struct {
    int32_t memory[128 * 128]; // single array of matrices
    uint32_t matrices[8][2]; // max 8 matrices in one, 2 dims each
}
```
### Vectorization

Matrix operations are elementwise, so we can vectorize it pretty easily. 

