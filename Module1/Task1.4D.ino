/*
  Module 1 Task 4 (Distinction)
  
  More-Inputs-Timer Board

  Uses 3 PIR motion sensors to toggle Red/Blue/Green LEDs
  as well as a timer interrupt to blink Yellow LED.
*/

#define PIR_1_PIN 2
#define PIR_2_PIN 3
#define PIR_3_PIN 13
#define LED_YELLOW_PIN 7
#define LED_RED_PIN 6
#define LED_BLUE_PIN 5
#define LED_GREEN_PIN 4

// Environment state
bool isMotionDetected1 = false;
bool isMotionDetected2 = false;
bool isMotionDetected3 = false;

void setup()
{
  // Configure pins
  pinMode(PIR_1_PIN, INPUT);
  pinMode(PIR_2_PIN, INPUT);
  pinMode(PIR_3_PIN, INPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);

  /* Attach arduino hardware interrupts for
   motion detected/stopped on PIR 1 and 2 */
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
  
  /* Setup pin change interrupt for motion
   detected/stopped on PIR 3 */
  PCICR |= B00000001; // Port B
  PCMSK0 |= B00100000; // Pin 13
  
  // Setup timer to blink yellow LED every second
  startTimer(1);

  // Open serial communication
  Serial.begin(9600);
}

void loop()
{
  // Do nothing
}

// Hardware interrupt: Handle motion PIR sensor 1
void updateMotion1() {
  // Update motion detection state
  isMotionDetected1 = digitalRead(PIR_1_PIN) == HIGH;
  
  // Log current button state, and previous LED state
  Serial.print("Sensor 1 input: motion ");
  Serial.println(isMotionDetected1 ? "detected" : "stopped");

  // Assign new red LED state
  digitalWrite(LED_RED_PIN, isMotionDetected1 ? HIGH : LOW);
  Serial.print("Actuator 1 output: Red LED ");
  Serial.println(isMotionDetected1 ? "on" : "off");
}

// Hardware interrupt: Handle motion PIR sensor 2
void updateMotion2() {
  // Update motion detection state
  isMotionDetected2 = digitalRead(PIR_2_PIN) == HIGH;
  
  // Log current button state, and previous LED state
  Serial.print("Sensor 2 input: motion ");
  Serial.println(isMotionDetected2 ? "detected" : "stopped");

  // Assign new blue LED state
  digitalWrite(LED_BLUE_PIN, isMotionDetected2 ? HIGH : LOW);
  Serial.print("Actuator 2 output: Blue LED ");
  Serial.println(isMotionDetected2 ? "on" : "off");
}

// Pin change interrupt: Handle motion PIR sensor 3
ISR(PCINT0_vect) {
  // Update motion detection state
  isMotionDetected3 = digitalRead(PIR_3_PIN) == HIGH;

  // Log current button state, and previous LED state
  Serial.print("Sensor 3 input: motion ");
  Serial.println(isMotionDetected3 ? "detected" : "stopped");

  // Assign new green LED state
  digitalWrite(LED_GREEN_PIN, isMotionDetected3 ? HIGH : LOW);
  Serial.print("Actuator 3 output: Green LED ");
  Serial.println(isMotionDetected3 ? "on" : "off");
}

// Timer interrupt: blink yellow LED
ISR(TIMER1_COMPA_vect){
  Serial.print("Timer interrupt: Yellow LED ");
  Serial.println(digitalRead(LED_YELLOW_PIN) ? "off" : "on");
  digitalWrite(LED_YELLOW_PIN, !digitalRead(LED_YELLOW_PIN));
}

// Utility: add timer interrupt
void startTimer(double timerFrequency)
{
  // timerFrequency = (clock frequency / ((timerTarget + 1) * prescale))
  // timerFrequency = (16000000 / ((timerTarget + 1) * 1024))
  uint16_t timerTarget = (15625 / timerFrequency) - 1;

  noInterrupts();

  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Set timer compare
  OCR1A = timerTarget;
  
  // Prescaler 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);

  // CTC
  TCCR1B |= (1 << WGM12);

  interrupts();
}
