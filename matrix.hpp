#include <stdint.h>


// const size_t MAX_MATRIX_SIZE = 1024;
const size_t MAX_MATRIX_SIZE = 16;

struct Matrix {
    int32_t data[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    size_t dims[2];

    void print() const {
        for (int i = 0; i < dims[0]; ++i) {
            for (int j = 0; j < dims[1]; ++j) {
                std::cout << data[i][j] << "\t";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
};
