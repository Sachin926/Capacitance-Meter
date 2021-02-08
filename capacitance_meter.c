#define F_CPU 16000000UL
#define impedence 9420

#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char s[20];
uint16_t val = 0;
int count = 0;
double time_period = 0.0;
float ufarad_cap = 0.0;

void send_string(char *ch)
{
	while (*ch != '\0')
	{
		while (!(UCSRA & (1<<UDRE)));
		UDR = *ch;
		ch++;
	}		
}

void timer_init()
{
	TCCR1B |= (1<<CS10);
	TCNT1 = 0x00;
	TIMSK |= (1<<TOIE1);
}

ISR(TIMER1_OVF_vect)
{
	count++;
}

ISR (ADC_vect)
{
	val = (ADCL>>6) | (ADCH<<2);
	
	if (val > 647)
	{
		
		time_period = count * (4.096 * pow(10, -3)) + TCNT1 * (6.25 * pow(10, -8));
		ufarad_cap = (time_period / impedence) * pow(10, 6);
		
		if (ufarad_cap > 1)
		{
			dtostrf(ufarad_cap, 3, 2, s);
			send_string(s);
			send_string(" Micro_Farads\r\n");
		}
		
		else
		{
			ufarad_cap = ufarad_cap * pow(10, 3);
			
			dtostrf(ufarad_cap, 3, 2, s);
			send_string(s);
			send_string("Nano_Farads\r\n");
			
		}
		
		PORTB &= ~(1<<0);
		DDRB |= (1<<1);
		PORTB &= ~(1<<1);
		_delay_ms(100);
		DDRB &= ~(1<<1);
		DDRB |= (1<<0);
		PORTB &= ~(1<<1);
		PORTB |= (1<<0);
		
		count = 0;
		TCNT1 = 0x00;
	}
	ADCSRA |= (1<<ADSC);
}

int main()
{
	DDRB |= (1<<0);
	
	
	
	UBRRL = 103;
	UBRRH = 103>>8;
	UCSRC |= (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);
	UCSRB |= (1<<TXEN);
	
	ADMUX |= (1<<REFS0) | (1<<ADLAR);
	ADCSRA |= (ADPS0) | (1<<ADPS1) | (1<<ADPS2);
	ADCSRA |=  (1<<ADEN) | (1<<ADIE) | (1<<ADSC);
	sei();
	timer_init();
	
	
	PORTB |= (1<<0);
	DDRC |= (1<<0);
	PORTC = (1<<0);
	
	while(1);
	
	return 0;
}