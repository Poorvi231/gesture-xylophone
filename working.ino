#include <Stepper.h>
#include <Servo.h>

// Stepper motor configuration
const int STEPS_PER_REV = 2048;  // for 28BYJ-48
Stepper stepper(STEPS_PER_REV, 8, 10, 9, 11);

// Servo configuration
Servo mallet;
const int servoPin = 6;

// Position constants for Cycle A
const int STEPS_30_DEG = (STEPS_PER_REV / 360.0) * 30;  // ~171 steps
const int STEPS_25_DEG = (STEPS_PER_REV / 360.0) * 25;  // ~142 steps

// Position constants for Cycle B
const int POS_O = 0;     // Origin
const int POS_F = 910;   // 160 degrees
const int POS_E = 768;   // 135 degrees
const int POS_C = 483;   // 85 degrees
const int POS_D = 625;   // 110 degrees
// Using the already defined constants for positions
const int POS_A = STEPS_30_DEG;  // Same as first position in Cycle A
const int POS_B = POS_A + STEPS_25_DEG;  // Same as second position in Cycle A

// Servo positions
const int SERVO_POS_G = 45;   // Rest position
const int SERVO_POS_H = 15;  // Strike position

// Timing constants for Cycle B
const int STANDARD_HIT = 200;   // Standard hit duration
const int EXTENDED_HIT = 400;   // Extended hit duration
const int B_POSITION_HOLD = 500; // B position hold duration
const int SEQUENCE_PAUSE = 40;   // Pause between sequences
const int SEQUENCE_PAUS = 70;   // Pause between sequence

// State tracking
int currentPosition = 0;
bool isCycleRunning = false;
char currentCycle = 'N';  // 'N' for None, 'A' for Cycle A, 'B' for Cycle B

void setup() {
  // Initialize servo
  mallet.attach(servoPin);
  mallet.write(SERVO_POS_G);
  
  // Set stepper speed
  stepper.setSpeed(12);
  
  // Start serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Send ready signal
  Serial.println("READY");
  
  // Wait for setup
  delay(2000);
}

void strikeKey(int duration = 300) {
  mallet.write(SERVO_POS_H);
  delay(duration / 2);  // Split the duration between down and up movement
  mallet.write(SERVO_POS_G);
  delay(duration / 2);
}

void moveToPosition(int targetPos) {
  int steps = targetPos - currentPosition;
  stepper.step(steps);
  currentPosition = targetPos;
  delay(100);
}

void runCycleA() {
  isCycleRunning = true;
  Serial.println("START_A");
  
  // Point O
  strikeKey();
  
  // Point A
  stepper.step(STEPS_30_DEG);
  strikeKey();
  
  // Point B
  stepper.step(STEPS_25_DEG);
  strikeKey();
  
  // Point C
  stepper.step(STEPS_30_DEG);
  strikeKey();
  
  // Point D
  stepper.step(STEPS_25_DEG);
  strikeKey();
  
  // Point E
  stepper.step(STEPS_25_DEG);
  strikeKey();
  
  // Point F
  stepper.step(STEPS_25_DEG);
  strikeKey();
  
  // Return to origin
  int totalSteps = (2 * STEPS_30_DEG) + (4 * STEPS_25_DEG);
  stepper.step(-totalSteps);
  
  isCycleRunning = false;
  currentPosition = 0;
  Serial.println("DONE_A");  // Signal completion
}

void runCycleB() {
  isCycleRunning = true;
  Serial.println("START_B");
  
  // First sequence
  moveToPosition(POS_O);
  strikeKey(STANDARD_HIT);
  delay(SEQUENCE_PAUSE);
  
  moveToPosition(POS_A);
  strikeKey(STANDARD_HIT);
  delay(SEQUENCE_PAUSE);
  
  moveToPosition(POS_C);
  for(int i = 0; i <3; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  delay(SEQUENCE_PAUS);
  for(int i = 0; i <2; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  delay(SEQUENCE_PAUS);
  for(int i = 0; i <2; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  
  
  // Second sequence
  moveToPosition(POS_O);
  strikeKey(STANDARD_HIT);
  delay(SEQUENCE_PAUSE);
  
  moveToPosition(POS_A);
  strikeKey(STANDARD_HIT);
  delay(SEQUENCE_PAUSE);
  
  moveToPosition(POS_C);
   for(int i = 0; i <3; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  delay(SEQUENCE_PAUS);
  for(int i = 0; i <2; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  delay(SEQUENCE_PAUS);
  for(int i = 0; i <2; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  
  // Third sequence
  moveToPosition(POS_O);
  strikeKey(STANDARD_HIT);
  delay(SEQUENCE_PAUSE);
  
  moveToPosition(POS_A);
  strikeKey(STANDARD_HIT);
  delay(SEQUENCE_PAUSE);
  
  moveToPosition(POS_C);
  for(int i = 0; i <3; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  delay(SEQUENCE_PAUS);
  for(int i = 0; i <2; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  delay(SEQUENCE_PAUS);
  for(int i = 0; i <2; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
   delay(SEQUENCE_PAUS);
  for(int i = 0; i <2; i++) {
    strikeKey(STANDARD_HIT);
    delay(SEQUENCE_PAUSE);
  }
  
  // Final B position sequence
  moveToPosition(POS_B);
  strikeKey(B_POSITION_HOLD);
  delay(B_POSITION_HOLD);  // Hold at position B
  
  // Return to origin
  moveToPosition(POS_O);
  
  isCycleRunning = false;
  currentPosition = 0;
  Serial.println("DONE_B");  // Signal completion
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    // Only accept new commands if not currently running a cycle
    if (!isCycleRunning) {
      currentCycle = command;  // Update current cycle
      
      switch(command) {
        case 'A':
          runCycleA();
          break;
        case 'B':
          runCycleB();
          break;
        case 'S':
          Serial.println("STOPPED");
          break;
        default:
          // Invalid command
          Serial.println("INVALID");
          break;
      }
    } else {
      // Busy signal
      Serial.println("BUSY");
    }
  }
} 