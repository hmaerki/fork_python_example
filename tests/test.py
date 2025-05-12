import dynamic_buffer

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

list_separator = [
    0x00,
    0x00,
    0x00,  # 1 measurement
    0x42,  # error bits
    0x43,  # crc
    0x00,  # 1 measurement
]


def dump(db: dynamic_buffer.DynamicBuffer) -> None:
    for i, v in enumerate(db.get_buffer()):
        print(f"{i}: 0x{ord(v):02X}")


def log(
    db: dynamic_buffer.DynamicBuffer,
    expected_size: int,
    msg: str | None = None,
) -> None:
    if msg is not None:
        print(f"Test: {msg}")

    success = db.size() == expected_size
    if not success:
        dump(db=db)
    assert success, (db.size(), expected_size)


def test_normal_data():
    db = dynamic_buffer.DynamicBuffer()

    db.push_bytes(bytes(list_measurements_2))
    log(db, 6, "Not sufficient data")
    numpy_array = db.get_numpy_array()
    assert numpy_array is None, numpy_array
    log(db, 6)

    # db.push_bytes(
    #     bytes(list_measurements_5 + list_separator + list_measurements_5 + list_separator)
    # )
    db.push_bytes(bytes(list_separator))
    log(db, 12, "Just sufficient data")
    numpy_array = db.get_numpy_array()
    assert numpy_array is not None, numpy_array
    log(db, 0)



    print(type(numpy_array))
    print(numpy_array.dtype)
    for measurement_signed in numpy_array:
        REF_V = 5.0
        GAIN = 5.0  # 1.0, 2.0, 5.0, 10.0
        measurement_V = measurement_signed / (2**23) * REF_V / GAIN
        print(measurement_signed, measurement_V)

    # db.push_bytes(
    #     bytes(list_measurements + list_separator + list_measurements + list_separator)
    # )

    numpy_array = db.get_numpy_array()
    print(type(numpy_array))
    print(numpy_array.dtype)
    for measurement_signed in numpy_array:
        REF_V = 5.0
        GAIN = 5.0  # 1.0, 2.0, 5.0, 10.0
        measurement_V = measurement_signed / (2**23) * REF_V / GAIN
        print(measurement_signed, measurement_V)

    print(numpy_array)


if __name__ == "__main__":
    test_normal_data()
