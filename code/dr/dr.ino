// Defence Robot (DR) - Robocon 2021 - Arduino Mega
// turns the Type 2 and Type 3 tables with the claw, blocks the other
// team's arrows with the channel straight up and passes arrows to TR.
// it can also shoot one arrow at a time from the outer area.
//
// drive mode:
//   left stick Y = forward / back     right stick X = turn
//   right stick Y = claw up / down    left stick X = channel angle
// arm mode (left click to switch):
//   left X = base   left Y = shoulder   right Y = wrist   right X = roll
// buttons:
//   B1 grip on/off   B2 arm to floor   B3 arm over channel   B4 arm home
//   B5 cock, press again to shoot
//   B6 B7 B8 aim at Type 1 / 2 / 3 pot
//   right click = block pose on/off
//
// HC-05 on Serial1, TFmini Plus lidar on Serial2

#include <Servo.h>
#include <AccelStepper.h>

// drive motors on MDD10A: front left, rear left, front right, rear right
int pwmPin[4] = {5, 6, 7, 8};
int dirPin[4] = {22, 24, 26, 28};

// arm servos: base, shoulder, wrist, roll
int armPin[4] = {40, 41, 42, 43};
#define VALVE_PIN 44  // gripper valve, HIGH = grip

// channel angle stepper
#define TILT_STEP 30
#define TILT_DIR 31
#define TILT_EN 32
#define HOME_SWITCH 33

// winch stepper that pulls the pusher back
#define WINCH_STEP 34
#define WINCH_DIR 35
#define WINCH_EN 36

#define LATCH_PIN 45
#define LATCH_OPEN 20
#define LATCH_LOCK 100

// claw on the rack, MG996R changed to continuous rotation
#define CLAW_PIN 46
#define CLAW_STOP 1500

// steppers run at 1600 steps per rev (1/8 step)
// tilt has a 10T pinion on the 250T gear so 1600 * 25 / 360 steps per degree
float stepsPerDeg = 111.1;
long cockSteps = 3055;  // 180 mm of string on the 30 mm drum

#define MIN_ANGLE 15
#define MAX_ANGLE 90
#define BLOCK_ANGLE 90  // channel straight up, robot is 515 mm tall

// launch speed and height of the arrow, change speed after test shots
float launchSpeed = 9.0;
float shootHeight = 0.71;

// pot heights from the rulebook in m
#define POT1 0.50
#define POT2 0.60
#define POT3 0.80

// lidar sees the front of the pot table, add this to get the pot centre
#define LIDAR_OFFSET 0.25

// arm poses in degrees
int homePose[4] = {90, 150, 30, 90};
int floorPose[4] = {90, 60, 120, 90};
int loadPose[4] = {20, 130, 70, 90};

Servo arm[4];
Servo latch;
Servo claw;
AccelStepper tilt(AccelStepper::DRIVER, TILT_STEP, TILT_DIR);
AccelStepper winch(AccelStepper::DRIVER, WINCH_STEP, WINCH_DIR);

// remote
int lx, ly, rx, ry;
int buttons = 0;
int oldButtons = 0;
unsigned long lastMsg = 0;

// lidar
int lidarCm = -1;
unsigned long lidarTime = 0;

int armPos[4];
int armTarget[4];
bool grip = false;
bool armMode = false;

int leftSpeed = 0;
int rightSpeed = 0;

float angle = MIN_ANGLE;
float oldAngle = MIN_ANGLE;
bool cocked = false;
bool blocking = false;

unsigned long lastTime = 0;

void readRemote() {
  oldButtons = buttons;

  if (Serial1.available()) {
    String msg = Serial1.readStringUntil('\n');
    int a, b, c, d, e;
    if (sscanf(msg.c_str(), "%d,%d,%d,%d,%d", &a, &b, &c, &d, &e) == 5) {
      lx = a;
      ly = b;
      rx = c;
      ry = d;
      buttons = e;
      lastMsg = millis();
    }
  }

  // nothing from the remote for 300 ms, stop everything
  if (millis() - lastMsg > 300) {
    lx = 0;
    ly = 0;
    rx = 0;
    ry = 0;
    buttons = 0;
  }
}

// true only when the button was just pressed
bool pressed(int b) {
  return bitRead(buttons, b - 1) && !bitRead(oldButtons, b - 1);
}

void readLidar() {
  // frame is 0x59 0x59 distL distH strengthL strengthH tempL tempH checksum
  while (Serial2.available() >= 9) {
    if (Serial2.read() != 0x59) continue;
    if (Serial2.read() != 0x59) continue;

    int buf[7];
    for (int i = 0; i < 7; i++) buf[i] = Serial2.read();

    int sum = 0x59 + 0x59;
    for (int i = 0; i < 6; i++) sum += buf[i];
    if ((sum & 0xFF) != buf[6]) continue;

    // weak or saturated readings are wrong
    unsigned int strength = buf[2] + buf[3] * 256u;
    if (strength < 100 || strength == 65535) continue;

    lidarCm = buf[0] + buf[1] * 256;
    lidarTime = millis();
  }
}

void motor(int i, int speed) {
  if (i >= 2) speed = -speed;  // right side is mirrored
  if (speed < 0) digitalWrite(dirPin[i], HIGH);
  else digitalWrite(dirPin[i], LOW);
  analogWrite(pwmPin[i], abs(speed));
}

void drive(int fwd, int turn) {
  int left = constrain(fwd + turn, -255, 255);
  int right = constrain(fwd - turn, -255, 255);

  // change speed slowly so the wheels don't slip
  leftSpeed += constrain(left - leftSpeed, -30, 30);
  rightSpeed += constrain(right - rightSpeed, -30, 30);

  motor(0, leftSpeed);
  motor(1, leftSpeed);
  motor(2, rightSpeed);
  motor(3, rightSpeed);
}

