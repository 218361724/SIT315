/*
  Module 1 Task 3 (Credit)
  
  Multiple-Inputs Board

  Uses 2 PIR motion sensors to toggle either Red/Green LEDs
  when motion is detected.
*/

#define PIR_1_PIN 2
#define PIR_2_PIN 3
#define LED_RED_PIN 4
#define LED_GREEN_PIN 5

// Environment state
bool isMotionDetected1 = false;
bool isMotionDetected2 = false;

void setup()
{
  // Configure pins
  pinMode(PIR_1_PIN, INPUT);
  pinMode(PIR_2_PIN, INPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);

  // Handle motion detected/stopped
  attachInterrupt(
    digitalPinToInterrupt(PIR_1_PIN),
    updateMotion1,
    CHANGE
  );
  attachInterrupt(
    digitalPinToInterrupt(PIR_2_PIN),
    updateMotion2,
    CHANGE
  );

  // Open serial communication
  Serial.begin(9600);
}

void loop()
{
  // Do nothing
}

void updateMotion1() {
  // Update motion detection state
  isMotionDetected1 = digitalRead(PIR_1_PIN) == HIGH;
  
  // Log current button state, and previous LED state
  Serial.print("Sensor 1 input: motion ");
  Serial.println(isMotionDetected1 ? "detected" : "stopped");

  // Assign new LED state
  digitalWrite(LED_RED_PIN, isMotionDetected1 ? HIGH : LOW);
  Serial.print("Actuator 1 output: Red LED ");
  Serial.println(isMotionDetected1 ? "on" : "off");
}

void updateMotion2() {
  // Update motion detection state
  isMotionDetected2 = digitalRead(PIR_2_PIN) == HIGH;
  
  // Log current button state, and previous LED state
  Serial.print("Sensor 2 input: motion ");
  Serial.println(isMotionDetected2 ? "detected" : "stopped");

  // Assign new LED state
  digitalWrite(LED_GREEN_PIN, isMotionDetected2 ? HIGH : LOW);
  Serial.print("Actuator 2 output: Green LED ");
  Serial.println(isMotionDetected2 ? "on" : "off");
}
