#include <Servo.h>
#include <MsTimer2.h>

const int servoPin = 9;
const int minPulseWidth = 880;   // stopped
const int maxPulseWidth = 2080;  // rotating at max speed
const int pulseWidthStep = (maxPulseWidth - minPulseWidth) / 50;
const int startButtonPin = 2;
const int stopButtonPin = 4;
const unsigned long timeToRotate = 60000;  // 60s

enum Status {
  stopped,
  accelerating,
  rotatingMaxSpeed,
  decelerating,
};

Servo servo;
Status status = stopped;
int currPulseWidth = minPulseWidth;
bool isRotationFinished = false;

void setup() {
  servo.attach(servoPin, minPulseWidth, maxPulseWidth);
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(stopButtonPin, INPUT_PULLUP);
  MsTimer2::set(timeToRotate, timerExpired);
}

void loop() {
  switch (status) {
    case stopped:
      doAtStopped();
      break;
    case accelerating:
      doAtAccelerating();
      break;
    case rotatingMaxSpeed:
      doAtRotatingMaxSpeed();
      break;
    case decelerating:
      doAtDecelerating();
      break;
  }
  delay(100);
}

void doAtStopped() {
  servo.writeMicroseconds(minPulseWidth);

  int startButtonState = digitalRead(startButtonPin);
  if (startButtonState == LOW) {
    status = accelerating;
  }
}

void doAtAccelerating() {
  if (currPulseWidth < maxPulseWidth) {
    currPulseWidth += pulseWidthStep;
  }
  servo.writeMicroseconds(currPulseWidth);

  if (currPulseWidth >= maxPulseWidth) {
    status = rotatingMaxSpeed;
  }

  int stopButtonState = digitalRead(stopButtonPin);
  if (stopButtonState == LOW) {
    status = decelerating;
  }
}

void doAtRotatingMaxSpeed() {
  servo.writeMicroseconds(maxPulseWidth);

  static bool isEntry = true;
  if (isEntry) {
    MsTimer2::start();
    isEntry = false;
  }

  int stopButtonState = digitalRead(stopButtonPin);
  if (stopButtonState == LOW || isRotationFinished) {
    status = decelerating;
    isRotationFinished = false;
    isEntry = true;
    MsTimer2::stop();
  }
}

void doAtDecelerating() {
  if (currPulseWidth > minPulseWidth) {
    currPulseWidth -= pulseWidthStep;
  }
  servo.writeMicroseconds(currPulseWidth);

  if (currPulseWidth <= minPulseWidth) {
    status = stopped;
  }
}

void timerExpired() {
  isRotationFinished = true;
}
