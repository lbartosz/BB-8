/*
 Name:		bb8_fw.ino
 Created:	1/15/2016 8:01:30 PM
 Author:	Lukasz Bartosz
*/

#include <servo.h>


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
#define PIN_M2_RIGHT 15		// PC1ADC1

#define PIN_PIEZO 11		// D12 PB3 MOSI


// globals
Servo servo_head_x;
Servo servo_head_y;
#define WAIT 1000



// the setup function runs once when you press reset or power the board
void setup() {
	// head control servos
	servo_head_x.attach(PIN_HEAD_SERVO_X, SERVO_MIN, SERVO_MAX);
	servo_head_y.attach(PIN_HEAD_SERVO_Y, SERVO_MIN, SERVO_MAX);

	pinMode(PIN_M1_LEFT, OUTPUT);
	pinMode(PIN_M1_RIGHT, OUTPUT);
	pinMode(PIN_M2_LEFT, OUTPUT);
	pinMode(PIN_M2_RIGHT, OUTPUT);

	pinMode(PIN_PIEZO, OUTPUT);
}

// the loop function runs over and over again until power down or reset
void loop() {

	//piezo test
	if(1) {
		for (int i = 500; i >= 0; i--) {
			digitalWrite(PIN_PIEZO, LOW);
			delayMicroseconds(1000);
			digitalWrite(PIN_PIEZO, HIGH);
			delayMicroseconds(1000);
		}
		digitalWrite(PIN_PIEZO, LOW);
		delay(WAIT);
	}
	
	//serwo test
	if (1) {
		servo_head_x.write(0);
		delay(WAIT);
		servo_head_x.write(180);
		delay(WAIT);

		servo_head_y.write(0);
		delay(WAIT);
		servo_head_y.write(180);
		delay(WAIT);
	}

	//motor test
	if (true) {
		//analogWrite(PIN_M1_SPEED, 0);
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

}