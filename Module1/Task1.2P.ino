/*
  Module 1 Task 2 (Pass)
  
  Interrupt-driven Board

  Toggles an LED when motion is detected.
*/

#define PIR_PIN 2
#define LED_PIN 3

// Environment state
bool isMotionDetected = false;

void setup()
{
  // Configure pins
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Handle motion detected/stopped
  attachInterrupt(
    digitalPinToInterrupt(PIR_PIN),
    updateEnv,
    CHANGE
  );

  // Open serial communication
  Serial.begin(9600);
}

void loop()
{
  // Do nothing
}

void updateEnv() {
  // Update motion detection state
  isMotionDetected = digitalRead(PIR_PIN) == HIGH;
  
  // Log current button state, and previous LED state
  Serial.print("Sensor input: motion ");
  Serial.println(isMotionDetected ? "detected" : "stopped");

  // Assign new LED state
  digitalWrite(LED_PIN, isMotionDetected ? HIGH : LOW);
  Serial.print("Actuator output: LED ");
  Serial.println(isMotionDetected ? "on" : "off");
}
