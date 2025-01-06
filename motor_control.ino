#include <AccelStepper.h>

// Define the stepper motors and the pins they will use
AccelStepper LeftBackWheel(1, 26, 27);   // STEP: GPIO 26, DIR: GPIO 27
AccelStepper LeftFrontWheel(1, 14, 12);  // STEP: GPIO 14, DIR: GPIO 12
AccelStepper RightBackWheel(1, 25, 33);  // STEP: GPIO 25, DIR: GPIO 33
AccelStepper RightFrontWheel(1, 32, 15); // STEP: GPIO 32, DIR: GPIO 15

#define led 14

int wheelSpeed = 1500;  // Default speed

int dataIn, m;
int lbw[50], lfw[50], rbw[50], rfw[50]; // for storing positions/steps
int dataIndex = 0; // Renamed from 'index' to avoid conflicts

// Function prototypes for movement functions
void moveSidewaysLeft();
void moveSidewaysRight();
void moveForward();
void moveBackward();
void moveRightForward();
void moveLeftForward();
void moveRightBackward();
void moveLeftBackward();
void rotateLeft();
void rotateRight();
void stopMoving();
void runSteps();
void parseSerialData(String data);

void setup() {
  // Set initial speed limits for the steppers
  LeftFrontWheel.setMaxSpeed(3000);
  LeftBackWheel.setMaxSpeed(3000);
  RightFrontWheel.setMaxSpeed(3000);
  RightBackWheel.setMaxSpeed(3000);

  Serial.begin(38400);
  Serial.println("Serial Communication Initialized. Waiting for input...");

  pinMode(led, OUTPUT);
}

void loop() {
  // Check for incoming data from Serial
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // Read the data until newline
    parseSerialData(data);  // Parse the received data (speed and direction)

    // Perform actions based on the value of m
    switch (m) {
      case 4: moveSidewaysLeft(); break;
      case 5: moveSidewaysRight(); break;
      case 2: moveForward(); break;
      case 7: moveBackward(); break;
      case 3: moveRightForward(); break;
      case 1: moveLeftForward(); break;
      case 8: moveRightBackward(); break;
      case 6: moveLeftBackward(); break;
      case 9: rotateLeft(); break;
      case 10: rotateRight(); break;
      case 0: stopMoving(); break;
      case 12: 
        if (dataIndex == 0) {
          LeftBackWheel.setCurrentPosition(0);
          LeftFrontWheel.setCurrentPosition(0);
          RightBackWheel.setCurrentPosition(0);
          RightFrontWheel.setCurrentPosition(0);
        }
        lbw[dataIndex] = LeftBackWheel.currentPosition();  // save position into the array
        lfw[dataIndex] = LeftFrontWheel.currentPosition();
        rbw[dataIndex] = RightBackWheel.currentPosition();
        rfw[dataIndex] = RightFrontWheel.currentPosition();
        dataIndex++;  // Increase the array index
        m = 0; // Reset m after saving positions
        break;
      case 14:
        runSteps();
        if (dataIn != 14) {
          stopMoving();
          memset(lbw, 0, sizeof(lbw)); // Clear the array data to 0
          memset(lfw, 0, sizeof(lfw));
          memset(rbw, 0, sizeof(rbw));
          memset(rfw, 0, sizeof(rfw));
          dataIndex = 0;  // Reset index
        }
        break;
    }
  }

  LeftFrontWheel.runSpeed();
  LeftBackWheel.runSpeed();
  RightFrontWheel.runSpeed();
  RightBackWheel.runSpeed();

  // Monitor the battery voltage
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (3.3 / 4095.00) * 4; // Adjusted for ESP32 ADC and 12V scaling
  if (voltage < 11) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}

// Movement functions
void moveSidewaysLeft() {
  LeftFrontWheel.setSpeed(wheelSpeed);
  LeftBackWheel.setSpeed(-wheelSpeed);
  RightFrontWheel.setSpeed(-wheelSpeed);
  RightBackWheel.setSpeed(wheelSpeed);
}

