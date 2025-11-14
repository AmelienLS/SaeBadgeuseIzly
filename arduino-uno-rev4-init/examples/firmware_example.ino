void setup() {
    // Initialize serial communication at 9600 baud rate
    Serial.begin(9600);
}

void loop() {
    // Check if data is available to read
    if (Serial.available() > 0) {
        // Read the incoming byte
        char incomingByte = Serial.read();
        
        // Echo the byte back to the serial monitor
        Serial.print("I received: ");
        Serial.println(incomingByte);
    }
    
    // Add a small delay to avoid overwhelming the serial monitor
    delay(100);
}