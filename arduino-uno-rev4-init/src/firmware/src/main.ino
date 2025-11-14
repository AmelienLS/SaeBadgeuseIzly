void setup() {
    // Initialize serial communication at 9600 baud rate
    Serial.begin(9600);
    
    // Additional setup code can be added here
}

void loop() {
    // Main program logic goes here

    // Example: Check if data is available to read
    if (Serial.available() > 0) {
        // Read the incoming byte
        char incomingByte = Serial.read();
        
        // Echo the byte back to the serial monitor
        Serial.print("Received: ");
        Serial.println(incomingByte);
    }

    // Add a small delay to avoid overwhelming the serial output
    delay(100);
}