#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

#include <vector>
#include <stdexcept>
#include <algorithm>

class DynamicBuffer {
private:
    std::vector<unsigned int> buffer;

public:
    // Adds elements to the buffer
    void begin_add(const unsigned int* buf, size_t len);

    // Removes elements from the end of the buffer
    void end_free(size_t len);

    // Finds the last zero in the buffer and returns its index
    int find_last_zero() const;

    // Optional: Get the current size of the buffer
    size_t size() const;

    // Optional: Access the buffer for debugging or other purposes
    const std::vector<unsigned int>& get_buffer() const;
};

#endif // DYNAMIC_BUFFER_H