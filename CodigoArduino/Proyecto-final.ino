#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 2         
#define DHTTYPE DHT11    
#define MOTOR_PIN 11    
#define BLUETOOTH_RX 5   
#define BLUETOOTH_TX 4   
#define BLUETOOTH_STATE 3 

SoftwareSerial bluetooth(BLUETOOTH_RX, BLUETOOTH_TX); 
DHT dht(DHTPIN, DHTTYPE);

int motorSpeed = 0; 
char command = 'O'; 

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BLUETOOTH_STATE, INPUT);
  bluetooth.begin(9600); 
  Serial.begin(9600);    
  dht.begin();
  Serial.println("System Ready. Waiting for Bluetooth commands...");
}

void loop() {
  
  if (digitalRead(BLUETOOTH_STATE) == HIGH) {
    if (bluetooth.available()) {
      command = bluetooth.read();  
      Serial.print("Comando recibido: ");
      Serial.println(command);
      handleCommand(command);
    }
  } 

  
  if (command == 'A') {
    float temperature = dht.readTemperature();
    if (isnan(temperature)) {
      Serial.println("Error leyendo temperatura");
      return;
    }

    
    motorSpeed = map(constrain(temperature, 20, 30), 20, 30, 100, 255);
    analogWrite(MOTOR_PIN, motorSpeed);
    Serial.print("Modo automatico: Temperatura = ");
    Serial.print(temperature);
    Serial.print("°C, Velocidad del ventilador= ");
    Serial.println(motorSpeed);
  }

  delay(100); 
}

void handleCommand(char cmd) {
  switch (cmd) {
    case 'M': // Max speed
      motorSpeed = 255;
      analogWrite(MOTOR_PIN, motorSpeed);
      Serial.println("Modo Maxima Velocidad.");
      break;

    case 'L': // Low speed
      motorSpeed = 100;
      analogWrite(MOTOR_PIN, motorSpeed);
      Serial.println("Modo Baja Velocidad.");
      break;

    case 'O': // Off
      motorSpeed = 0;
      analogWrite(MOTOR_PIN, motorSpeed);
      Serial.println("Ventilador Apagado.");
      break;

    case 'A': // Automatic mode
      Serial.println("Modo Automatico activado.");
      break;

    default:
      Serial.println("Comando desconocido");
  }

  
  if (cmd != 'O') {
    analogWrite(MOTOR_PIN, motorSpeed);
  }
}