void stopDrive() {
  leftSpeed = 0;
  rightSpeed = 0;
  for (int i = 0; i < 4; i++) motor(i, 0);
}

void setPose(int pose[]) {
  for (int i = 0; i < 4; i++) armTarget[i] = pose[i];
}

// move each servo 2 degrees at a time so the arm doesn't jerk
void moveArm() {
  for (int i = 0; i < 4; i++) {
    armTarget[i] = constrain(armTarget[i], 0, 180);
    if (armPos[i] < armTarget[i] - 2) armPos[i] += 2;
    else if (armPos[i] > armTarget[i] + 2) armPos[i] -= 2;
    else armPos[i] = armTarget[i];
    arm[i].write(armPos[i]);
  }
}

void setGrip(bool on) {
  grip = on;
  if (grip) digitalWrite(VALVE_PIN, HIGH);
  else digitalWrite(VALVE_PIN, LOW);
}

void setAngle(float a) {
  angle = constrain(a, MIN_ANGLE, MAX_ANGLE);
  tilt.moveTo(angle * stepsPerDeg);
}

// lower the channel until it hits the home switch
void homeChannel() {
  tilt.setSpeed(-400);
  unsigned long start = millis();
  while (digitalRead(HOME_SWITCH) == HIGH) {
    tilt.runSpeed();
    if (millis() - start > 15000) {
      Serial.println("home switch not found");
      return;
    }
  }
  tilt.setCurrentPosition(MIN_ANGLE * stepsPerDeg);
  setAngle(MIN_ANGLE);
}

// pull the pusher back, lock it and let the string out again
void cock() {
  stopDrive();
  claw.writeMicroseconds(CLAW_STOP);
  latch.write(LATCH_OPEN);
  winch.runToNewPosition(cockSteps);
  latch.write(LATCH_LOCK);
  delay(300);
  winch.runToNewPosition(0);
  cocked = true;
}

void shoot() {
  // wait till the channel stops moving
  if (tilt.distanceToGo() != 0) return;
  latch.write(LATCH_OPEN);
  delay(300);
  cocked = false;
}

void toggleBlock() {
  blocking = !blocking;
  if (blocking) {
    oldAngle = angle;
    setAngle(BLOCK_ANGLE);
  } else {
    setAngle(oldAngle);
  }
}

// angle for the high arc that lands in the pot, 0 if we can't reach
float findAngle(float dist, float potHeight) {
  float g = 9.81;
  float v2 = launchSpeed * launchSpeed;
  float h = potHeight - shootHeight;
  float k = v2 * v2 - g * (g * dist * dist + 2 * h * v2);
  if (k < 0) return 0;
  return atan((v2 + sqrt(k)) / (g * dist)) * 180 / PI;
}

void aim(float potHeight) {
  if (lidarCm < 0 || millis() - lidarTime > 200) {
    Serial.println("no lidar reading");
    return;
  }
  float dist = lidarCm / 100.0 + LIDAR_OFFSET;
  float a = findAngle(dist, potHeight);
  if (a == 0 || a > 80) {
    Serial.println("pot out of reach");
    return;
  }
  if (a < 50) a = 50;
  blocking = false;
  setAngle(a);
  Serial.print("aim at ");
  Serial.print(dist);
  Serial.println(" m");
}

void sticks() {
  if (armMode) {
    armTarget[0] += lx / 100;
    armTarget[1] += ly / 100;
    armTarget[2] += ry / 100;
    armTarget[3] += rx / 100;
    claw.writeMicroseconds(CLAW_STOP);
    return;
  }

  drive(ly, rx * 3 / 5);
  claw.writeMicroseconds(CLAW_STOP + ry * 400L / 255);
  setAngle(angle + lx / 512.0);
}

void setupStepper(AccelStepper &s, int en, int maxSpeed) {
  s.setEnablePin(en);
  s.setPinsInverted(false, false, true);  // A4988 enable is active low
  s.setMaxSpeed(maxSpeed);
  s.setAcceleration(maxSpeed * 2);
  s.enableOutputs();
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  Serial1.setTimeout(50);
  Serial2.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(pwmPin[i], OUTPUT);
    pinMode(dirPin[i], OUTPUT);
    motor(i, 0);
  }

  for (int i = 0; i < 4; i++) {
    armPos[i] = homePose[i];
    armTarget[i] = homePose[i];
    arm[i].write(homePose[i]);
    arm[i].attach(armPin[i]);
  }
  pinMode(VALVE_PIN, OUTPUT);
  setGrip(false);

  latch.write(LATCH_OPEN);
  latch.attach(LATCH_PIN);
  claw.writeMicroseconds(CLAW_STOP);
  claw.attach(CLAW_PIN);

  pinMode(HOME_SWITCH, INPUT_PULLUP);
  setupStepper(tilt, TILT_EN, 2000);
  setupStepper(winch, WINCH_EN, 3000);

  homeChannel();
  Serial.println("DR ready");
}

void loop() {
  readRemote();
  readLidar();

  if (pressed(9)) {
    armMode = !armMode;
    stopDrive();
  }
  if (pressed(1)) setGrip(!grip);
  if (pressed(2)) setPose(floorPose);
  if (pressed(3)) setPose(loadPose);
  if (pressed(4)) setPose(homePose);
  if (pressed(6)) aim(POT1);
  if (pressed(7)) aim(POT2);
  if (pressed(8)) aim(POT3);
  if (pressed(10)) toggleBlock();

  if (pressed(5)) {
    if (cocked) shoot();
    else cock();
  }

  // sticks and arm every 20 ms
  if (millis() - lastTime >= 20) {
    lastTime = millis();
    sticks();
    moveArm();
  }

  tilt.run();
}
