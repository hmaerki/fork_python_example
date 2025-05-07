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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
// https://medium.com/@ahmedfgad/handling-python-numpy-arrays-in-c-using-pybind11-0b7450f4f4b3
#include <pybind11/numpy.h>

namespace py = pybind11;

class DynamicBuffer
{
private:
    std::vector<unsigned char> buffer;
    std::string buf;

public:
    void add_bytes(std::string buf) {
        printf("This is buf:\n");
        printf("  %ld\n", buf.length());
        printf("  %s\n", buf.c_str());
        this->buf = buf;
    }

    py::bytes get_bytes( ) {
        printf("Returning buf\n");
        return this->buf;
    }

    pybind11::array_t<double> get_numpy_array( ) {
        printf("Returning numpy array\n");

        // pybind11::array_t<double> narray = {};

        ssize_t size = 4;
        py::array_t<double> narray(size);
        // Obtain mutable access to the array
        auto r = narray.mutable_unchecked<1>();

        // Populate the array with values
        for (ssize_t i = 0; i < size; i++) {
            r(i) = 1.001001 * static_cast<double>(1+i);
        }

        return narray;
    }

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
    int find_last_zero() const
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

    // Optional: Access the buffer for debugging or other purposes
    const std::vector<unsigned char> &get_buffer() const
    {
        return buffer;
    }
};


PYBIND11_MODULE(dynamic_buffer, m)
{
    py::class_<DynamicBuffer>(m, "DynamicBuffer")
        .def(py::init<>())
        .def("add_bytes", &DynamicBuffer::add_bytes, py::arg("buf"),
             "Adds elements to the buffer.")
        .def("get_bytes", &DynamicBuffer::get_bytes,
             "Return bytesbytes.")

        .def("get_numpy_array", &DynamicBuffer::get_numpy_array,
            "Return numarray.")

        .def("begin_add", &DynamicBuffer::begin_add, py::arg("buf"), py::arg("len"),
             "Adds elements to the buffer.")
        .def("end_free", &DynamicBuffer::end_free, py::arg("len"),
             "Removes elements from the end of the buffer.")
        .def("find_last_zero", &DynamicBuffer::find_last_zero,
             "Finds the last zero in the buffer and returns its index.")
        .def("size", &DynamicBuffer::size,
             "Returns the current size of the buffer.")
        .def("get_buffer", &DynamicBuffer::get_buffer,
             "Returns the current buffer as a list.");
}