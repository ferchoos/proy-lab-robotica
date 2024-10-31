#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 2       // Pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT11 sensor type
#define FAN_PIN 9      // Pin connected to the fan's PWM control
#define STATE_PIN 2    // Pin connected to the STATE pin on Bluetooth

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor
SoftwareSerial bluetooth(10, 11); // RX, TX

int manualFanSpeed = -1; // -1 indicates auto control, 0-255 for manual control
bool fanEnabled = true;  // Variable to track if the fan is enabled or disabled

void setup() {
    pinMode(FAN_PIN, OUTPUT);
    pinMode(STATE_PIN, INPUT);
    Serial.begin(9600);
    bluetooth.begin(9600);
    dht.begin(); // Start the DHT sensor
}

void loop() {
    // Read temperature and humidity
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Check if Bluetooth command is received
    if (bluetooth.available()) {
        char command = bluetooth.read();
        if (command == 'A') {
            manualFanSpeed = -1; // Switch to automatic control
            fanEnabled = true;   // Enable fan in automatic mode
            Serial.println("Automatic control mode");
        } else if (command >= '0' && command <= '9') {
            manualFanSpeed = map(command - '0', 0, 9, 0, 255); // Map 0-9 to 0-255
            fanEnabled = true;   // Enable fan in manual mode
            Serial.print("Manual control mode, fan speed: ");
            Serial.println(manualFanSpeed);
        } else if (command == 'O') {
            fanEnabled = false;  // Turn off the fan
            Serial.println("Fan turned off");
        } else if (command == 'E') {
            fanEnabled = true;   // Enable the fan
            Serial.println("Fan enabled");
        }
    }

    // Fan speed control
    int fanSpeed = 0; // Default to 0 (off)
    if (fanEnabled) {
        fanSpeed = (manualFanSpeed == -1) ? autoControlFanSpeed(temperature) : manualFanSpeed;
    }
    analogWrite(FAN_PIN, fanSpeed);

    // Output status
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C, Fan Speed: ");
    Serial.println(fanSpeed);

    delay(1000);
}

int autoControlFanSpeed(float temperature) {
    // Set fan speed based on temperature
    if (isnan(temperature)) {
        return 0; // If there's a reading error, turn off the fan
    }
    // Map temperature range 25-35°C to fan speed range 64-255
    if (temperature < 25) {
        return 0; // Below 25°C, fan off
    } else if (temperature >= 25 && temperature < 35) {
        return map(temperature, 25, 35, 64, 255); // Gradual increase from 64 to 255
    } else {
        return 255; // Above 35°C, maximum speed
    }
}
