import numpy


import dynamic_buffer

assert dynamic_buffer.__version__ == "0.0.1"

db = dynamic_buffer.DynamicBuffer()
db.push_bytes(b'Hallo')
# print("Bytes:")
# b = db.get_bytes()
# print(type(b))
# print(b)

print("NumpyArray:")
na = db.get_numpy_array()
print(type(na))
print(na)

# db.begin_add(42, 5)
