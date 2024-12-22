#include <BluetoothSerial.h>
#include <AccelStepper.h>

// Initialize Bluetooth Serial
BluetoothSerial Bluetooth;

// Define the stepper motors and the pins they will use
AccelStepper LeftBackWheel(1, 42, 43);   // (Type:driver, STEP, DIR) - Stepper1
AccelStepper LeftFrontWheel(1, 40, 41);  // Stepper2
AccelStepper RightBackWheel(1, 44, 45);  // Stepper3
AccelStepper RightFrontWheel(1, 46, 47); // Stepper4

#define led 14

int wheelSpeed = 1500;

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

void setup() {
  // Set initial speed limits for the steppers
  LeftFrontWheel.setMaxSpeed(3000);
  LeftBackWheel.setMaxSpeed(3000);
  RightFrontWheel.setMaxSpeed(3000);
  RightBackWheel.setMaxSpeed(3000);

  Serial.begin(38400);
  Bluetooth.begin("ESP32_Mobile_Robot"); // Set the name for the Bluetooth device
  Serial.println("Bluetooth Initialized. Waiting for connections...");

  pinMode(led, OUTPUT);
}

void loop() {
  // Check for incoming data from Bluetooth
  if (Bluetooth.available()) {
    dataIn = Bluetooth.read();  // Read the data

    // Set m based on received data
    m = dataIn;

    // Set speed
    if (dataIn >= 16) {
      wheelSpeed = dataIn * 10;
      Serial.println(wheelSpeed);
    }
  }

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

// Movement functions implementation (example)

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
  RightFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(wheelSpeed);
}

void moveBackward() {
  LeftFrontWheel.setSpeed(-wheelSpeed);
  LeftBackWheel.setSpeed(-wheelSpeed);
  RightFrontWheel.setSpeed(-wheelSpeed);
  RightBackWheel.setSpeed(-wheelSpeed);
}

void moveRightForward() {
  LeftFrontWheel.setSpeed(wheelSpeed);
  LeftBackWheel.setSpeed(0);
  RightFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(0);
}

void moveLeftForward() {
  LeftFrontWheel.setSpeed(0);
  LeftBackWheel.setSpeed(wheelSpeed);
  RightFrontWheel.setSpeed(0);
  RightBackWheel.setSpeed(wheelSpeed);
}

void moveRightBackward() {
  LeftFrontWheel.setSpeed(-wheelSpeed);
  LeftBackWheel.setSpeed(0);
  RightFrontWheel.setSpeed(-wheelSpeed);
  RightBackWheel.setSpeed(0);
}

void moveLeftBackward() {
  LeftFrontWheel.setSpeed(0);
  LeftBackWheel.setSpeed(-wheelSpeed);
  RightFrontWheel.setSpeed(0);
  RightBackWheel.setSpeed(-wheelSpeed);
}

void rotateLeft() {
  LeftFrontWheel.setSpeed(-wheelSpeed);
  LeftBackWheel.setSpeed(wheelSpeed);
  RightFrontWheel.setSpeed(wheelSpeed);
  RightBackWheel.setSpeed(-wheelSpeed);
}

void rotateRight() {
  LeftFrontWheel.setSpeed(wheelSpeed);
  LeftBackWheel.setSpeed(-wheelSpeed);
  RightFrontWheel.setSpeed(-wheelSpeed);
  RightBackWheel.setSpeed(wheelSpeed);
}

void stopMoving() {
  LeftFrontWheel.setSpeed(0);
  LeftBackWheel.setSpeed(0);
  RightFrontWheel.setSpeed(0);
  RightBackWheel.setSpeed(0);
}

void runSteps() {
  // Example implementation for running stored steps
  if (dataIndex > 0) {
    LeftBackWheel.setCurrentPosition(lbw[0]);
    LeftFrontWheel.setCurrentPosition(lfw[0]);
    RightBackWheel.setCurrentPosition(rbw[0]);
    RightFrontWheel.setCurrentPosition(rfw[0]);
    // Add more logic to run stored positions if needed
  }
}
