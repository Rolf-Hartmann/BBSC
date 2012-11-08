/*
 * BBC_CNT4.c
 *
 * Created: 16.10.2012 12:21:27
 *  Author: Hartmann
 */ 

#define F_CPU 4194304UL
#include <util/delay.h>
#include <avr/io.h>
//#include <avr/interrupt.h>
#include <stdbool.h>

const uint8_t	 DIG_CR =	0b01000000;	//-- Outputbit Common Digit Rechts
const uint8_t	 DIG_CL	=	0b00100000;	//-- Outputbit Common Digit Links

const uint8_t	Dig_Aus	=	0x00;	//-- Dig-Seg. Aus 
const uint8_t	Dig_0	=	0x3F;	//-- Dig-Seg. 0 
const uint8_t	Dig_1	=	0x06;	//-- Dig-Seg. 1 
const uint8_t	Dig_2	=	0x5B;	//-- Dig-Seg. 2 
const uint8_t	Dig_3	=	0x4F;	//-- Dig-Seg. 3 
const uint8_t	Dig_4	=	0x66;	//-- Dig-Seg. 4 
const uint8_t	Dig_5	=	0x6D;	//-- Dig-Seg. 5 
const uint8_t	Dig_6	=	0x7C;	//-- Dig-Seg. 6 
const uint8_t	Dig_7	=	0x07;	//-- Dig-Seg. 7 
const uint8_t	Dig_8	=	0x7F;	//-- Dig-Seg. 8 
const uint8_t	Dig_9	=	0x67;	//-- Dig-Seg. 9 


unsigned int Wait;		// Display-Wait
unsigned int Wait_Max;	// Display-Wait
unsigned int Loop;	// Sekunden-Wait

volatile int8_t Counter;	// Sekunden-Zähler
volatile uint8_t z;

volatile uint8_t Dig_L;		// Digit-Links
volatile uint8_t Dig_R;		// Digit-Rechts


void WR_Disp_Links(volatile uint8_t Dig_L)
{
		PORTC = Dig_L;
		PORTA = DIG_CL;
		
		for (Wait = 0; Wait < 100; Wait = Wait + 1 ){}


		PORTA = 0b00000000; // alle Digits aus
//		PORTC = 0b00000000; // alle Digits aus
};

void WR_Disp_Rechts(volatile uint8_t Dig_R)
{
		PORTC = Dig_R;
		PORTA = DIG_CR;
		
		for (Wait = 0; Wait < 100; Wait = Wait + 1 ){}


		PORTA = 0b00000000; // alle Digits aus
//		PORTC = 0b00000000; // alle Digits aus
}
		
;

void Wait_Loop()
{
		for (Wait = 0; Wait < 50; Wait = Wait + 1 ){}
};



int main(void)
{

const uint8_t Dez_7Seg[10]	=	{Dig_0, Dig_1, Dig_2, Dig_3, Dig_4, Dig_5, Dig_6, Dig_7, Dig_8, Dig_9};
//--------Index-----------------{--0--, --1--, --2--, --3--, --4--, --5--, --6--, --7--, --8--, --9--}


	DDRA = 0b11100000;	//-- Set Bit[7..5]	= Output
	DDRB = 0b00011110;	//-- Set Bit[4..1]	= Output (Common ext. Digits)
	DDRC = 0b11111111;	//-- Set Bit[7..0]	= Output (D7=Dez.-Punkt, D0..D6 = 7-Seg. A..G)
	DDRD = 0b00111111;	//-- Set Bit[5..0]  = Output 


	Counter = 25;		//-- Startwert für den Zähler
	
    while(1)
    {

		for (Counter = 25; Counter >= 0; Counter = Counter - 1 ){
			
/*
		if ((Counter >= 0  ) || (Counter <=  9))	{WR_Disp_Rechts(Counter);}
		if ((Counter >= 10 ) || (Counter <= 19))	{WR_Disp_Rechts(Counter);}
		if ((Counter >= 20 ) || (Counter <= 24))	{WR_Disp_Rechts(Counter);}
		else										{WR_Disp_Rechts(0x40);}


		if ((Counter >= 0  ) || (Counter <=  9))	{WR_Disp_Links(Dig_Aus);	WR_Disp_Rechts(Dez_7Seg[Counter]);}
		if ((Counter >= 10 ) || (Counter <= 19))	{WR_Disp_Links(Dig_1);		WR_Disp_Rechts(Dez_7Seg[Counter-10]);}
		if ((Counter >= 20 ) || (Counter <= 24))	{WR_Disp_Links(Dig_2);		WR_Disp_Rechts(Dez_7Seg[Counter-20]);}
		else										{WR_Disp_Links(0x7F);		WR_Disp_Rechts(0x7F);}

*/
/*
		if ((Counter >= 0  ) || (Counter <=  9))	{Wait_Loop(); PORTC = Dig_Aus;	Wait_Loop(); PORTC = Dez_7Seg[Counter];}
		if ((Counter >= 10 ) || (Counter <= 19))	{Wait_Loop(); PORTC = Dig_1;	Wait_Loop(); PORTC = Dez_7Seg[Counter-10];}
		if ((Counter >= 20 ) || (Counter <= 24))	{Wait_Loop(); PORTC = Dig_2;	Wait_Loop(); PORTC = Dez_7Seg[Counter-20];}
		else										{Wait_Loop(); PORTC = 0x7F;		Wait_Loop(); PORTC = 0x7F;}


		if ((Counter >= 0  ) || (Counter <=  9))	{PORTC = Dig_Aus;}
		if ((Counter >= 10 ) || (Counter <= 19))	{PORTC = Dig_1;}
		if ((Counter >= 20 ) || (Counter <= 24))	{PORTC = Dig_2;}
		else										{PORTC = 0x7F;		PORTC = 0x7F;}
*/



	switch (Counter) {
			  case	 0: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_Aus;break;
			  case	 1: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_1;	break;
			  case	 2: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_2;	break;
			  case	 3: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_3;	break;
			  case	 4: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_4;	break;
			  case	 5: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_5;	break;
			  case	 6: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_6;	break;
			  case	 7: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_7;	break;
			  case	 8: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_8;	break;
			  case	 9: Wait_Loop(); PORTC = Dig_Aus; Wait_Loop(); PORTC = Dig_9;	break;
			  case	10: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_0;	break;
			  case	11: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_1;	break;
			  case	12: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_2;	break;
			  case	13: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_3;	break;
			  case	14: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_4;	break;
			  case	15: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_5;	break;
			  case	16: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_6;	break;
			  case	17: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_7;	break;
			  case	18: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_8;	break;
			  case	19: Wait_Loop(); PORTC = Dig_1;	  Wait_Loop(); PORTC = Dig_9;	break;
			  case	20: Wait_Loop(); PORTC = Dig_2;	  Wait_Loop(); PORTC = Dig_0;	break;
			  case	21: Wait_Loop(); PORTC = Dig_2;	  Wait_Loop(); PORTC = Dig_1;	break;
			  case	22: Wait_Loop(); PORTC = Dig_2;	  Wait_Loop(); PORTC = Dig_2;	break;
			  case	23: Wait_Loop(); PORTC = Dig_2;	  Wait_Loop(); PORTC = Dig_3;	break;
			  case	24: Wait_Loop(); PORTC = Dig_2;	  Wait_Loop(); PORTC = Dig_4;	break;
			  default : Wait_Loop(); PORTC = 0x7F;	  Wait_Loop(); PORTC = 0x7F;
};


//		PORTC = Counter;



		if (Counter <= 0 )	{Counter = 25;}

		}

}
}