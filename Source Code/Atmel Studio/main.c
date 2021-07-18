/* ---- Code for Digital Clock with Alarm using AVR Microcontroller ------ */

#define F_CPU 11059200

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7
#include "lcd.h"

#include <stdlib.h>
#include <stdio.h>

ISR(TIMER1_COMPA_vect);

static volatile int SEC =45; //allocating integer memory for storing seconds
static volatile int MIN =0; // allocating integer memory for storing minutes
static volatile int HOU =0; // allocating integer memory for storing hours

void UART_init()
{
	UCSRA = 0b00000010;  // double speed
	UCSRB = 0b00011000;  // Enable Tx and Rx, polling
	UCSRC = 0b10000110;  // Async mode, no parity, 1 stop bit, 8 data bits
						 //in double-speed mode, UBRR = clock/(8xbaud rate) - 1
	int x = 143;
	UBRRL = x;
	UBRRH = (x >> 8);
}

unsigned char UART_RxChar()
{
	while ((UCSRA & (1 << RXC)) == 0);/* Wait till data is received */
	return(UDR);	 /* Return the byte*/
}

void UART_TxChar(unsigned char ch)
{
	while (! (UCSRA & (1<<UDRE)));	/* Wait for empty transmit buffer*/
	UDR = ch ;
}

