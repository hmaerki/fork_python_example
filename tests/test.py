import typing
import dynamic_buffer
from contextlib import contextmanager

assert dynamic_buffer.__version__ == "0.0.1"

list_measurements_2 = [
    0x3F,
    0x30,
    0x06,  # 1 measurement
    0x3F,
    0x2F,
    0x34,  # 1 measurement
]


list_measurements_5 = [
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

list_separator_2 = [
    0x00,
    0x00,
    0x00,  # 1 measurement
    0x42,  # error bits
    0x43,  # crc
    0x00,  # 1 measurement
]

list_measurements_2_values = [
    4141062,
    4140852,
]

list_measurements_5_values = [
    4140860,
    4140942,
    4140839,
    4140836,
    4140860,
]


def dump(db: dynamic_buffer.DynamicBuffer) -> None:
    x = db.get_buffer()
    print(x)
    for i, v in enumerate(db.get_buffer()):
        print(f"{i}: 0x{v:02X}")


def expect_size(db: dynamic_buffer.DynamicBuffer, expected_size: int) -> None:
    success = db.size() == expected_size
    if not success:
        dump(db=db)
    assert success, (db.size(), expected_size)

def assert_list_equal(numpy_array, list_values):
    assert len(numpy_array) == len(list_values), (len(numpy_array), len(list_values))
    for n, v in zip(numpy_array, list_values, strict=True):
        assert n == v, (n, v)


@contextmanager
def test_context(
    db: dynamic_buffer.DynamicBuffer,
    expected_size_before: int,
    expected_size_after: int,
    msg: str,
) -> typing.Generator[typing.Any, typing.Any, typing.Any]:
    print(f"Test: {msg}")
    expect_size(db, expected_size_before)
    try:
        yield
    finally:
        expect_size(db, expected_size_after)
        # print(f"Done: {msg}")



def test_normal_data():
    db = dynamic_buffer.DynamicBuffer()

    db.push_bytes(bytes(list_measurements_2))
    with test_context(db, 6, 6, "Not sufficient data"):
        numpy_array = db.get_numpy_array()
        assert numpy_array is None, numpy_array

    db.push_bytes(bytes(list_separator_2))
    with test_context(db, 12, 0, "Just sufficient data"):
        numpy_array = db.get_numpy_array()
        assert numpy_array is not None, numpy_array
        assert len(numpy_array) == 2


def test_normal_data_iterator():
    db = dynamic_buffer.DynamicBuffer()

    db.push_bytes(
        bytes(
            list_measurements_5
            + list_separator_2
            + list_measurements_2
            + list_separator_2
        )
    )
    with test_context(db, 3 * (5 + 2 + 2 + 2), 3 * (2 + 2), "iterator 1"):
        numpy_array = db.get_numpy_array()
        assert_list_equal(numpy_array, list_measurements_5_values)
        # for measurement_signed in numpy_array:
        #     REF_V = 5.0
        #     GAIN = 5.0  # 1.0, 2.0, 5.0, 10.0
        #     measurement_V = measurement_signed / (2**23) * REF_V / GAIN
        #     print(measurement_signed, measurement_V)

    with test_context(db, 3 * (2 + 2), 0, "iterator 2"):
        numpy_array = db.get_numpy_array()
        assert_list_equal(numpy_array, list_measurements_2_values)


if __name__ == "__main__":
    if True:
        test_normal_data()
        test_normal_data_iterator()
