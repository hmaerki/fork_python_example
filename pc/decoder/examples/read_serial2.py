import time

import ad_low_noise_float_2023_decoder
from read_serial import BUFFER_SIZE, PID, VID, open_serial_port


def main():
    port = open_serial_port(VID, PID)

    port.write(b"s")
    time.sleep(0.1)
    begin_ns = time.monotonic_ns()
    counter = 0
    counter_after_db = 0
    decoder = ad_low_noise_float_2023_decoder.Decoder()
    while True:
        measurements = port.read(size=BUFFER_SIZE)
        counter += len(measurements) // 3
        duration_ns = time.monotonic_ns() - begin_ns
        print(f"{1e9*counter/duration_ns:0.1f} SPS {len(measurements)=}, duration_s={duration_ns/1e9}")

        decoder.push_bytes(measurements)
        while True:
            numpy_array = decoder.get_numpy_array()
            if numpy_array is None:
                print(
                        f"   counter != counter_after_db: {counter} != {counter_after_db} + {counter-counter_after_db}"
                    )
                break
            if decoder.get_crc() != 0:
                print(f"ERROR crc={decoder.get_crc()}")
            if decoder.get_errors() != 72:
                print(f"ERROR errors={decoder.get_errors()}")
            # print(decoder.get_errors())
            counter_after_db += len(numpy_array)
            for measurement_signed in numpy_array:
                REF_V = 5.0
                GAIN = 5.0  # 1.0, 2.0, 5.0, 10.0
                measurement_V = measurement_signed / (2**23) * REF_V / GAIN
                # print(measurement_signed, measurement_V)


if __name__ == "__main__":
    main()