int main(void)
{
	DDRA = 0b11000000; //only pin7 and pin8 of port a as output
	DDRC = 0xFF; //Taking portC as output.
	DDRD = 0xFE; 
	
	TCCR1B |=(1<<CS12)|(1<<CS10)|(1<<WGM12); // setting prescale and CTC mode
	OCR1A=5000; //setting compare value equal to counter clock frequency to get an interrupt every second
	sei(); // enabling global interrupts
	TIMSK |=(1<<OCIE1A); //compare match interrupt enable
	

	char SHOWSEC [2]; //seconds displaying character on LCD
	char SHOWMIN [2]; //minutes displaying character on LCD
	char SHOWHOU [2]; // hours displaying character on LCD
	
	int ALSEC = 0; //alarm seconds storing memory
	int ALMIN = 1; //alarm minutes storing memory
	int ALHOU = 0; //alarm hours storing memory
	
	char SHOWALSEC [2];//alarm  seconds displaying character on LCD
	char SHOWALMIN [2];// alarm minutes displaying character on LCD
	char SHOWALHOU [2];//alarm hours displaying character on LCD
	
	/*Reading msg from arduino interfaced with SD Card*/
	UART_init();
	stdout = fdevopen(UART_TxChar, NULL);
	char str[10] = {0};
	int idx;
	for(idx = 0; ; idx++){
		char c = UART_RxChar();
		if(c == '#') break;
		str[idx] = c;
	}
	str[idx] = '\0';
	_delay_ms(100);
	
	
	Lcd4_Init();
	while(1)
	{
		Lcd4_Set_Cursor(1,0);
		
		itoa(HOU/10,SHOWHOU,10); //command for putting variable number in LCD(variable number, in which character to replace, which base is variable(ten here as we are counting number in base10))
								// telling the display to show character(replaced by variable number) of first person after positioning the courser on LCD
		Lcd4_Write_String(SHOWHOU);
		// displaying tens place of hours above
		itoa(HOU%10,SHOWHOU,10);
		Lcd4_Write_String(SHOWHOU);
		// displaying ones place of hours above
		Lcd4_Write_String(":");
		Lcd4_Set_Cursor(1,3);

		itoa(MIN/10,SHOWMIN,10);  ///as integer cannot store decimal values, when MIN=9, we have MIN/10 = 0.9(actual), = 0 for CPU(as integer cannot store decimal values)
		Lcd4_Write_String(SHOWMIN);
		// displaying tens place of minutes above
		itoa(MIN%10,SHOWMIN,10);
		Lcd4_Write_String(SHOWMIN);
		// displaying ones place of minutes above
		Lcd4_Write_String(":");
		Lcd4_Set_Cursor(1,6);
		
		itoa(SEC/10,SHOWSEC,10);
		Lcd4_Write_String(SHOWSEC);
		itoa(SEC%10,SHOWSEC,10);
		Lcd4_Write_String(SHOWSEC);
		
		if (bit_is_set(PINA,5))  //if alarm pin is high
		{
			Lcd4_Write_String(" ALM:ON "); 
			if ((ALHOU==HOU)&(ALMIN==MIN)&(ALSEC==SEC)) //alarm minute=min //and alarm hours= time hours and alarm seconds= time seconds
			{
				PORTA|=(1<<PINB7); //buzzer on
				Lcd4_Set_Cursor(3, 0);
				Lcd4_Write_String("    ");
				Lcd4_Write_String(str);
			}
		}
		if (bit_is_clear(PINA,5)) //if alarm pin is low
		{
			Lcd4_Write_String(" ALM:OFF"); //show alarm is off
			PORTA&=~(1<<PINB7); //buzzer off
		
			Lcd4_Set_Cursor(3, 0);
			Lcd4_Write_String("             ");

		}
		Lcd4_Set_Cursor(2, 0);
		Lcd4_Write_String("ALARM:");
		Lcd4_Set_Cursor(2, 7);
		// Showing alarm hours above
		itoa(ALHOU/10,SHOWALHOU,10);
		Lcd4_Write_String(SHOWALHOU);
		itoa(ALHOU%10,SHOWALHOU,10);
		Lcd4_Write_String(SHOWALHOU);
		Lcd4_Set_Cursor(2, 9);
		Lcd4_Write_String (":");
		Lcd4_Set_Cursor(2, 10);
		
		// Showing alarm hours above
		itoa(ALMIN/10,SHOWALMIN,10);
		Lcd4_Write_String(SHOWALMIN);
		itoa(ALMIN%10,SHOWALMIN,10);
		Lcd4_Write_String(SHOWALMIN);
		Lcd4_Set_Cursor(2, 12);
		Lcd4_Write_String (":");
		Lcd4_Set_Cursor(2, 13);
		
		// Showing alarm seconds above
		itoa(ALSEC/10,SHOWALSEC,10);
		Lcd4_Write_String(SHOWALSEC);
		itoa(ALSEC%10,SHOWALSEC,10);
		Lcd4_Write_String(SHOWALSEC);
		
		Lcd4_Set_Cursor(1,0);

		if (bit_is_set(PINA,4)) // if switch is set to adjust TIME
		{
			if (bit_is_clear(PINA,0)) //button 1 is pressed
			{
				if (MIN<60)
				{
					MIN++; //if minutes of TIME are less than 60 increment it by one
					_delay_ms(220);
				}
				if (MIN==60)
				{
					if (HOU<24)
					{
						HOU++; //if minutes of TIME =60 when button is pressed //and hours of TIME are less than 24, increment hour by one.
					}
					MIN=0; //if minute of TIME=60,reset it to zero
					_delay_ms(220);
				}
			}
			if (bit_is_clear(PINA,1))
			{
				if (MIN>0)
				{
					MIN--; //if second button is pressed and minute of TIME are //greater than zero, decrease minutes by one
					_delay_ms(220);
				}
			}
			if (bit_is_clear(PINA,2))
			{
				if (HOU<24)
				{
					HOU++; //if third button is pressed and hours of TIME are less //than 24, increment the hour by one
				}
				_delay_ms(220);
				if (HOU==24)
				{
					HOU=0; //if hour of TIME equal to 24, reset hour of TIME
				}
			}
			if (bit_is_clear(PINA,3))
			{
				if (HOU>0)
				{
					HOU--; //if fourth button is pressed and hours of TIME are //greater than ZERO, decrement the hour by one
					_delay_ms(220);
				}
			}
		}
		
		
		if (bit_is_clear(PINA,4)) //if alarm adjust is set
		{
			if (bit_is_clear(PINA,0))
			{
				if (ALMIN<60)
				{
					ALMIN++;
					_delay_ms(220);
				}
				if (ALMIN==60)
				{
					if (ALHOU<24)
					{
						ALHOU++;
					}
					ALMIN=0;
					_delay_ms(220);
				}
			}
			if (bit_is_clear(PINA,1))
			{
				if (ALMIN>0)
				{
					ALMIN--;
					_delay_ms(220);
				}
			}
			if (bit_is_clear(PINA,2))
			{
				if (ALHOU<24)
				{
					ALHOU++;
				}
				_delay_ms(220);
				if (ALHOU==24)
				{
					ALHOU=0;
				}
			}
			if (bit_is_clear(PINA,3))
			{
				if (ALHOU>0)
				{
					ALHOU--;
					_delay_ms(220);
				}
			}
		}
	}
	
}
// Everything follows the same as described above for TIME
ISR(TIMER1_COMPA_vect) //loop to be executed on counter compare match
{
	if (SEC<60)
	{
		SEC++;
	}
	if (SEC==60)
	{
		if (MIN<60)
		{
			MIN++;
		}
		SEC=0;
	}
	if (MIN==60)
	{
		if (HOU<24)
		{
			HOU++;
		}
		MIN=0;
	}
	if (HOU==24)
	{
		HOU=0;
	}

}