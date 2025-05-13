import serial
import serial.tools.list_ports
import time
import dynamic_buffer

from read_serial import VID, PID, open_serial_port, BUFFER_SIZE


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
        print(f"{len(measurements)=}, duration_s={duration_ns/1e9}")
        print(f"{1e9*counter/duration_ns:0.1f} SPS")

        db.push_bytes(measurements)
        while True:
            numpy_array = db.get_numpy_array()
            if numpy_array is None:
                if counter != counter_after_db:
                    print(
                        f"counter != counter_after_db: {counter} != {counter_after_db}"
                    )
                break
            if db.get_crc() != 0:
                print(f"crc={db.get_crc()}")
            # print(db.get_errors())
            counter_after_db += len(numpy_array)
            for measurement_signed in numpy_array:
                REF_V = 5.0
                GAIN = 5.0  # 1.0, 2.0, 5.0, 10.0
                measurement_V = measurement_signed / (2**23) * REF_V / GAIN
                # print(measurement_signed, measurement_V)


if __name__ == "__main__":
    main()
