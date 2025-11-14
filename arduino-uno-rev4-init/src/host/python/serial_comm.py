def send_data(serial_port, data):
    """Send data to the Arduino via the specified serial port."""
    serial_port.write(data.encode())

def receive_data(serial_port, buffer_size=64):
    """Receive data from the Arduino via the specified serial port."""
    if serial_port.in_waiting > 0:
        return serial_port.read(buffer_size).decode()
    return None

def initialize_serial(port, baud_rate=9600):
    """Initialize the serial connection to the Arduino."""
    import serial
    return serial.Serial(port, baud_rate)