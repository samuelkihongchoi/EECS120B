/*	Partner(s) Name & E-mail: Stanley Okwuoma sokwu001
 *	Lab Section: 22
 *	Assignment: Lab 9 Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

//NOTE*** THIS NEW CODE TARGETS PB6 NOT PB3

void set_PWM(double frequency) {
	
	
	// Keeps track of the currently set frequency
	// Will only update the registers when the frequency
	// changes, plays music uninterrupted.
	static double current_frequency;
	if (frequency != current_frequency) {

		if (!frequency) TCCR3B &= 0x08; //stops timer/counter
		else TCCR3B |= 0x03; // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) OCR3A = 0xFFFF;
		
		// prevents OCR3A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) OCR3A = 0x0000;
		
		// set OCR3A based on desired frequency
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;

		TCNT3 = 0; // resets counter
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

#define C4 261.63
#define D4 293.66
#define E4 329.63
#define F4 349.23
#define G4 392.00
#define A4 440.00
#define B4 493.88
#define C5 523.25

double notearray[] = {C4, D4, E4, F4, G4, A4, B4, C5};

unsigned char tempA, noteCount, flag;

enum States {init, notes, on, wait} state;

void noteTick(){
	//transitions
	switch(state){
		case init:
		state = notes;
		break;
		
		case notes:
		if(tempA == 0x04)
		{
			flag = !flag;
			state = on;
		}
		else if(tempA == 0x02)
		{
			state = wait;
			if(noteCount)
			{
				noteCount--;
			}
		}
		else if(tempA == 0x01)
		{
			state = wait;
			if(noteCount < 7)
			{
				noteCount++;
			}
		}
		else
		{
			state = notes;
		}
		break;
		
		case on:
		if(tempA == 0x04)
		{
			state = on;
		}
		else
		{
			state = notes;
		}
		break;
		
		case wait:
		if(tempA)
		{
			state = wait;
		}
		else
		{
			state = notes;
		}
		break;

		default:
		state = init;
		break;
	}
	if(flag)
	{
		set_PWM(notearray[noteCount]);
	}
	else
	{
		set_PWM(0);
	}
	
}

int main(void)
{
	
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	tempA = 0x00;
	noteCount = 0;
	flag = 0;
	PWM_on();
	while(1)
	{
		tempA = ~PINA;
		noteTick();
	}
}
