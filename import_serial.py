import serial
import struct

def main():
    # 사용자에게 포트 입력 받기
    port = input("Enter COM port (e.g., COM3 or /dev/ttyUSB0): ")
    baudrate = 115200
    file_name = "output.txt"

    try:
        # 시리얼 포트 열기
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"Connected to {port} at {baudrate} baud.")

        # CDC 장치로 0x7F 전송
        ser.write(b'\x7F')
        print("Sent 0x7F to the device.")

    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        return

    try:
        with open(file_name, 'a') as f:
            print("Receiving 6144-byte blocks... Press Ctrl+C to stop.")
            while True:
                # 충분히 수신될 때까지 대기
                while ser.in_waiting < 6144:
                    pass

                # 6144바이트 수신
                raw_data = ser.read(6144)

                # 2바이트 signed int로 파싱 (3072개)
                values = struct.unpack('<' + 'h' * 3072, raw_data)

                # 공백으로 구분된 문자열로 변환 후 줄바꿈 추가
                line = ' '.join(str(v) for v in values) + '\n'

                # 출력 및 저장
                print(line, end='')
                f.write(line)

    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        ser.close()

if __name__ == "__main__":
    main()
