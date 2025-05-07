import python_example as m

assert m.__version__ == "0.0.1"
assert m.add(1, 2) == 3
assert m.subtract(1, 2) == -1

import dynamic_buffer

db = dynamic_buffer.DynamicBuffer()
db.begin_add(42, 5)
