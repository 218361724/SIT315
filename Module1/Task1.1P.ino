/*
  Module 1 Task 1 (Pass)
  
  Simple Sense-Think-Act Board

  Tuns an LED on/off based on a temperature threshold.
*/

#define LED_PIN 13
#define TMP_SENSOR_PIN A5
#define TMP_SENSOR_VIN 5.0
#define TMP_THRESHOLD = 22

// Environment state
bool isLedOn = false;

void setup()
{
  // Configure pins
  pinMode(TMP_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Open serial communication
  Serial.begin(9600);
}

void loop()
{
  // Sense: read current temperature
  float tempSensorReading = analogRead(TMP_SENSOR_PIN);
  float tempSensorVOut = (tempSensorReading * TMP_SENSOR_VIN) / 1024.0;
  float currentTemp = (tempSensorVOut - 0.5) * 100;
  Serial.print("Sensor input (raw): ");
  Serial.println(tempSensorReading);
  Serial.print("Sensor input (temp c): ");
  Serial.println(currentTemp);
  
  // Think: assess whether temp threshold is being exceeded
  bool prevIsLedOn = isLedOn;
  isLedOn = currentTemp > 22;
  Serial.print("Actuator output: LED ");
  Serial.println(isLedOn ? "On" : "Off");

  // Act: set new LED state when changed
  if(prevIsLedOn != isLedOn)
  {
    digitalWrite(LED_PIN, isLedOn ? HIGH : LOW);
  }

  // Set polling interval to 500ms
  delay(500);
}
