/*
 Name:    bb8_fw.ino
 Created:  1/15/2016 8:01:30 PM
 Author:  Lukasz Bartosz
*/

#include <servo.h>
#include <SoftwareSerial.h>

//==================================
// enable/disable test mode here
#define TEST_ENABLED 0

// =================================
// pin assignments here and other constants
#define SERVO_MIN 1000
#define SERVO_MAX 2400

#define PIN_HEAD_SERVO_X 9  // D9 PB1 OC1A
#define PIN_HEAD_SERVO_Y 10  // D10 PB2 OC1B

#define PIN_M1_SPEED 3    // D3 PD3
#define PIN_M1_LEFT 7    // D7 PD7
#define PIN_M1_RIGHT 8    // D8 PB0 

#define PIN_M2_SPEED 6  // D6 PD6
#define PIN_M2_LEFT 14    // PC0 ADC0
#define PIN_M2_RIGHT 15    // PC1 ADC1

#define PIN_PIEZO 11    // D12 PB3 MOSI

#define WAIT 10
#define MIN_MOTOR_SPEED 80
#define MAX_MOTOR_SPEED 81
#define ACC_STEP 1    //this will be added to translation once accelerating
#define SLOW_STEP 1  //this will be added to translation once slowing down
#define MAX_TRANSLATION 10 //this has to be MAX_MOTOR_SPEED minus MIN_MOTOR_SPEED
#define MAX_ROTATION 5    //this is max value that can be added/substracted from translation when turning
#define ROT_STEP 1    // this will be added to rotation once turn control is pressed
#define MAX_SPEEDUP_TICKS 3  // this is reset value to ticks
#define MAX_SLOWDOWN_TICKS 10 // this is reset value to ticks
 
#define LEFT 'L'
#define RIGHT 'R'
#define FORWARD 'F'
#define BACKWARD 'B'
#define STOP 'S'
#define SOUND 'P'
#define NOOP 'N'


// globals
Servo servo_head_x;
Servo servo_head_y;

SoftwareSerial bt_serial(0, 1);

int translation = 0;
int rotation = 0;
int m1_speed = 0;
int m2_speed = 0;
unsigned int speedup_ticks = MAX_SPEEDUP_TICKS;
unsigned int slowdown_ticks = MAX_SLOWDOWN_TICKS;

char last_ctrls_received = NOOP;

//==================================
// functions declarations
void test_actuators(void);
void update_actuators(int, int);
int set_m1_speed(int = MAX_MOTOR_SPEED);
int set_m2_speed(int = MAX_MOTOR_SPEED);
void full_stop(bool = true, bool = true);
void make_sound(void);

//==================================
// SYSTEM SETUP
void setup() {
  // head control servos
  servo_head_x.attach(PIN_HEAD_SERVO_X, SERVO_MIN, SERVO_MAX);
  servo_head_y.attach(PIN_HEAD_SERVO_Y, SERVO_MIN, SERVO_MAX);
  
  pinMode(PIN_M1_LEFT, OUTPUT);
  pinMode(PIN_M1_RIGHT, OUTPUT);
  pinMode(PIN_M2_LEFT, OUTPUT);
  pinMode(PIN_M2_RIGHT, OUTPUT);
  
  pinMode(PIN_PIEZO, OUTPUT);
  
  //Serial.begin(9600);
  bt_serial.begin(9600);

}

//==================================
// MAIN LOOP
void loop() {
  // --------------------------------------
  // test code to check actuators
  while(TEST_ENABLED) {
    test_actuators();
  }

  // this sounds like a good idea
  // ALTERNATIVE, DO NOT LOOP THROUGH COMMAND PARSING IF CTRLS ARE NOT RECEIVED. JUST RESET last_ctrls_received value to NONO after some timeout

  // receive ctrl via bt
  if (bt_serial.available())
    last_ctrls_received = bt_serial.read();
  //else
  //  last_ctrls_received = NOOP;
  
  


  // some ifs here to read what to do from ctrl commands
  switch (last_ctrls_received)  {
  case SOUND:
    make_sound();
    last_ctrls_received = NOOP;
    break;
  case FORWARD:
    if (translation < 0) {
      full_stop();
      translation = 0;
    } else {
      translation = MAX_TRANSLATION;
    }
    break;
  case BACKWARD:
    if (translation > 0) {
      full_stop();
      translation = 0;
    } else {
      translation = -MAX_TRANSLATION;
    }
    break;
  case LEFT:
    if (rotation <= 0) {
      rotation = -MAX_ROTATION;
    } else {
      rotation = 0;
    }
    break;
  case RIGHT:
    if (rotation >= 0) {
      rotation = MAX_ROTATION;
    } else {
      rotation = 0;
    }
    break;
  case STOP:
    last_ctrls_received = NOOP;
    translation = 0;
    rotation = 0;
    full_stop();
    break;
  case NOOP:
    // translation = 0;
    // rotation = 0;
    // full_stop();
    break;
  default:
    break;
  }


  // update actuators
  update_actuators(translation, rotation);
  
  
  //Serial.print(" |Trans: "); Serial.print(translation);
  //Serial.print(" |Rot: "); Serial.print(rotation);
  //Serial.print(" |M1: "); Serial.print(m1_speed);
  //Serial.print(" |M2: "); Serial.print(m2_speed);
  //Serial.print("\n");
}

