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

#define SEPARATOR_0 0
#define SEPARATOR_1 1
#define SEPARATOR_2 2
#define STATUS_ERRORS 3
#define STATUS_CRC 4
#define STATUS_SPARE 5
#define SEPARATOR_SIZE 6

class DynamicBuffer
{
private:
    uint8_t crc = 0xFF;
    uint8_t errors = 0xFF;
    std::vector<unsigned char> buffer;

    // Finds the next separator and returns its index
    // return 0: no separator found
    inline int32_t find_first_separator() const
    {
        int32_t last = buffer.size() - STATUS_SPARE;
        for (int32_t i = 0; i < last; i += 3)
        {
            // Fast early rejection
            if (buffer[i + SEPARATOR_0] | buffer[i + SEPARATOR_1] | buffer[i + SEPARATOR_2] | buffer[i + STATUS_SPARE])
            {
                continue;
            }

            return i;
        }
        return 0; // No separator
    }

    inline int8_t calculate_crc(int32_t index_first_separator) const
    {
        int8_t crc = 0;
        for (int32_t i = 0; i < index_first_separator + STATUS_SPARE; i++)
        {
            crc ^= buffer[i];
        }
        return crc;
    }

public:
    void push_bytes(std::string buf)
    {
        buffer.insert(buffer.end(), buf.begin(), buf.end());
        crc = 0xFF;
        errors = 0xFF;
    }

    // Returns: pybind11::array_t<int32_t>
    py::object get_numpy_array()
    {
        int32_t index_first_separator = find_first_separator();
        if (index_first_separator == 0)
        {
            crc = 0xFF;
            errors = 0xFF;
            // Need more data
            return py::none();
        }

        crc = calculate_crc(index_first_separator);
        errors = buffer[index_first_separator + STATUS_ERRORS];

        if ((index_first_separator % 3) != 0)
        {
            printf("WARNING: index_first_separator=%d which is not a multiple of 3!\n", index_first_separator);
        }
        size_t measurement_count = index_first_separator / 3;
        py::array_t<int32_t> narray(measurement_count);

        // Obtain mutable access to the array
        auto narray_uncheckec = narray.mutable_unchecked<1>();

        // Populate the array with values
        for (size_t i = 0; i < measurement_count; i++)
        {
            int32_t measurement_value = 0;
            for (size_t j = 0; j < 3; j++)
            {
                measurement_value <<= 8;
                measurement_value += buffer[3 * i + j];
            }
            if (measurement_value & 0x800000)
            {
                measurement_value -= 0x1000000;
            }

            narray_uncheckec(i) = static_cast<int32_t>(measurement_value);
        }

        buffer.erase(buffer.begin(), buffer.begin() + index_first_separator + SEPARATOR_SIZE);

        return narray;
    }

    size_t size() const
    {
        return buffer.size();
    }

    py::bytes get_buffer()
    {
        return py::bytes(reinterpret_cast<const char *>(buffer.data()), buffer.size());
    }
    uint8_t get_crc()
    {
        return crc;
    }
    uint8_t get_errors()
    {
        return errors;
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
        .def("size", &DynamicBuffer::size,
             "Returns the current size of the buffer.")
        .def("get_crc", &DynamicBuffer::get_crc,
             "get_crc.")
        .def("get_errors", &DynamicBuffer::get_errors,
             "get_errors.")
        .def("get_buffer", &DynamicBuffer::get_buffer,
             "Returns the buffer.");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}

#ifdef MAIN
int main(int argc, char *argv[])
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
