#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 2       
#define DHTTYPE DHT11  
#define FAN_PIN 9      
#define STATE_PIN 2    

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial bluetooth(10, 11); 

int manualFanSpeed = -1; 
bool fanEnabled = true;  

void setup() {
    pinMode(FAN_PIN, OUTPUT);
    pinMode(STATE_PIN, INPUT);
    Serial.begin(9600);
    bluetooth.begin(9600);
    dht.begin(); 
}

void loop() {
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    
    if (bluetooth.available()) {
        char command = bluetooth.read();
        if (command == 'A') {
            manualFanSpeed = -1; 
            fanEnabled = true;   
            Serial.println("Automatic control mode");
        } else if (command >= '0' && command <= '9') {
            manualFanSpeed = map(command - '0', 0, 9, 0, 255); 
            fanEnabled = true;   
            Serial.print("Manual control mode, fan speed: ");
            Serial.println(manualFanSpeed);
        } else if (command == 'O') {
            fanEnabled = false;  
            Serial.println("Fan turned off");
        } else if (command == 'E') {
            fanEnabled = true;   
            Serial.println("Fan enabled");
        }
    }

    
    int fanSpeed = 0; 
    if (fanEnabled) {
        fanSpeed = (manualFanSpeed == -1) ? autoControlFanSpeed(temperature) : manualFanSpeed;
    }
    analogWrite(FAN_PIN, fanSpeed);

    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C, Fan Speed: ");
    Serial.println(fanSpeed);

    delay(1000);
}

int autoControlFanSpeed(float temperature) {
    
    if (isnan(temperature)) {
        return 0; 
    }
    
    if (temperature < 25) {
        return 0; 
    } else if (temperature >= 25 && temperature < 35) {
        return map(temperature, 25, 35, 64, 255); 
    } else {
        return 255; 
    }
}
