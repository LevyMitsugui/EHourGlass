#include <Arduino.h>
#include <NeoPixelConnect.h>

#define SGO_PIN 2
#define SUP_PIN 3
#define SDOWN_PIN 4
#define MAXIMUM_NUM_NEOPIXELS 5
#define LED_PIN 6

typedef struct{
	int state, new_state;

	// tes - time entering state
	// tis - time in state
	unsigned long tes, tis;
} fsm_t;

// input
uint8_t SGO, prevSGO;
uint8_t SUP, prevSUP, longSUP;
uint8_t SDOWN, prevSDOWN;

// Output variables
uint8_t LED[5][3] = {0};

fsm_t operation, supDecipher, ledControl; // State Machines

unsigned long interval, last_cycle;	//	Period execution
uint16_t blink_period;

void set_state(fsm_t &fsm, int new_state){
	if (fsm.state != new_state)
	{ // if the state chnanged tis is reset
		fsm.state = new_state;
		fsm.tes = millis();
		fsm.tis = 0;
	}
}

// Create an instance of NeoPixelConnect and initialize it
// to use GPIO pin 22 as the control pin, for a string
// of 8 neopixels. Name the instance p
NeoPixelConnect strip(LED_PIN, MAXIMUM_NUM_NEOPIXELS, pio0, 0);

void setup(){
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(115200);

	interval = 40;
	set_state(operation, 0);
	set_state(supDecipher, 0);
	set_state(ledControl, 0);

	pinMode(SGO_PIN, INPUT_PULLUP);
	pinMode(SUP_PIN, INPUT_PULLUP);
	pinMode(SDOWN_PIN, INPUT_PULLUP);


	for (uint8_t i = 0; i < MAXIMUM_NUM_NEOPIXELS; i++)
	{
		strip.neoPixelSetValue(i, 255, i * 16, 0);
	}
	digitalWrite(LED_BUILTIN, 1);
	// strip.neoPixelFill(255, 0, 0);
	strip.neoPixelShow();
	Serial.print("On");
	delay(500);

	digitalWrite(LED_BUILTIN, 0);
	strip.neoPixelClear();
	delay(500);
	Serial.println("Off");
}

void loop(){
	uint8_t cmd;
	if(Serial.available()){
		cmd = Serial.read();
		Serial.println(cmd);
		switch (cmd)
		{
		case 'q':
			SGO = 2;
			break;
		case 'w':
			SUP = 2;
			break;
		case 's':
			longSUP = 2;
			break;
		case 'e':
			SDOWN = 2;
			break;
		default:
			cmd = 0;
			break;
		}
	}

	unsigned long now = millis();
	if (now - last_cycle > interval)
	{
		last_cycle = now;

		//	Read Inputs
		prevSGO = (SGO = 2) ? 0 : SGO;
		prevSUP = (SUP = 2) ? 0 : SUP;
		prevSDOWN = (SDOWN = 2) ? 0 : SDOWN;
		SGO = (SGO = 2) ? 1 : !digitalRead(SGO_PIN); //TODO Com o circuito montado, negar o digitalRead
		SUP = (SUP = 2) ? 1 : !digitalRead(SUP_PIN); //TODO Com o circuito montado, negar o digitalRead
		SDOWN = (SDOWN = 2) ? 1 : !digitalRead(SDOWN_PIN); //TODO Com o circuito montado, negar o digitalRead


		//	Update State Machine Timers
		unsigned long cur_time = millis();
		operation.tis 	 = cur_time - operation.tes;
		supDecipher.tis  = cur_time - supDecipher.tes;
		ledControl.tis	 = cur_time - ledControl.tes;

		//	Calculate next states
		//	Update States
		set_state(operation, operation.new_state);
		set_state(supDecipher, supDecipher.new_state);
		set_state(ledControl, ledControl.new_state);

		//	Set Actions

		//	Set Outputs
		strip.neoPixelShow();
		
		//	Serial Log
		Serial.print("SGO: ");
		Serial.print(SGO);
		Serial.print(" | SUP: ");
		Serial.print(SUP);
		Serial.print(" | SDOWN: ");
		Serial.print(SDOWN);

		Serial.print("  _ _  LED1: ");
		Serial.print(LED[0][0] + LED[0][1] + LED[0][2]);
		Serial.print(" | LED2: ");
		Serial.print(LED[1][0] + LED[1][1] + LED[1][2]);
		Serial.print(" | LED3: ");
		Serial.print(LED[2][0] + LED[2][1] + LED[2][2]);
		Serial.print(" | LED4: ");
		Serial.print(LED[3][0] + LED[3][1] + LED[3][2]);
		Serial.print(" | LED5: ");
		Serial.print(LED[4][0] + LED[4][1] + LED[4][2]);

		Serial.print("  _ _  operation: ");
		Serial.print(operation.state);
		Serial.print(" | supDecipher: ");
		Serial.print(supDecipher.state);
		Serial.print(" | ledcControl: ");
		Serial.print(ledControl.state);

		Serial.println();
		
	}
}
