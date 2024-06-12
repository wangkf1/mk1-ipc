# MK1 - IPC + matrix ops

## Design

All communication and matrices are stored in shared memory. A consumer queue and producer queue is set up for each worker, also in shared memory. 

The messages being passed are "tokens", or matrix indices. In the beginning, the main process has all indices stored in its pool (not shared). It is free to write to matrices associated with the tokens. After writing to one, it pushes the token to a worker's consumer queue (randomly picked per the interview requirements). 

The workers pops tokens from the queue and stores it in its own local pool. Then, it can operate on the matrices however it likes. Same as the producer, to send it to the producer, it pushes the token to the producer's queue. 

Finally, the producer can pop the returned tokens and return them to its own pool again for reuse. 

## TODO

 * actually implement this
 * benchmark
 * write down improvement ideas
 