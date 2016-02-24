/*
 Name:		bb8_fw.ino
 Created:	1/15/2016 8:01:30 PM
 Author:	Lukasz Bartosz
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

#define PIN_HEAD_SERVO_X 9	// D9 PB1 OC1A
#define PIN_HEAD_SERVO_Y 10	// D10 PB2 OC1B

#define PIN_M1_SPEED 3		// D5 PD5
#define PIN_M1_LEFT 7		// D7 PD7
#define PIN_M1_RIGHT 8		// D8 PB0 

#define PIN_M2_SPEED  12	// D6 PD6
#define PIN_M2_LEFT 14		// PC0 ADC0
#define PIN_M2_RIGHT 15		// PC1 ADC1

#define PIN_PIEZO 11		// D12 PB3 MOSI

#define WAIT 100
#define MIN_MOTOR_SPEED 155
#define MAX_MOTOR_SPEED 255
#define ACCELERATION 10		//this will be added to translation once accelerating
#define MAX_TRANSLATION 100 //this has to be MAX_MOTOR_SPEED minus MIN_MOTOR_SPEED

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

int translation = 0;
int rotation = 0;
int m1_speed = 0;
int m2_speed = 0;

SoftwareSerial bt_serial(0, 1);
char ctrls_received = NOOP;

//==================================
// functions declarations
void test_actuators(void);
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

	Serial.begin(9600);
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
	
	// receive ctrl via bt
	ctrls_received = NOOP;
	if (bt_serial.available())
		ctrls_received = bt_serial.read();
	else
		ctrls_received = NOOP;


	if (ctrls_received != NOOP)
		Serial.write(ctrls_received);

	// some ifs here to read what to do from ctrl commands
	switch (ctrls_received)	{
	case SOUND:
		make_sound();
		break;
	case FORWARD:
		if (translation >= 0 && translation < MAX_TRANSLATION)
			translation += ACCELERATION;
		else if (translation < 0)
			full_stop();
		break;
	case BACKWARD:
		if (translation <= 0 && abs(translation) < MAX_TRANSLATION)
			translation -= ACCELERATION;
		else if (translation > 0)
			full_stop();
		break;
	case LEFT:
		break;
	case RIGHT:
		break;
	case STOP:
		translation = 0;
		full_stop();
		break;
	case NOOP:
		if (translation > 0)
			translation -= ACCELERATION;
		else if (translation < 0)
			translation += ACCELERATION;
		break;
	default:
		break;
	}


	// update actuators
	int target_speed = 0;
	if (translation > 0)
		target_speed = MIN_MOTOR_SPEED + translation;
	else if (translation < 0)
		target_speed = translation - MIN_MOTOR_SPEED;
	

	Serial.print(translation);
	Serial.print("\n");

	delay(WAIT);
}

//==================================
// function definitions

int set_m1_speed(int target_speed) {
	// accelerate and start rolling forward
	// return current engine speed delta

	if (target_speed > 0) {
		if (m1_speed >= 0) {
			digitalWrite(PIN_M1_LEFT, HIGH);
			digitalWrite(PIN_M1_RIGHT, LOW);
		}
		else {
			digitalWrite(PIN_M1_LEFT, LOW);
			digitalWrite(PIN_M1_RIGHT, LOW);
			analogWrite(PIN_M1_SPEED, 0);
			return(0);
		}
	}
	else if (target_speed < 0) {
		if (m1_speed <= 0) {
			digitalWrite(PIN_M1_LEFT, LOW);
			digitalWrite(PIN_M1_RIGHT, HIGH);
		}
		else {
			digitalWrite(PIN_M1_LEFT, LOW);
			digitalWrite(PIN_M1_RIGHT, LOW);
			analogWrite(PIN_M1_SPEED, 0);
			return(0);
		}
	}
	else {
		digitalWrite(PIN_M1_LEFT, LOW);
		digitalWrite(PIN_M1_RIGHT, LOW);
		analogWrite(PIN_M1_SPEED, 255);
		return(0);
	}

	for (int speed = max(m1_speed, MIN_MOTOR_SPEED); speed <= target_speed; speed++) {
		analogWrite(PIN_M1_SPEED, speed);
		delay(5);
	}
	return (target_speed - m2_speed);
}

int set_m2_speed(int target_speed) {
	// accelerate and start rolling forward
	// return current engine speed delta

	if (target_speed > 0) {
		if (m2_speed >= 0) {
			digitalWrite(PIN_M2_LEFT, HIGH);
			digitalWrite(PIN_M2_RIGHT, LOW);
		}
		else {
			digitalWrite(PIN_M2_LEFT, LOW);
			digitalWrite(PIN_M2_RIGHT, LOW);
			analogWrite(PIN_M2_SPEED, 0);
			return(0);
		}
	}
	else if (target_speed < 0) {
		if (m2_speed <= 0) {
			digitalWrite(PIN_M2_LEFT, LOW);
			digitalWrite(PIN_M2_RIGHT, HIGH);
		}
		else {
			digitalWrite(PIN_M2_LEFT, LOW);
			digitalWrite(PIN_M2_RIGHT, LOW);
			analogWrite(PIN_M2_SPEED, 0);
			return(0);
		}
	}
	else {
		digitalWrite(PIN_M2_LEFT, LOW);
		digitalWrite(PIN_M2_RIGHT, LOW);
		analogWrite(PIN_M2_SPEED, 255);
		return(0);
	}

	for (int speed = max(m2_speed, MIN_MOTOR_SPEED); speed <= target_speed; speed++) {
		analogWrite(PIN_M2_SPEED, speed);
		delay(5);
	}
	return (target_speed - m2_speed);
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

void test_actuators(void) {

//piezo test
	for (int i = 500; i >= 0; i--) {
		digitalWrite(PIN_PIEZO, LOW);
		delayMicroseconds(1000);
		digitalWrite(PIN_PIEZO, HIGH);
		delayMicroseconds(1000);
	}
	digitalWrite(PIN_PIEZO, LOW);
	delay(WAIT);
	
	
	//serwo test
	servo_head_x.write(0);
	delay(WAIT);
	servo_head_x.write(180);
	delay(WAIT);

	servo_head_y.write(0);
	delay(WAIT);
	servo_head_y.write(180);
	delay(WAIT);
	

	//motor test
	digitalWrite(PIN_M1_LEFT, HIGH);
	digitalWrite(PIN_M1_RIGHT, LOW);
	for (int speed = 150; speed <= 255; speed++) {
		analogWrite(PIN_M1_SPEED, speed);
		delay(10);
	}
	delay(3000);
	for (int speed = 255; speed <= 150; speed--) {
		analogWrite(PIN_M1_SPEED, speed);
		delay(10);
	}
	analogWrite(PIN_M1_SPEED, 0);
	delay(WAIT);
}