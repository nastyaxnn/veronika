/*
 * 3 rabota.cpp
 *
 * Created: 29.03.2026 19:23:32
 * Author : User
 */ 

#define F_CPU 8000000
#define MAIN_DELAY 100
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int min = 30, max = 90, equal = 30;
int num = max/2 ,n = 0;
static int digits[3] = {10, 0, 0};

//Массив масок для цифр
const uint8_t digit_table[] = {
	0b00111111,  // 0
	0b00000110,  // 1
	0b01011011,  // 2
	0b01001111,  // 3
	0b01100110,  // 4
	0b01101101,  // 5
	0b01111101,  // 6
	0b00000111,  // 7
	0b01111111,  // 8
	0b01101111,  // 9
	0b11110011   // P.
};

void initPort();
void initTimer0();
void initTimer1_PWM();
void setPWM(int duty);
void ledPrint(int num);
void segmentSeven(int i);
void buttonsHandler();

//Прерывание для динамической индикации
ISR (TIMER0_OVF_vect)
{
	if (n == 0)
	{
		PORTB |= (1 << PB0);
		PORTB &=~(1 << PB3);
		PORTB &=~(1 << PB2);
		segmentSeven(digits[2]);
	}
	else
	{
		if(n == 1)
		{
			PORTB |= (1 << PB3);
			PORTB &=~(1 << PB0);
			PORTB &=~(1 << PB2);
			segmentSeven(digits[1]);
		}
		else
		{
			PORTB |= (1 << PB2);
			PORTB &=~(1 << PB0);
			PORTB &=~(1 << PB3);
			segmentSeven(digits[0]);
		}
	}
	n++;
	if (n > 2)
	{
		n = 0;
	}
}

//Основная функция
int main(void)
{
	initPort();
	initTimer0();
	initTimer1_PWM();
	while (1)
	{
		buttonsHandler();
		ledPrint(num);
		setPWM(num);
		_delay_ms(MAIN_DELAY);
		
	}
}

//Инициализация портов
void initPort()
{
	DDRB = 0b00001111;
	PORTB = 0b00000000;
	
	DDRC = 0x00;
	PORTC = 0xFF;
	
	DDRD = 0xFF;
	PORTD = 0x00;
}

//Инициализация таймера для индикации
void initTimer0()
{
	TCCR0 = (1<<CS01)|(1<<CS00);
	TIMSK |= (1 << TOIE0);
	sei();
}

//Инициализация таймера 1 для ШИМ
void initTimer1_PWM()
{
	TCCR1A |= (1 << COM1A1);
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	TCCR1B |= (1 << CS11);

	ICR1 = 255;
	OCR1A = max / 2;
}

//Изменение скважности ШИМ
void setPWM(int duty)
{
	OCR1A = 255 * duty / 100;
}

//Запись значиний для таймера
void ledPrint(int num)
{
	digits[1] = num / 10;
	digits[2] = num % 10;
}

//Передача цифры на индикацию
void segmentSeven(int i)
{
	PORTD = ~digit_table[i];
}


//Обработчик кнопок
void buttonsHandler()
{
	if(!(PINC & 1<<PC0) && !(PINC & 1<<PC1))
	{
		num = equal;
	}
	else
	{
		if (!(PINC & 1<<PC0) && num > min)
		{
			if(num < min)
			{
				num = min;
			}
			else
			{
				num--;
			}
		}
		
		if (!(PINC & (1<<PC1)) && num < max)
		{
			if(num > max)
			{
				num = max;
			}
			else
			{
				num++;
			}
		}
	}
}
