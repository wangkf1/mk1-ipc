#include "shm_queue.hpp"
#include <iostream>

int main() {
    ShmQueue<int> buf("/name", 5);
        buf.push(1);
        buf.push(2);
        buf.push(3);
        buf.push(4);
        buf.push(5);
    {
        auto fail = buf.push(6);
        std::cout << "should fail: " << fail<< std::endl;
    }
    {
        auto opt = buf.pop();
        std::cout << "pop " << opt.value() << std::endl;
    }
    {
        auto opt = buf.pop();
        std::cout << "pop " << opt.value() << std::endl;
    }
    {
        auto opt = buf.pop();
        std::cout << "pop " << opt.value() << std::endl;
    }
    {
        auto opt = buf.pop();
        std::cout << "pop " << opt.value() << std::endl;
    }
    {
        auto opt = buf.pop();
        std::cout << "pop " << opt.value() << std::endl;
    }
    {
        auto val = buf.pop();
        std::cout << "got nullopt: " << (val == std::nullopt )<< std::endl;
    }
    return 0;
}