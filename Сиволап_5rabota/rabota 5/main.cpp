#define F_CPU 8000000UL
#include <avr/io.h>          
#include <util/delay.h>      
#include <avr/interrupt.h>   

// Биты: dp g f e d c b a
//       7  6 5 4 3 2 1 0

const uint8_t digits_table[] = {
	0b11000000,  // 0
	0b11111001,  // 1
	0b10100100,  // 2
	0b10110000,  // 3
	0b10011001,  // 4
	0b10010010,  // 5
	0b10000010,  // 6
	0b11111000,  // 7
	0b10000000,  // 8
	0b10010000   // 9
};

const uint8_t digits_table_dp[] = {
	0b01000000,  // 0.
	0b01111001,  // 1.
	0b00100100,  // 2.
	0b00110000,  // 3.
	0b00011001,  // 4.
	0b00010010,  // 5.
	0b00000010,  // 6.
	0b01111000,  // 7.
	0b00000000,  // 8.
	0b00010000   // 9.
};

volatile uint8_t n = 0;           
volatile int digits[4] = {0,0,0,0}; 
volatile uint8_t clear_mode = 0;  // 0 = обычный режим, 1 = режим очистки (мигающие нули)
volatile uint8_t blink_state = 1; // 1 = показывать цифры
volatile uint16_t blink_counter = 0; // счётчик для частоты мигания

int read_adc(uint8_t channel)
{
	ADMUX = (1 << REFS0) | (channel & 0x07);
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	return ADC;
}

ISR(TIMER0_OVF_vect)
{
	PORTB &= 0b00001111;  // обнуляем(PB4-PB7)
	
	if(clear_mode && blink_state == 0)
	{
		PORTD = 0xFF;  // Все сегменты выключены
	}
	else
	{
		if(n == 1)
		PORTD = digits_table_dp[digits[n]];
		else
		PORTD = digits_table[digits[n]];
		
		switch(n)
		{
			case 0: PORTB |= (1 << 4); break;  // PB4
			case 1: PORTB |= (1 << 5); break;  // PB5
			case 2: PORTB |= (1 << 6); break;  // PB6
			case 3: PORTB |= (1 << 7); break;  //PB7 
		}
	}
	
	n++;
	if(n > 3) n = 0; 
}

ISR(TIMER2_OVF_vect)
{
	blink_counter++;
	
	if(blink_counter >= 0,5)
	{
		blink_counter = 0;
		
		if(clear_mode)
		{
			// В режиме очистки - переключаем состояние (вкл/выкл)
			blink_state = !blink_state;
		}
		else
		{
			blink_state = 1;
		}
	}
}

int main(void)
{	
	DDRD = 0xFF; 
	DDRB = 0b11110000;  
	DDRC = 0x00;        
	PORTC |= (1 << 2);  
	
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	TCCR0 = (1 << CS01) | (1 << CS00);
	TIMSK |= (1 << TOIE0);
	
	TCCR2 = (1 << CS22) | (1 << CS21) | (1 << CS20);
	TIMSK |= (1 << TOIE2);
	sei();  
	
	while(1)
	{
		int adc_vin = read_adc(0);   // измеряем PC0 (вход RV1)
		int adc_vref = read_adc(1);  // измеряем PC1 (опорное RV2)
		
		long voltage = 0;
		if(adc_vref > 0)
		{
			// Формула: U = (U_вх / U_оп) ? 5В
			// В сотых долях вольта (0 = 0.00, 500 = 5.00)
			voltage = ((long)adc_vin * 50L) / adc_vref;
		}
		
		if(voltage < 0) voltage = 0;
		if(voltage > 999) voltage = 999;
		
		static uint8_t last_button = 1;
		// Читаем текущее состояние (0 = нажата, 1 = не нажата)
		uint8_t current_button = (PINC >> 2) & 1;
		
		// Если кнопку только что нажали (было 1, стало 0)
		if(last_button == 1 && current_button == 0)
		{
			_delay_ms(50);  
			
			if(!(PINC & (1 << 2)))  // проверяем, что кнопка всё ещё нажата
			{
				// Переключаем режим очистки
				clear_mode = !clear_mode;
				blink_state = 1;
				blink_counter = 0;
				
				// Ждём, пока кнопку отпустят
				while(!(PINC & (1 << 2)));
				_delay_ms(50); 
			}
		}
		last_button = current_button;
		
		if(!clear_mode)
		{
			// Разбиваем число на отдельные цифры
			digits[0] = voltage / 100;        
			digits[1] = (voltage / 10) % 10;  
			digits[2] = voltage % 10;         
			digits[3] = 0;                    
		}
		else
		{
			// Режим очистки: показываем нули на всех разрядах
			digits[0] = 0;
			digits[1] = 0;
			digits[2] = 0;
			digits[3] = 0;
		}
		
		_delay_ms(20); 
	}
}