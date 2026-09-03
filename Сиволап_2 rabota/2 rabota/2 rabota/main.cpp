#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

static int num = 0;              // (0-111)
static int startNum = 0;         // (0-12)
static int current21 = 0;        // текущее число для 21 варианта
static int sum21 = 0;            // сумма
static int n = 0;                // для динамической индикации
static int mode = 0;             
static int reverse = 0;          
static int setting21 = 1;        // 1=установка числа, 0=счет для 21 варианта
static int counting21 = 0;       // 0=не считаем, 1=считаем для 21 варианта

// Массив для цифр
static int digits[3] = {0, 0, 0};

const uint8_t digit_table[] = {
	0b11000000,  
	0b11111001,  
	0b10100100,  
	0b10110000,  
	0b10011001,  
	0b10010010,  
	0b10000010,  
	0b11111000,  
	0b10000000,  
	0b10010000,  
	0b10111111,  
};

void initPort();
void initTimer1();
void handleButtons();
void handleMainMode();
void handleVariant21();
void updateDisplay();
void ledPrint(int number);
void segmentSeven(int digit);

// Прерывание Timer1
ISR(TIMER1_OVF_vect)
{
	// выключаем
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
	
	// Включаем нужный индикатор
	if (n == 0) {
		PORTB |= (1 << PB0);
		PORTB &= ~(1 << PB1);
		PORTB &= ~(1 << PB2);
		segmentSeven(digits[0]);  
	}
	else if (n == 1) {
		PORTB |= (1 << PB1);
		PORTB &= ~(1 << PB0);
		PORTB &= ~(1 << PB2);
		segmentSeven(digits[1]);  
	}
	else {
		PORTB |= (1 << PB2);
		PORTB &= ~(1 << PB0);
		PORTB &= ~(1 << PB1);
		segmentSeven(digits[2]);  
	}
	n++;
	if (n > 2) n = 0;
	TCNT1 = 64536;
}

int main(void)
{
	initPort();
	initTimer1();
	
	while (1)
	{
		handleButtons();
		
		if (mode == 0) {
			handleMainMode();    
			} else {
			handleVariant21();   
		}
		
		updateDisplay();
		_delay_ms(50);
	}
}

void initPort()
{
	DDRD = 0xFF;
	PORTD = 0xFF;
	DDRB = 0b00000111;
	PORTB = 0b01110000;
}

// Timer1
void initTimer1()
{
	TCCR1B = (1 << CS11);
	TIMSK |= (1 << TOIE1);
	TCNT1 = 64536;
	sei();
}

// кнопки
void handleButtons()
{
	static int revPrev = 1;
	static int modePrev = 1;
	static int countPrev = 1;
	
	// Reverse
	if (!(PINB & (1 << PB4))) {
		if (revPrev) {
			reverse = !reverse;
			revPrev = 0;
			_delay_ms(50);
		}
		} else {
		revPrev = 1;
	}
	
	// Mode
	if (!(PINB & (1 << PB5))) {
		if (modePrev) {
			mode = !mode;
			
			// смена режима
			if (mode == 1) {
				setting21 = 1;
				counting21 = 0;
				current21 = startNum;
				sum21 = startNum;
			}
			
			modePrev = 0;
			_delay_ms(50);
		}
		} else {
		modePrev = 1;
	}
	
	// Count
	if (!(PINB & (1 << PB6))) {
		if (countPrev) {
			if (mode == 1) {  // Только в режиме 21 варианта
				if (setting21) {
					// Завершаем установку, начинаем счет
					setting21 = 0;
					counting21 = 1;
					current21 = startNum;
					sum21 = startNum;
					} else {
					// Старт/стоп счета
					counting21 = !counting21;
					if (counting21) {
						current21 = startNum;
						sum21 = startNum;
					}
				}
			}
			countPrev = 0;
			_delay_ms(50);
		}
		} else {
		countPrev = 1;
	}
}

// счет 0-111
void handleMainMode()
{
	static int counter = 0;
	counter++;
	
	if (counter >= 10) {  // 0.5 сек
		counter = 0;
		
		if (reverse == 0) {
			num++;
			if (num > 111) num = 0;
			} else {
			num--;
			if (num < 0) num = 111;
		}
	}
}

// Режим 21 варианта
void handleVariant21()
{
	static int changeDelay = 0;
	static int countTimer = 0;
	
	// установка числа
	if (setting21) {
		if (changeDelay == 0) {
			// увеличиваем число кнопкой Reverse
			if (!(PINB & (1 << PB4))) {
				startNum++;
				if (startNum > 12) startNum = 0;
				changeDelay = 10;
			}
			
			if (!(PINB & (1 << PB6))) {
				startNum--;
				if (startNum < 0) startNum = 12;
				changeDelay = 10;
			}
			} else {
			changeDelay--;
		}
		
		current21 = startNum;
		sum21 = startNum;
	}
	// Режим счета
	else if (counting21) {
		countTimer++;
		
		if (countTimer >= 20) {
			countTimer = 0;
			
			if (reverse == 0) {
				if (current21 < 23) {
					current21++;
					sum21 += current21;
					} else {
					counting21 = 0;
				}
				} else {
				// обратный счет: до 0
				if (current21 > 0) {
					current21--;
					sum21 += current21;
					} else {
					counting21 = 0;
				}
			}
		}
	}
}

void updateDisplay()
{
	if (mode == 0) {
		// оснвной режим
		ledPrint(num);
		} else {
		// режим варианта
		if (setting21) {
			ledPrint(startNum);
			} else if (counting21) {
			ledPrint(current21);
			} else {
			ledPrint(sum21);
		}
	}
}

// для отображения
void ledPrint(int number)
{
	if (number < 0) {
		number = 0;  // -0
	}
	
	if (number < 10) {  // от 0 до 9
		digits[0] = number % 10; 
		digits[1] = 0;          
		digits[2] = 0;         
		} else if (number < 100) { // от 10 до 99
		digits[0] = number % 10;     
		digits[1] = number / 10 % 10;
		digits[2] = 0;              
		} else {  // от 100 
		digits[0] = number % 10;        
		digits[1] = number / 10 % 10;   
		digits[2] = number / 100;       
	}
}

// Вывод цифры на индикатор
void segmentSeven(int digit)
{
	if (digit >= 0 && digit <= 10) {
		PORTD = digit_table[digit];
		} else {
		PORTD = 0xFF;
	}
}