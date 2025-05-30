import typing
from contextlib import contextmanager

import ad_low_noise_float_2023_decoder

assert ad_low_noise_float_2023_decoder.__version__ == "0.0.1"

ERROR_BITS = 0x42
CRC_BITS = 0x43


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
    ERROR_BITS,
    CRC_BITS,
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

list_odd_chunck_5bytes = [
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
]


def dump(decoder: ad_low_noise_float_2023_decoder.Decoder) -> None:
    x = decoder.get_buffer()
    print(x)
    for i, v in enumerate(decoder.get_buffer()):
        print(f"{i}: 0x{v:02X}")


def expect_size(decoder: ad_low_noise_float_2023_decoder.Decoder, expected_size: int) -> None:
    success = decoder.size() == expected_size
    if not success:
        dump(decoder=decoder)
    assert success, (decoder.size(), expected_size)


def assert_list_equal(numpy_array, list_values):
    assert len(numpy_array) == len(list_values), (len(numpy_array), len(list_values))
    for n, v in zip(numpy_array, list_values):  # noqa: B905
        assert n == v, (n, v)


def assert_crc(decoder: ad_low_noise_float_2023_decoder.Decoder, crc: int):
    assert decoder.get_crc() == crc, (decoder.get_crc(), crc)


@contextmanager
def test_context(
    decoder: ad_low_noise_float_2023_decoder.Decoder,
    expected_size_before: int,
    expected_size_after: int,
    msg: str,
) -> typing.Generator[typing.Any, typing.Any, typing.Any]:
    print(f"Test: {msg}")
    expect_size(decoder, expected_size_before)
    try:
        yield
    finally:
        expect_size(decoder, expected_size_after)
        # print(f"Done: {msg}")


def test_normal_data():
    decoder = ad_low_noise_float_2023_decoder.Decoder()
    assert_crc(decoder, 0xFF)

    decoder.push_bytes(bytes(list_measurements_2))
    with test_context(decoder, 6, 6, "Not sufficient data"):
        numpy_array = decoder.get_numpy_array()
        assert numpy_array is None, numpy_array

    decoder.push_bytes(bytes(list_separator_2))
    with test_context(decoder, 12, 0, "Just sufficient data"):
        numpy_array = decoder.get_numpy_array()
        assert numpy_array is not None, numpy_array
        assert len(numpy_array) == 2
        assert_crc(decoder, 44)
        assert decoder.get_errors() == ERROR_BITS, (decoder.get_errors(), ERROR_BITS)


def test_normal_data_iterator():
    decoder = ad_low_noise_float_2023_decoder.Decoder()
    assert_crc(decoder, 0xFF)

    decoder.push_bytes(
        bytes(
            list_measurements_5
            + list_separator_2
            + list_measurements_2
            + list_separator_2
        )
    )
    with test_context(decoder, 3 * (5 + 2 + 2 + 2), 3 * (2 + 2), "Iterator 1"):
        numpy_array = decoder.get_numpy_array()
        assert_crc(decoder, 156)
        assert_list_equal(numpy_array, list_measurements_5_values)
        # for measurement_signed in numpy_array:
        #     REF_V = 5.0
        #     GAIN = 5.0  # 1.0, 2.0, 5.0, 10.0
        #     measurement_V = measurement_signed / (2**23) * REF_V / GAIN
        #     print(measurement_signed, measurement_V)

    with test_context(decoder, 3 * (2 + 2), 0, "Iterator 2"):
        numpy_array = decoder.get_numpy_array()
        assert_crc(decoder, 44)
        assert_list_equal(numpy_array, list_measurements_2_values)

    with test_context(decoder, 0, 0, "Iterator 3"):
        numpy_array = decoder.get_numpy_array()
        assert_crc(decoder, 0xFF)
        assert numpy_array is None, numpy_array


def test_resync():
    decoder = ad_low_noise_float_2023_decoder.Decoder()
    assert_crc(decoder, 0xFF)

    decoder.push_bytes(
        bytes(
            list_measurements_5
            + list_odd_chunck_5bytes
            + list_separator_2
            + list_measurements_2
            + list_separator_2
        )
    )
    with test_context(decoder, 3 * (5 + 2 + 2 + 2) + 5, 3 * (2 + 2), "Resync 1"):
        numpy_array = decoder.get_numpy_array()
        assert_crc(decoder, 157)
        # Now numpy_array contains the chunck values!
        # assert_list_equal(numpy_array, list_measurements_2_values)

    with test_context(decoder, 3 * (2 + 2), 0, "Resync 2"):
        numpy_array = decoder.get_numpy_array()
        assert_crc(decoder, 44)
        assert_list_equal(numpy_array, list_measurements_2_values)

    with test_context(decoder, 0, 0, "Resync 3"):
        numpy_array = decoder.get_numpy_array()
        assert_crc(decoder, 0xFF)
        assert numpy_array is None, numpy_array


if __name__ == "__main__":
    if True:
        test_normal_data()
        test_normal_data_iterator()
        test_resync()
