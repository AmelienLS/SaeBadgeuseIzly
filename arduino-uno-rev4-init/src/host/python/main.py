import serial
import time

def initialize_serial(port, baudrate=9600):
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(2)  # Wait for the connection to establish
        return ser
    except serial.SerialException as e:
        print(f"Error initializing serial port: {e}")
        return None

def main():
    port = '/dev/ttyUSB0'  # Change this to your Arduino's port
    ser = initialize_serial(port)

    if ser:
        print("Serial connection established.")
        # Example of sending data to Arduino
        ser.write(b'Hello Arduino!')
        
        # Example of reading data from Arduino
        while True:
            if ser.in_waiting > 0:
                response = ser.readline().decode('utf-8').rstrip()
                print(f"Received from Arduino: {response}")
                break

        ser.close()

if __name__ == "__main__":
    main()