import dynamic_buffer
import numpy

assert dynamic_buffer.__version__ == "0.0.1"

db = dynamic_buffer.DynamicBuffer()

list_measurements = [
    0x3F,
    0x30,
    0x06,  # 1 measurement
    0x3F,
    0x2F,
    0x34,  # 1 measurement
    0x3F,
    0x2F,
    0x3C,  # 1 measurement
    0x3F,
    0x2F,
    0x8E,  # 1 measurement
    0x3F,
    0x2F,
    0x27,  # 1 measurement
    0x3F,
    0x2F,
    0x24,  # 1 measurement
    0x3F,
    0x2F,
    0x3C,  # 1 measurement
]

list_separator = [
    0x00,
    0x00,
    0x00,  # 1 measurement
    0x42,  # error bits
    0x43,  # crc
    0x00,  # 1 measurement
]

db.push_bytes(
    bytes(list_measurements + list_separator + list_measurements + list_separator)
)

print("NumpyArray:")
numpy_array = db.get_numpy_array()
print(type(numpy_array))
print(numpy_array.dtype)
for measurement_signed in numpy_array:
    REF_V = 5.0
    GAIN = 5.0  # 1.0, 2.0, 5.0, 10.0
    measurement_V = measurement_signed / (2**23) * REF_V / GAIN
    print(measurement_signed, measurement_V)

print(numpy_array)

