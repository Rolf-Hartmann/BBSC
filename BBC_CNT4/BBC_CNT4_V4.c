/*
 * BBC_CNT4.c
 *
 * Created: 16.10.2012 12:21:27
 *  Author: Hartmann
 */ 

#define F_CPU 4194304UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>



#define BUZZER	0b10000000;	//-- Outputbit Buzzer
#define DIG_CR	0b01000000;	//-- Outputbit Common Digit Rechts
#define DIG_CL	0b00100000;	//-- Outputbit Common Digit Links

#define M_PD2_CntP		0b00000001; //-- Maske: Sign. 0-aktiv, bei PD2=0 => Cnt +1 
#define M_PD2_Start     0b00000010; //-- Maske: Sign. 0-aktiv, bei PD2=0 => Start 
#define M_PD2_30_24		0b00010000; //-- Maske: Sign. 0-aktiv, bei PD2=0 => 30/24 Sec. 

#define M_PD1_Display   0b00001000; //-- Maske: Sign. 0-aktiv, bei PD1=0 => Display Ein/Aus 
#define M_PD1_Buzzer	0b00010000; //-- Maske: Sign. 0-aktiv, bei PD1=0 => Buzzer

#define M_PD0_CntM		0b00000001; //-- Maske: Sign. 0-aktiv, bei PD0=0 => Cnt -1 
#define M_PD0_Reset		0b00000010; //-- Maske: Sign. 0-aktiv, bei PD0=0 => Reset 
#define M_PD0_Clear		0b00001000; //-- Maske: Sign. 0-aktiv, bei PD0=0 => All_Clear
#define M_PD0_C_Buzz	0b00010000; //-- Maske: Sign. 0-aktiv, bei PD0=0 => Call Buzzer



#define Dig_Aus	0x00;	//-- Dig-Seg. Aus 
#define Dig_0	0x3F;	//-- Dig-Seg. 0 
#define Dig_1	0x06;	//-- Dig-Seg. 1 
#define Dig_2	0x5B;	//-- Dig-Seg. 2 
#define Dig_3	0x4F;	//-- Dig-Seg. 3 
#define Dig_4	0x66;	//-- Dig-Seg. 4 
#define Dig_5	0x6D;	//-- Dig-Seg. 5 
#define Dig_6	0x7C;	//-- Dig-Seg. 6 
#define Dig_7	0x07;	//-- Dig-Seg. 7 
#define Dig_8	0x7F;	//-- Dig-Seg. 8 
#define Dig_9	0x67;	//-- Dig-Seg. 9 


#define Seg_A	0b00000001;	//-- Segment A 
#define Seg_B	0b00000010;
#define Seg_C	0b00000100;
#define Seg_D	0b00001000;
#define Seg_E	0b00010000;
#define Seg_F	0b00100000;
#define Seg_G	0b01000000;	//-- Segment G 


unsigned int Wait;	// Display-Wait
unsigned int Loop;	// Sekunden-Wait

volatile int8_t Counter;	// Sekunden-Zähler
volatile uint8_t z;

volatile uint8_t Dig_L;		// Digit-Links
volatile uint8_t Dig_R;		// Digit-Rechts


bool	Reset;
bool	Start;
bool	Cnt_Plus;
bool	Cnt_Minus;
bool	Display;
bool	Clear;
bool	Buzzer;
bool	Call_Buzz;
bool	S30_S24;

/*


//Die ISR wird 64-mal pro Sekunde aufgerufen
ISR(TIMER0_COMP_vect){
	z = z+1;
	if(z==64){
		z=0;
		Counter =  Counter -1;
		if (Counter ==  -1) {Counter = 24;}
	}
}


TIMER0_interrupt_init(void){
z=0; //ISR-Zähler = 0
TCNT0=0; //Anfangszählerstand = 0
//OCR0=249; //Zähler zählt bis 250: 31250/250 = 125
//TCCR0=0x0c; //CTC-Modus: Takt intern von 8 Mhz /256 = 31250Hz
OCR0=255; //Zähler zählt bis 256: 16384/256 = 64
TCCR0=0x0c; //CTC-Modus: Takt intern von 4,194304 Mhz /256 = 16384Hz
//Timer/Counter0 Compare Match Interrupt aktivieren:
TIMSK|=(1<<OCIE0);

}

*/

