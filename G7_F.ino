#include <DHT.h>
#include <SoftwareSerial.h>
// ----- Pins -----
#define trigFront 4
#define echoFront 5

#define trigRight 6
#define echoRight 7

#define trigLeft 8
#define echoLeft 9

#define smokePin A6

#define DHTPIN A4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define in1 A0
#define in2 A1
#define in3 A2
#define in4 A3

#define ENA 10
#define ENB 11

#define vacuumRelay A5

#define ANALOG_IN_PIN A7

SoftwareSerial bluetooth(13, 12);  // RX, TX

float temp;
float hum;
int smokeValue;
long distFront;
long distRight;
long distLeft;

// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;
// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0;
// Integer for ADC value
int adc_value = 0;
// Float for Reference Voltage                              kbmnjlhvccccc
float ref_voltage = 5.0;

String TxData = "";
int F = 0;

// ----- Setup -----
void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  dht.begin();

  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);

  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);

  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);

  pinMode(smokePin, INPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(vacuumRelay, OUTPUT);
}

// ----- Measure distance -----
long readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);  // Timeout after 30ms
  return duration * 0.034 / 2;             // cm
}

// ----- Loop -----
void loop() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  smokeValue = analogRead(smokePin);
  distFront = readUltrasonic(trigFront, echoFront);
  distRight = readUltrasonic(trigRight, echoRight);
  distLeft = readUltrasonic(trigLeft, echoLeft);
  
  if (distFront > 20 && distLeft > 20 && distRight > 20) {
    Forward();
    F = 0;
  } else if (distFront <= 20 || distRight <= 20 || distLeft <= 20) {
    if (F == 0) {
      Stop();
      delay(1000);
    }
    if (distRight < distLeft) {
      Left();
      delay(500);
    } else if (distLeft < distRight) {
      Right();
      delay(500);
    } else {
      Backward();  // أو random اختيار
      delay(500);
    }
    Stop();
    F = 1;
  }
  // Read the Analog Input
  adc_value = analogRead(ANALOG_IN_PIN);

  // Determine voltage at ADC input
  adc_voltage = (adc_value * ref_voltage) / 1024.0;

  // Calculate voltage at divider input
  in_voltage = adc_voltage * (R1 + R2) / R2;

  Serial_print();
}

void Forward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(ENA, 200);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(ENB, 200);
}

void Right() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(ENA, 150);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(ENB, 200);
}

void Left() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(ENA, 200);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ENB, 150);
}

void Backward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(ENA, 200);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ENB, 200);
}


void Stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(ENA, 0);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(ENB, 0);
}




//Function to Print Data to Serial Monitor
void Serial_print() {
  TxData += temp;
  TxData += ";";
  TxData += hum;
  TxData += ";";
  TxData += smokeValue;
  TxData += ";";
  TxData += ";";
  TxData += distFront;
  TxData += ";";
  TxData += distRight;
  TxData += ";";
  TxData += distLeft;
  TxData += ";";
  TxData +=in_voltage;
  TxData += ";";
  Serial.println(TxData);
  TxData = "";
}

void Bluetooth_print() {
  TxData += temp;
  TxData += ";";
  TxData += hum;
  TxData += ";";
  TxData += smokeValue;
  TxData += ";";
  TxData += distFront;
  TxData += ";";
  TxData += distRight;
  TxData += ";";
  TxData += distLeft;
  TxData += ";";
   TxData +=in_voltage;
  TxData += ";";
  bluetooth.print(TxData);
  TxData = "";
}