//==================================
// function definitions

void update_actuators(int translation, int rotation) {
  int m1_target_speed = 0;
  int m2_target_speed = 0;

  if (translation > 0) {
    m1_target_speed = MIN_MOTOR_SPEED + translation;
    m2_target_speed = MIN_MOTOR_SPEED + translation;
  }
  else if (translation < 0) {
    m1_target_speed = translation - MIN_MOTOR_SPEED;
    m2_target_speed = translation - MIN_MOTOR_SPEED;
  }

  m1_target_speed += rotation;
  m2_target_speed -= rotation;
  
  m1_speed = set_m1_speed(m1_target_speed);
  m2_speed = set_m2_speed(m2_target_speed);
}

int set_m1_speed(int target_speed) {
  // set motor speed to given in argument
  // return current engine speed

  if (target_speed > 0) {
    if (m1_speed >= 0) {
      digitalWrite(PIN_M1_LEFT, HIGH);
      digitalWrite(PIN_M1_RIGHT, LOW);
    }
    else {
      full_stop(true, false);
      return(0);
    }
  }
  else if (target_speed < 0) {
    if (m1_speed <= 0) {
      digitalWrite(PIN_M1_LEFT, LOW);
      digitalWrite(PIN_M1_RIGHT, HIGH);
    }
    else {
      full_stop(true, false);
      return(0);
    }
  }
  else {
    full_stop(true, false);
    return(0);
  }

  analogWrite(PIN_M1_SPEED, abs(target_speed));
  return (target_speed);
}

int set_m2_speed(int target_speed) {
  // accelerate and start rolling forward
  // return current engine speed

  if (target_speed > 0) {
    if (m2_speed >= 0) {
      digitalWrite(PIN_M2_LEFT, HIGH);
      digitalWrite(PIN_M2_RIGHT, LOW);
    }
    else {
      full_stop(false, true);
      return(0);
    }
  }
  else if (target_speed < 0) {
    if (m2_speed <= 0) {
      digitalWrite(PIN_M2_LEFT, LOW);
      digitalWrite(PIN_M2_RIGHT, HIGH);
    }
    else {
      full_stop(false, true);
      return(0);
    }
  }
  else {
    digitalWrite(PIN_M2_LEFT, LOW);
    digitalWrite(PIN_M2_RIGHT, LOW);
    analogWrite(PIN_M2_SPEED, 255);
    return(0);
  }

  analogWrite(PIN_M2_SPEED, abs(target_speed));
  return (target_speed);
}


void full_stop(bool stop_m1, bool stop_m2) {
  if (stop_m1) {
    digitalWrite(PIN_M1_LEFT, LOW);
    digitalWrite(PIN_M1_RIGHT, LOW);
    analogWrite(PIN_M1_SPEED, 0);
    m1_speed = 0;
  }
  if (stop_m2) {
    digitalWrite(PIN_M2_LEFT, LOW);
    digitalWrite(PIN_M2_RIGHT, LOW);
    analogWrite(PIN_M2_SPEED, 0);
    m2_speed = 0;
  }
  // wait for bb8 to stop swaying
  delay(200);
}

void make_sound(void) {
  for (int i = 200; i >= 0; i--) {
    digitalWrite(PIN_PIEZO, LOW);
    delayMicroseconds(1000);
    digitalWrite(PIN_PIEZO, HIGH);
    delayMicroseconds(1000);
  }
  digitalWrite(PIN_PIEZO, LOW);
}