void moveSidewaysRight() {
  LeftFrontWheel.setSpeed(-wheelSpeed);
  LeftBackWheel.setSpeed(wheelSpeed);
  RightFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(-wheelSpeed);
}

void moveForward() {
  LeftFrontWheel.setSpeed(wheelSpeed);
  LeftBackWheel.setSpeed(wheelSpeed);
  RightFrontWheel.setSpeed(-wheelSpeed);
  RightBackWheel.setSpeed(-wheelSpeed);
}

void moveBackward() {
  LeftFrontWheel.setSpeed(-wheelSpeed);
  LeftBackWheel.setSpeed(-wheelSpeed);
  RightFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(wheelSpeed);
}

void moveRightForward() {
  LeftFrontWheel.setSpeed(0);
  LeftBackWheel.setSpeed(wheelSpeed);
  RightFrontWheel.setSpeed(-wheelSpeed);
  RightBackWheel.setSpeed(0);
}

void moveLeftForward() {
  LeftFrontWheel.setSpeed(wheelSpeed);
  LeftBackWheel.setSpeed(0);
  RightFrontWheel.setSpeed(0);
  RightBackWheel.setSpeed(-wheelSpeed);
}

void moveRightBackward() {
  LeftFrontWheel.setSpeed(0);
  LeftBackWheel.setSpeed(-wheelSpeed);
  RightFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(0);
}

void moveLeftBackward() {
  LeftFrontWheel.setSpeed(-wheelSpeed);
  LeftBackWheel.setSpeed(0);
  RightFrontWheel.setSpeed(0);
  RightBackWheel.setSpeed(wheelSpeed);
}

void rotateLeft() {
  LeftFrontWheel.setSpeed(-wheelSpeed);
  LeftBackWheel.setSpeed(-wheelSpeed);
  RightFrontWheel.setSpeed(-wheelSpeed);
  RightBackWheel.setSpeed(-wheelSpeed);
}

void rotateRight() {
  LeftFrontWheel.setSpeed(wheelSpeed);
  LeftBackWheel.setSpeed(wheelSpeed);
  RightFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(wheelSpeed);
}

void stopMoving() {
  LeftFrontWheel.setSpeed(0);
  LeftBackWheel.setSpeed(0);
  RightFrontWheel.setSpeed(0);
  RightBackWheel.setSpeed(0);
}

void runSteps() {
  for (int i = dataIndex - 1; i >= 0; i--) {
    LeftBackWheel.moveTo(lbw[i]);
    LeftFrontWheel.moveTo(lfw[i]);
    RightBackWheel.moveTo(rbw[i]);
    RightFrontWheel.moveTo(rfw[i]);

    while (LeftBackWheel.distanceToGo() != 0 || LeftFrontWheel.distanceToGo() != 0 ||
           RightBackWheel.distanceToGo() != 0 || RightFrontWheel.distanceToGo() != 0) {
      LeftBackWheel.run();
      LeftFrontWheel.run();
      RightBackWheel.run();
      RightFrontWheel.run();
    }
  }
}

// Serial Data Parsing
void parseSerialData(String data) {
  // Parse the input for direction and speed
  int dIndex = data.indexOf('d'); // Find 'd' for direction
  int sIndex = data.indexOf('s'); // Find 's' for speed

  if (dIndex != -1 && sIndex != -1) {
    // Extract direction and speed substrings
    String directionString = data.substring(dIndex + 1, sIndex);
    String speedString = data.substring(sIndex + 1);

    // Trim the extracted strings to remove any whitespace
    directionString.trim();
    speedString.trim();

    // Convert the extracted values to integers
    m = directionString.toInt(); // Direction (d100 -> 100)
    wheelSpeed = speedString.toInt(); // Speed (s200 -> 200)
  } else {
    Serial.println("Invalid input format. Use format: d<direction> s<speed>");
  }
}

