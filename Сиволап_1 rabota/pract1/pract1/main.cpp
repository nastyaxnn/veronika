/*
 * pract1.cpp
 *
 * Created: 12.11.2025 20:43:00
 * Author : User
 */ 

#define F_CPU 8000000
#define DELAY 150
#include <avr/io.h>
#include <util/delay.h>

void initPort();
void checkButtons();
void downLight();
void upLight();
void clockwise();
void clockwiseRevers();

volatile bool reverse = false;
volatile bool mode = false;

int main(void)
{
	initPort();
	while (1)
	{
		checkButtons();

		if (mode == false)
		{
			if (reverse == false)
			{
				downLight();
			}
			else
			{
				upLight();
			}
		}
		else
		{
			if (reverse == false)
			{
				clockwise();
			}
			else
			{
				clockwiseRevers();
			}
		}
	}
}

void initPort()
{
	DDRD = 0xFF;
	PORTD = 0x00;

	DDRC = 0xFF;
	PORTC = 0x00;

	DDRB &= ~((1 << PB0) | (1 << PB1));
	PORTB |= (1 << PB0) | (1 << PB1);
}

void checkButtons()
{
	if (!(PINB & (1 << PB0)))
	{
		reverse = !reverse;
		_delay_ms(150);
	}
	if (!(PINB & (1 << PB1)))
	{
		mode = !mode;
		reverse = false;
		_delay_ms(150);
	}
}

void downLight()
{
	for (uint8_t i = 0; i < 7; i++)
	{
		checkButtons();
		PORTC = (1 << i);
		_delay_ms(DELAY);
		PORTC &= ~(1 << i);
	}
	PORTC &= ~(1 << 6);
	
	checkButtons();

	for (uint8_t i = 0; i < 7; i++)
	{
		checkButtons();
		PORTD = (1 << i);
		_delay_ms(DELAY);
		PORTD &= ~(1 << i);
	}
	PORTD &= ~(1 << 6);
}

void upLight()
{
	for (int i = 6; i >= 0; i--)
	{
		checkButtons();
		PORTD = (1 << i);
		_delay_ms(DELAY);
		PORTD &= ~(1 << i);
		_delay_ms(DELAY);
	}
	PORTD &= ~(1 << 0);
	
	
	checkButtons();
	PORTD &= ~(1 << 7);

	for (int i = 6; i >= 0; i--)
	{
		checkButtons();
		PORTC = (1 << i);
		_delay_ms(DELAY);
		PORTC &= ~(1 << i);
		_delay_ms(DELAY);
	}
	PORTC &= ~(1 << 0);
}

void clockwise(void)
{
	static uint8_t position = 0;
	checkButtons();
	PORTC = 0x00;
	PORTD = 0x00;
	
	for (uint8_t i = 0; i < 4; i++) {
		uint8_t led_num = (position + i) % 15;
		
		if (led_num < 8) {
			PORTC |= (1 << led_num);      // PORTC 0-7
			} else {
			PORTD |= (1 << (led_num - 8)); // PORTD 8-13
		}
	}
	
	position = (position + 1) % 15;
	_delay_ms(DELAY);
	checkButtons();
}

void clockwiseRevers()
{
	static uint8_t position = 0;
	checkButtons();
	PORTC = 0x00;
	PORTD = 0x00;
	
	for (uint8_t i = 0; i < 4; i++) {
		uint8_t led_num = (position - i + 14) % 15;  // +14 -
		
		if (led_num < 8) {
			PORTC |= (1 << led_num);      // PORTC 0-7
			} else {
			PORTD |= (1 << (led_num - 8)); // PORTD 8-13
		}
	}
	
	position = (position - 1) % 15;
	_delay_ms(DELAY);
	checkButtons();
}
