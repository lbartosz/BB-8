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

#define WAIT 1000
#define MIN_MOTOR_SPEED 150
#define MAX_MOTOR_SPEED 255


// globals
Servo servo_head_x;
Servo servo_head_y;

int translation = 0;
int rotation = 0;
int m1_speed = 0;
int m1_mode = 0;
int m2_speed = 0;
int m2_mode = 0;

SoftwareSerial bt_serial(0, 1);
char c = 'x';


//==================================
// functions declarations
void test_actuators(void);
void move_motor_1(bool, int);
void move_motor_2(bool, int);
void full_stop(bool, bool);

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
		//test_actuators();
	}
	
	// real control code
	if (bt_serial.available()) {
		c = bt_serial.read();
		Serial.write(c);
	}
	//Serial.write('x');
	delay(100);
	//Serial.write(c);

	// get cmd via bt
	//int ctrls_received = receive_control_via_bt();


	// some ifs here to read what to do from ctrl commands

	// call move functions
	
	

}

//==================================
// function definitions

int receive_control_via_bt(void) {
	// get and return bt control transmission here
	return 0;
}

void move_motor_1(bool forward = true, int target_speed = MAX_MOTOR_SPEED) {
	// accelerate and start rolling forward
	// return current engine speed delta

	if (forward) {
		digitalWrite(PIN_M1_LEFT, HIGH);
		digitalWrite(PIN_M1_RIGHT, LOW);
	}
	else {
		digitalWrite(PIN_M1_LEFT, LOW);
		digitalWrite(PIN_M1_RIGHT, HIGH);
	}
	

	for (int speed = max(m1_speed, MIN_MOTOR_SPEED); speed <= target_speed; speed++) {
		analogWrite(PIN_M1_SPEED, speed);
		delay(10);
	}
	m1_speed = target_speed;
}

void move_motor_2(bool forward = true, int target_speed = MAX_MOTOR_SPEED) {
	// accelerate and start rolling forward
	// return current engine speed delta

	if (forward) {
		digitalWrite(PIN_M2_LEFT, HIGH);
		digitalWrite(PIN_M2_RIGHT, LOW);
	}
	else {
		digitalWrite(PIN_M2_LEFT, LOW);
		digitalWrite(PIN_M2_RIGHT, HIGH);
	}


	for (int speed = max(m2_speed, MIN_MOTOR_SPEED); speed <= target_speed; speed++) {
		analogWrite(PIN_M2_SPEED, speed);
		delay(10);
	}
	m2_speed = target_speed;
}


void full_stop(bool stop_m1 = true, bool stop_m2 = true) {
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