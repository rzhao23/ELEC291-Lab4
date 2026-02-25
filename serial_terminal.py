import serial
import threading
import sys
import argparse
import time


def reader_thread(ser, stop_event):
    """Continuously read from the serial port and print to stdout."""
    while not stop_event.is_set():
        try:
            n = ser.in_waiting
            if n > 0:
                data = ser.read(n)
                sys.stdout.write(data.decode('ascii', errors='replace'))
                sys.stdout.flush()
            else:
                time.sleep(0.005)
        except serial.SerialException as e:
            sys.stdout.write(f"\n[Serial read error: {e}]\n")
            stop_event.set()
            break


def main():
    parser = argparse.ArgumentParser(description='EFM8 UART terminal')
    parser.add_argument('port',           help='Serial port, e.g. COM3')
    parser.add_argument('--baud', '-b',   type=int, default=115200,
                        help='Baud rate (default: 115200)')
    args = parser.parse_args()

    try:
        ser = serial.Serial(args.port, args.baud, timeout=0)
    except serial.SerialException as e:
        print(f"[Error opening {args.port}: {e}]")
        sys.exit(1)

    print(f"[Connected to {args.port} @ {args.baud} baud — Ctrl+C to exit]\n")

    stop_event = threading.Event()
    t = threading.Thread(target=reader_thread, args=(ser, stop_event), daemon=True)
    t.start()

    try:
        while not stop_event.is_set():
            try:
                cmd = input()          # blocks until user presses Enter
            except EOFError:           # stdin closed (e.g. piped input finished)
                break
            if stop_event.is_set():
                break
            ser.write((cmd + '\r\n').encode('ascii'))
    except KeyboardInterrupt:
        pass
    finally:
        print("\n[Disconnected]")
        stop_event.set()
        ser.close()


if __name__ == '__main__':
    main()
