#include <iostream>

#include "dynamic_buffer.h"

int main() {
    DynamicBuffer db;

    unsigned int data[] = {1, 2, 0, 4, 5};
    db.begin_add(data, 5);

    std::cout << "Last zero index: " << db.find_last_zero() << std::endl;

    db.end_free(2);
    std::cout << "Buffer size after freeing: " << db.size() << std::endl;

    return 0;
}