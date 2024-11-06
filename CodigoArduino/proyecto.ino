#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 2       
#define DHTTYPE DHT11  
#define FAN_PIN 9      

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial bluetooth(6, 5); 

int manualFanSpeed = -1; 
bool fanEnabled = true;  

void setup() {
    pinMode(FAN_PIN, OUTPUT);
    Serial.begin(9600);       
    bluetooth.begin(9600);    
    dht.begin(); 
}

void loop() {
    
    float temperature = dht.readTemperature();

    if (bluetooth.available()) {
        char command = bluetooth.read();
        if (command == 'A') {             // AUTOMATICO
            manualFanSpeed = -1;         
            fanEnabled = true;
            bluetooth.println("Auto");
        } else if (command == 'M') {      // MAXIMO
            manualFanSpeed = 255;         
            fanEnabled = true;
            bluetooth.println("Max");
        } else if (command == 'L') {      // BAJO
            manualFanSpeed = 64;          
            fanEnabled = true;
            bluetooth.println("Low ");
        } else if (command == 'O') {      // APAGAR
            fanEnabled = false;           
            bluetooth.println("off");
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

