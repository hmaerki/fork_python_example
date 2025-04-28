#include <vector>
#include <algorithm>
#include <stdexcept>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

class DynamicBuffer
{
private:
    std::vector<unsigned int> buffer;

public:
    // Adds elements to the buffer
    void begin_add(const unsigned int *buf, size_t len)
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
    const std::vector<unsigned int> &get_buffer() const
    {
        return buffer;
    }
};

namespace py = pybind11;

PYBIND11_MODULE(dynamic_buffer, m)
{
    py::class_<DynamicBuffer>(m, "DynamicBuffer")
        .def(py::init<>())
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