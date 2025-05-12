/*
front/back: https://www.programiz.com/cpp-programming/vectors

https://docs.python.org/3/library/queue.html
  * put() Add to front
  * get() Get from end

Interface
  * put(buf: bytes)
  * get()
    None: No data yet
    status byte, numpy array
      In this case, the buffer is shrinked. memove?

Loop over 3 bytes.
Update CRC.

Convert values for numpy, byte order.
return buffer in format which fits for numpy.

Where to calculate gain? numpy?

dynamic buffer -> unsigned int 32 bit -> numpy (32bit) -> numpy (float)


z = np.arange(3, dtype=np.uint8)
z

https://numpy.org/doc/stable/reference/arrays.scalars.html#numpy.uint32
https://numpy.org/doc/stable/reference/c-api/dtype.html#c.npy_uint32

*/
#include <vector>
#include <algorithm>
#include <stdexcept>

#ifdef MAIN
#include <iostream>
#endif // MAIN

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
// https://medium.com/@ahmedfgad/handling-python-numpy-arrays-in-c-using-pybind11-0b7450f4f4b3
#include <pybind11/numpy.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

class DynamicBuffer
{
private:
    std::vector<unsigned char> buffer;

public:
    void push_bytes(std::string buf)
    {
        printf("This is buf:\n");
        printf("  %ld\n", buf.length());
        printf("  %s\n", buf.c_str());
        buffer.insert(buffer.begin(), buf.begin(), buf.end());
    }

    pybind11::array_t<uint32_t> get_numpy_array()
    {
        printf("Returning numpy array\n");

        // pybind11::array_t<double> narray = {};

        size_t size = 4;
        py::array_t<uint32_t> narray(size);
        // Obtain mutable access to the array
        auto r = narray.mutable_unchecked<1>();

        // Populate the array with values
        for (size_t i = 0; i < size; i++)
        {
            r(i) = 1.001001 * static_cast<uint32_t>(1 + i);
        }

        return narray;
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
        auto it = std::find(buffer.rbegin(), buffer.rend(), 0);
        if (it == buffer.rend())
        {
            return -1; // Return -1 if no zero is found
        }
        return std::distance(buffer.begin(), it.base()) - 1;
    }

    // Optional: Get the current size of the buffer
    size_t size() const
    {
        return buffer.size();
    }
};

PYBIND11_MODULE(dynamic_buffer, m)
{
    py::class_<DynamicBuffer>(m, "DynamicBuffer")
        .def(py::init<>())
        .def("push_bytes", &DynamicBuffer::push_bytes, py::arg("buf"),
             "Adds elements to the begin of the buffer.")
        .def("get_numpy_array", &DynamicBuffer::get_numpy_array,
             "Return numarray.")
        .def("end_free", &DynamicBuffer::end_free, py::arg("len"),
             "Removes elements from the end of the buffer.")
        .def("find_last_zero", &DynamicBuffer::find_last_zero,
             "Finds the last zero in the buffer and returns its index.")
        .def("size", &DynamicBuffer::size,
             "Returns the current size of the buffer.");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}


#ifdef MAIN
int main(int argc, char* argv[])
{
    DynamicBuffer buffer;
    {
        const unsigned char buf[] = {0, 1, 2, 3, 4, 5};
        std::string str_buf(reinterpret_cast<const char *>(buf), sizeof(buf));
        buffer.push_bytes(str_buf);
    }

    {
        const unsigned char buf[] = {6, 7, 8, 9, 10, 11};
        std::string str_buf(reinterpret_cast<const char *>(buf), sizeof(buf));
        buffer.push_bytes(str_buf);
    }

    {
        const unsigned char buf[] = {0x12, 0x13, 0, 0, 0, 17, 18, 19};
        std::string str_buf(reinterpret_cast<const char *>(buf), sizeof(buf));
        buffer.push_bytes(str_buf);
    }

    /*
    const auto &buf = buffer.get_numpy_array();
    std::cout << "Buffer contents: ";
    for (unsigned char byte : buf)
    {
        std::cout << "0x" << std::hex << static_cast<int>(byte) << " ";
    }
    */
    std::cout << std::dec << std::endl; // Reset to decimal format
    std::cout << "size: " << buffer.size() << std::endl;
    std::cout << "find_last_zero: " << buffer.find_last_zero() << std::endl;
    return 0;
}
#endif
