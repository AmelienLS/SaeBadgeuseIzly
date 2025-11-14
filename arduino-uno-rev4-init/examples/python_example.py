import time
import serial

def main():
    # Configure the serial connection to the Arduino
    arduino = serial.Serial(port='/dev/ttyUSB0', baudrate=9600, timeout=1)
    time.sleep(2)  # Wait for the connection to establish

    # Example of sending data to the Arduino
    arduino.write(b'Hello Arduino!')

    # Example of reading data from the Arduino
    while True:
        if arduino.in_waiting > 0:
            response = arduino.readline().decode('utf-8').rstrip()
            print(f'Received from Arduino: {response}')
            break

    arduino.close()

if __name__ == '__main__':
    main()