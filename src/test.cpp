#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>

class DynamicBuffer
{
private:
    std::vector<unsigned char> buffer;

public:
    // Adds elements to the buffer
    void begin_add(const unsigned char *buf, size_t len)
    {
        if (!buf || len == 0)
        {
            throw std::invalid_argument("Invalid buffer or length.");
        }
        buffer.insert(buffer.end(), buf, buf + len);
    }

    // Removes elements from the end of the buffer
    void end_free(size_t len)
    {
        if (len > buffer.size())
        {
            throw std::out_of_range("Length to free exceeds buffer size.");
        }
        buffer.erase(buffer.end() - len, buffer.end());
    }

    // Finds the last zero in the buffer and returns its index
    size_t find_last_zero() const
    {
        const unsigned char *data = buffer.data();
        size_t n = buffer.size();
        for (size_t i = n - 1; i >= 2; --i)
        {
            if (data[i] == 0 && data[i - 1] == 0 && data[i - 2] == 0)
            {
            return i - 2; // Return the starting index of the three consecutive zeros
            }
        }
        return -1; // Return -1 if no three consecutive zeros are found
        // auto it = std::find(buffer.rbegin(), buffer.rend(), 0);
        // if (it == buffer.rend())
        // {
        //     return -1; // Return -1 if no zero is found
        // }
        // return std::distance(buffer.begin(), it.base()) - 1;
    }

    // Optional: Get the current size of the buffer
    size_t size() const
    {
        return buffer.size();
    }

    // Optional: Access the buffer for debugging or other purposes
    const std::vector<unsigned char> &get_buffer() const
    {
        return buffer;
    }
};

int main()
{
    std::cout << "Hello, World!" << std::endl;

    DynamicBuffer buffer;
    const unsigned char buf1[] = {0, 1, 2, 3, 4, 5};
    size_t len1 = 6;
    buffer.begin_add(buf1, len1);

    const unsigned char buf2[] = {6, 7, 8, 9, 10, 11};
    size_t len2 = 6;
    buffer.begin_add(buf2, len2);

    const unsigned char buf3[] = {0x12, 0x13, 0, 0, 0, 17, 18, 19};
    size_t len3 = 8;
    buffer.begin_add(buf3, len3);

    const auto &buf = buffer.get_buffer();
    std::cout << "Buffer contents: ";
    for (unsigned char byte : buf)
    {
        std::cout << "0x" << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl; // Reset to decimal format
    std::cout << "size: " << buffer.size() << std::endl;
    std::cout << "find_last_zero: " << buffer.find_last_zero() << std::endl;
    return 0;
}