void WR_Disp_RD_Taster()
//void WR_Disp_RD_Taster(unsigned int Dig_Links, unsigned int Dig_Rechts)
{
	
//	cli();
/*
	          ---------------------- Taster-Matrix -------------------------------
	+========================+===============+===============+===============+===============+
	|  Enable Tasterinput's  |   Input PA4   |   Input PA3   |   Input PA1   |   Input PA0   |
	+========================+===============+===============+===============+===============+
	|   PD2=0, PD1=1, PD0=1  |    0=30/24    |       -       |     0=Start   |  0=Count + 1  |   
	+------------------------+---------------+---------------+---------------+---------------+
	|   PD2=1, PD1=0, PD0=1  |    0=Buzzer   |   0=Display   |        -      |        -      |   
    +------------------------+---------------+---------------+---------------+---------------+
	|   PD2=1, PD1=1, PD0=0  |  0=Call-Buzz  |  0=All-Clear  |    0=Reset    |  0=Count - 1  |   
	+========================+===============+===============+===============+===============+
*/


		PORTC = Dig_L;
		PORTA = DIG_CL;
		


	unsigned int Taster	= 0;


	// --- Start: Read_Taster: Cnt_Plus, Start, S30/24 = aktiv ---        
		PORTD |= (0b00000011);	// Set Bit D0+D1	= 1 => Cathode D3+D2 = 0 (inaktiv)
		PORTD &= (0b11111011);	// Set Bit		D2	= 0 => Cathode D4 = 0 (aktiv)

//		_delay_ms(1.5);
		for (Wait = 0; Wait < 1000; Wait = Wait + 1 ){}

		Taster	= PINA;	// Entprellung fehlt noch

		if ((Taster & 0b00000001 ) == 0 ) {Cnt_Plus = 1;} else {Cnt_Plus	= 0;} 
		if ((Taster & 0b00000010 ) == 0 ) {Start	= 1;} else {Start		= 0;}
		if ((Taster & 0b00010000 ) == 0 ) {S30_S24	= 1;} else {S30_S24		= 0;}

		PORTD |= (0b00000111);	// alles 1 ==> alle Taster inaktiv
	// --- End: Read_Taster: Cnt_Plus, Start, S30/24 = aktiv ---        


		PORTA = 0b00000000; // alle Digits aus






		PORTC = Dig_R;
		PORTA = DIG_CR;

	// --- Start: Read_Taster: Display, Buzzer ---            
		PORTD |= (0b00000101);	// Set Bit D2+D0	= 1 => Cathode D4+D2 = 0 (inaktiv)
		PORTD &= (0b11111101);	// Set Bit	D1		= 0 => Cathode D3 = 0 (aktiv)

//		_delay_ms(1.5);
		for (Wait = 0; Wait < 1000; Wait = Wait + 1 ){}

		Taster	= PINA;	// Entprellung fehlt noch

		if ((Taster & 0b00001000 ) == 0 ) {Display	= 1;} else {Display	= 0;}
		if ((Taster & 0b00010000 ) == 0 ) {Buzzer	= 1;} else {Buzzer	= 0;}

		PORTD |= (0b00000111);	// alles 1 ==> alle Taster inaktiv
	// --- End: Read_Taster: Display, Buzzer ---            


		PORTA = 0b00000000; // alle Digits aus





	// --- Start: Read_Taster: Cnt_Minus, Reset, Clear, Call_Buzz = aktiv ----            
		PORTD |= (0b00000110);	// Set Bit D3+D4	= 1 => Cathode D3+D4 = 0 (inaktiv)
		PORTD &= (0b11111110);	// Set Bit		D2	= 0 => Cathode D2 = 0 (aktiv)

//		_delay_ms(1.5);
		for (Wait = 0; Wait < 1000; Wait = Wait + 1 ){}

		Taster	= PINA;	// Entprellung fehlt noch

		if ((Taster & 0b00000001 ) == 0 ) {Cnt_Minus = 1;} else {Cnt_Minus	= 0;} 
		if ((Taster & 0b00000010 ) == 0 ) {Reset	 = 1;} else {Reset		= 0;}
		if ((Taster & 0b00001000 ) == 0 ) {Clear	 = 1;} else {Clear		= 0;}
		if ((Taster & 0b00010000 ) == 0 ) {Call_Buzz = 1;} else {Call_Buzz	= 0;}

		PORTD |= (0b00000111);	// alles 0 = inaktiv
	// --- End: Read_Taster: Cnt_Minus, Reset, Clear, Call_Buzz = aktiv ----            

//sei();

};





int main(void)

{


//--------Digit-----------------{-0--,-1--,-2--,-3--,-4--,-5--,-6--,-7--,-8--,-9--}
const uint8_t Dez_7Seg[10]	=	{0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x67};
//--------Index-----------------{-1--,-2--,-3--,-4--,-5--,-6--,-7--,-8--,-9--,-10-}


	DDRA = 0b11100000;	//-- Set Bit[7..5]	= Output
	DDRB = 0b00011110;	//-- Set Bit[4..1]	= Output (Common ext. Digits)
	DDRC = 0b11111111;	//-- Set Bit[7..0]	= Output (D7=Dez.-Punkt, D0..D6 = 7-Seg. A..G)
	DDRD = 0b00111111;	//-- Set Bit[5..0]  = Output 


	Counter = 24;		//-- Startwert für den Zähler


// Bitoperatoren: >> = Rechts schieben, << = Links schieben, | = binäres ODER, & = binäres UND

//	TIMER0_interrupt_init();
//	sei();
	
    while(1)
    {



		for (Loop = 0; Loop <= 200; Loop = Loop + 1 ){
			


		if ((Counter >= 0  ) || (Counter <=  9))	{Dig_L = 0x00; Dig_R = Dez_7Seg[Counter];}
		if ((Counter >= 10 ) || (Counter <= 19))	{Dig_L = 0x06; Dig_R = Dez_7Seg[Counter-10];}
		if ((Counter >= 20 ) || (Counter <= 24))	{Dig_L = 0x5B; Dig_R = Dez_7Seg[Counter-20];}
		else										{Dig_L = 0x7F; Dig_R = 0x7F;}



		WR_Disp_RD_Taster();


		if (Loop == 200)	{Counter = Counter-1; Loop = 0;}

		if (Counter == 0 )	{Counter = 24;}

		}


}
}