import python_example as m
import numpy

assert m.__version__ == "0.0.1"
assert m.add(1, 2) == 3
assert m.subtract(1, 2) == -1

import dynamic_buffer

db = dynamic_buffer.DynamicBuffer()
db.add_bytes(b'Hallo')
print("Bytes:")
b = db.get_bytes()
print(type(b))
print(b)

print("NumpyArray:")
na = db.get_numpy_array()
print(type(na))
print(na)

db.begin_add(42, 5)
