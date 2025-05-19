import time

import dynamic_buffer
from read_serial import BUFFER_SIZE, PID, VID, open_serial_port


def main():
    port = open_serial_port(VID, PID)

    port.write(b"s")
    time.sleep(0.1)
    begin_ns = time.monotonic_ns()
    counter = 0
    counter_after_db = 0
    db = dynamic_buffer.DynamicBuffer()
    while True:
        measurements = port.read(size=BUFFER_SIZE)
        counter += len(measurements) // 3
        duration_ns = time.monotonic_ns() - begin_ns
        print(f"{1e9*counter/duration_ns:0.1f} SPS {len(measurements)=}, duration_s={duration_ns/1e9}")

        db.push_bytes(measurements)
        while True:
            numpy_array = db.get_numpy_array()
            if numpy_array is None:
                print(
                        f"   counter != counter_after_db: {counter} != {counter_after_db} + {counter-counter_after_db}"
                    )
                break
            if db.get_crc() != 0:
                print(f"ERROR crc={db.get_crc()}")
            if db.get_errors() != 72:
                print(f"ERROR errors={db.get_errors()}")
            # print(db.get_errors())
            counter_after_db += len(numpy_array)
            for measurement_signed in numpy_array:
                REF_V = 5.0
                GAIN = 5.0  # 1.0, 2.0, 5.0, 10.0
                measurement_V = measurement_signed / (2**23) * REF_V / GAIN
                # print(measurement_signed, measurement_V)


if __name__ == "__main__":
    main()
