// Include the necessary libraries
#include <SoftwareSerial.h>

// Define the pins for the sensors
const int mq9Pin = A0;
const int mq7Pin = A1;
const int mq135Pin = A2;
const int buzzerPin = 8;  // Change this to the pin where your buzzer is connected
const int ledPin = 13;  // Pin for the LED

// Define the parameters for sensor calibration
const float RL_VALUE = 5.0;  // Resistance of the load resistor in kilohms
const float RO_CLEAN_AIR_MQ9 = 9.83;  // Ro value for clean air (MQ-9)
const float RO_CLEAN_AIR_MQ7 = 20.0;   // Ro value for clean air (MQ-7)
const float RO_CLEAN_AIR_MQ135 = 9.83; // Ro value for clean air (MQ-135)

// Define the threshold for considering the sensors disconnected
const int DISCONNECTED_THRESHOLD = 1;
const float SMOKE_THRESHOLD = 500.0;  // Threshold for smoke detection in ppm

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Set the buzzer pin and LED pin as OUTPUT
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Allow time for the sensors to warm up
  delay(2000);
}

void loop() {
  // Read the analog values from the MQ-9 sensor
  int mq9Value = analogRead(mq9Pin);
  int mq7Value = analogRead(mq7Pin);
  int mq135Value = analogRead(mq135Pin);

  // Check if any sensor value is below the threshold
  if (mq9Value < DISCONNECTED_THRESHOLD || mq7Value < DISCONNECTED_THRESHOLD || mq135Value < DISCONNECTED_THRESHOLD) {
    Serial.println("Sensor disconnected or reading unstable.");

    // Turn off the buzzer and LED
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
  } else {
    // Convert the analog values to voltages
    float mq9Voltage = mq9Value * (5.0 / 1023.0);
    float mq7Voltage = mq7Value * (5.0 / 1023.0);
    float mq135Voltage = mq135Value * (5.0 / 1023.0);

    // Calculate the resistances of the sensors
    float sensorResistanceMQ9 = (5.0 * RL_VALUE) / mq9Voltage - RL_VALUE;
    float sensorResistanceMQ7 = (5.0 * RL_VALUE) / mq7Voltage - RL_VALUE;
    float sensorResistanceMQ135 = (5.0 * RL_VALUE) / mq135Voltage - RL_VALUE;

    // Calculate the ppm concentrations using the formulas for MQ-9, MQ-7, and MQ-135
    float ppmMQ9 = pow(10, ((log10(sensorResistanceMQ9 / RO_CLEAN_AIR_MQ9) / 0.6) + 0.35));
    float ppmMQ7 = pow(10, ((log10(sensorResistanceMQ7 / RO_CLEAN_AIR_MQ7) / 0.3) + 0.47));
    float ppmMQ135 = pow(10, ((log10(sensorResistanceMQ135 / RO_CLEAN_AIR_MQ135) / 0.47) + 0.69));

    // Print sensor values and ppm concentrations to the serial monitor
    Serial.print("MQ-9 - PPM: ");
    Serial.print(ppmMQ9, 1);
    Serial.print(", MQ-7 - PPM: ");
    Serial.print(ppmMQ7, 1);
    Serial.print(", MQ-135 - PPM: ");
    Serial.println(ppmMQ135, 1);

    // Check if all sensor values are above 1000 ppm
    if (ppmMQ9 > SMOKE_THRESHOLD && ppmMQ7 > SMOKE_THRESHOLD && ppmMQ135 > SMOKE_THRESHOLD) {
      Serial.println("Smoke detected.");

      // Trigger the buzzer and LED
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("No smoke detected.");

      // Turn off the buzzer and LED
      digitalWrite(buzzerPin, LOW);
      digitalWrite(ledPin, LOW);
    }
  }

  // Delay for a short period
  delay(1000);
}
