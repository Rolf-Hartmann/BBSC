/*
 * BBC_CNT4.c
 *
 * Created: 06.11.2012
 *  Author: Hartmann
 */ 

#define F_CPU 4194304UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>


const uint8_t  BUZZER =	0b10000000;	//-- Outputbit Buzzer
const uint8_t  DIG_CR =	0b01000000;	//-- Outputbit Common Digit Rechts
const uint8_t  DIG_CL =	0b00100000;	//-- Outputbit Common Digit Links


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


const uint8_t	Read_Loop_Max = 180;// Maimale Anzahl der Abfragen ( = Mux_Time pro Digit, ca. 1,8ms) 
unsigned int	Read_Loop;			// Leseschleife für Taster-Abfrage
volatile int8_t Counter;			// Sekunden-Zähler
volatile uint8_t z;

volatile uint8_t Dig_L;				// Digit-Links
volatile uint8_t Dig_R;				// Digit-Rechts


const	 uint8_t	Deb_ok = 255;		// Minimale Pulsbreite
volatile uint8_t 	Deb_Reset;
volatile uint8_t 	Deb_Start;
volatile uint8_t 	Deb_Cnt_Plus;
volatile uint8_t 	Deb_Cnt_Minus;
volatile uint8_t 	Deb_Display;
volatile uint8_t 	Deb_Clear;
volatile uint8_t 	Deb_Buzzer;
volatile uint8_t 	Deb_Call_Buzz;
volatile uint8_t 	Deb_S30_S24;


bool	Count_Down;
bool	Reset;
bool	Start;
bool	Cnt_Plus;
bool	Cnt_Minus;
bool	Display;
bool	Clear;
bool	Buzzer;
bool	Call_Buzz;
bool	Buzzer_Ein;
bool	S30_S24;



//Die ISR wird 64-mal pro Sekunde aufgerufen
ISR(TIMER0_COMP_vect){
	z = z+1;
	if(z==64){
		z=0;
		if (Count_Down == true) {Counter =  Counter -1;} else {Counter = Counter;}

		if ((Count_Down == false) &  Cnt_Plus)  { if (Counter <= 23) { Counter = Counter+1;}}
		if ((Count_Down == false) &  Cnt_Minus) { if (Counter >= 0)  { Counter = Counter-1;}}

//		if (Counter <=  0)  {Counter = 0;}
		if (Counter <  0)  {Count_Down = false ;Counter = 24;}
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



void WR_Disp_RD_Taster(unsigned int Dig_Links, unsigned int Dig_Rechts)
{
	

/*
	          ---------------------- Taster-Matrix -------------------------------
	+========================+===============+=============+===========+===========+=============+
	|  Enable Tasterinput's  |   Input PA4   |  Input PA3  | Input PA2 | Input PA1 |  Input PA0  |
	+========================+===============+=============+===========+===========+=============+
	|   PD2=0, PD1=1, PD0=1  |    0=30/24    |      -      |     -     |  0=Start  | 0=Count + 1 |   
	+------------------------+---------------+-------------+-----------+-----------+-------------+
	|   PD2=1, PD1=0, PD0=1  |    0=Buzzer   |  0=Display  |     -     |     -     |      -      |   
    +------------------------+---------------+-------------+-----------+-----------+-------------+
	|   PD2=1, PD1=1, PD0=0  |  0=Call-Buzz  | 0=All-Clear |     -     |  0=Reset  | 0=Count - 1 |   
	+========================+===============+=============+===========+===========+=============+
*/


		PORTC  = Dig_Links;
		PORTA |= DIG_CL;
		


	unsigned int Taster	= 0;


	// --- Start: Read Pulsbreite der Taster: Cnt_Plus, Start, S30/24  ---        
		PORTD |= (0b00000011);	// Set Bit D0+D1	= 1 => Cathode D3+D2 = 0 (inaktiv)
		PORTD &= (0b11111011);	// Set Bit		D2	= 0 => Cathode D4 = 0 (aktiv)

		for (Read_Loop = 0; Read_Loop < Read_Loop_Max; Read_Loop = Read_Loop + 1 )
		{
		Taster	= PINA;	// lese Taster
		if (((Taster & 0b00000001) == 0) & (Deb_Cnt_Plus < Deb_ok)) {Deb_Cnt_Plus++;} // Taste = aktiv    ==> Zähler + 1 (bis Deb_ok)
			else { if (Deb_Cnt_Plus > 0) {Deb_Cnt_Plus--;} else {Deb_Cnt_Plus = 0;}}  // Taste = in aktiv ==> Zähler - 1 (bis 0)

		if (((Taster & 0b00000010) == 0) & (Deb_Start < Deb_ok)) {Deb_Start++;}
			else { if (Deb_Start > 0) {Deb_Start--;} else {Deb_Start = 0;}}

		if (((Taster & 0b00010000) == 0) & (Deb_S30_S24 < Deb_ok)) {Deb_S30_S24++;}
			else { if (Deb_S30_S24 > 0) {Deb_S30_S24--;} else {Deb_S30_S24 = 0;}}


		if ((Taster & 0b00000001 ) == 0 ) {Cnt_Plus = 1;} else {Cnt_Plus	= 0;} 
		if ((Taster & 0b00000010 ) == 0 ) {Start	= 1;} else {Start		= 0;}
		if ((Taster & 0b00010000 ) == 0 ) {S30_S24	= 1;} else {S30_S24		= 0;}
		}
		
		PORTD |= (0b00000111);	// alles 1 ==> Taster_Enable = inaktiv
	// --- End: Read Pulsbreite der Taster: Cnt_Plus, Start, S30/24 ---        


		PORTA &= 0b10011111; // alle Digits aus
		PORTC  = 0b00000000; // alle Segmente aus




		PORTC  = Dig_Rechts;
		PORTA |= DIG_CR;

	// --- Start: Read Pulsbreite der Taster: Display, Buzzer ---            
		PORTD |= (0b00000101);	// Set Bit D2+D0	= 1 => Cathode D4+D2 = 0 (inaktiv)
		PORTD &= (0b11111101);	// Set Bit	D1		= 0 => Cathode D3 = 0 (aktiv)

		for (Read_Loop = 0; Read_Loop < Read_Loop_Max; Read_Loop = Read_Loop + 1 )
		{
		Taster	= PINA;	// lese Taster
		if (((Taster & 0b00001000) == 0) & (Deb_Display < Deb_ok)) {Deb_Display++;}
			else { if (Deb_Display > 0) {Deb_Display--;} else {Deb_Display = 0;}}

		if (((Taster & 0b00010000) == 0) & (Deb_Buzzer < Deb_ok)) {Deb_Buzzer++;}
			else { if (Deb_Buzzer > 0) {Deb_Buzzer--;} else {Deb_Buzzer = 0;}}


		if ((Taster & 0b00001000 ) == 0 ) {Display	= 1;} else {Display	= 0;}
		if ((Taster & 0b00010000 ) == 0 ) {Buzzer	= 1;} else {Buzzer	= 0;}
		}		
		PORTD |= (0b00000111);	// alles 1 ==> Taster_Enable = inaktiv
	// --- End: Read Pulsbreite der Taster: Display, Buzzer ---            


		PORTA &= 0b10011111; // alle Digits aus
		PORTC  = 0b00000000; // alle Segmente aus





	// --- Start: Read Pulsbreite der Taster: Cnt_Minus, Reset, Clear, Call_Buzz ----            
		PORTD |= (0b00000110);	// Set Bit D3+D4	= 1 => Cathode D3+D4 = 0 (inaktiv)
		PORTD &= (0b11111110);	// Set Bit		D2	= 0 => Cathode D2 = 0 (aktiv)

		for (Read_Loop = 0; Read_Loop < Read_Loop_Max; Read_Loop = Read_Loop + 1 )
		{
		Taster	= PINA;	// lese Taster
		if (((Taster & 0b00000001) == 0) & (Deb_Cnt_Minus < Deb_ok)) {Deb_Cnt_Minus++;}
			else { if (Deb_Cnt_Minus > 0) {Deb_Cnt_Minus--;} else {Deb_Cnt_Minus = 0;}}

		if (((Taster & 0b00000010) == 0) & (Deb_Reset < Deb_ok)) {Deb_Reset++;}
			else { if (Deb_Reset > 0) {Deb_Reset--;} else {Deb_Reset = 0;}}

		if (((Taster & 0b00001000) == 0) & (Deb_Clear < Deb_ok)) {Deb_Clear++;}
			else { if (Deb_Clear > 0) {Deb_Clear--;} else {Deb_Clear = 0;}}

		if (((Taster & 0b00010000) == 0) & (Deb_Call_Buzz < Deb_ok)) {Deb_Call_Buzz++;}
			else { if (Deb_Call_Buzz > 0) {Deb_Call_Buzz--;} else {Deb_Call_Buzz = 0;}}


		if ((Taster & 0b00000001 ) == 0 ) {Cnt_Minus = 1;} else {Cnt_Minus	= 0;} 
		if ((Taster & 0b00000010 ) == 0 ) {Reset	 = 1;} else {Reset		= 0;}
		if ((Taster & 0b00001000 ) == 0 ) {Clear	 = 1;} else {Clear		= 0;}
		if ((Taster & 0b00010000 ) == 0 ) {Call_Buzz = 1;} else {Call_Buzz	= 0;}
		}		
		PORTD |= (0b00000111);	// alles 1 ==> Taster_Enable = inaktiv
	// --- End: Read Pulsbreite der Taster: Cnt_Minus, Reset, Clear, Call_Buzz ----            


};


void Output_Buzzer_Ein()	{PORTA |= (0b10000000);}
void Output_Buzzer_Aus()	{PORTA &= (0b01111111);}

void Led_Buzzer_Ein()		{PORTD |= (0b00100000);}
void Led_Buzzer_Aus()		{PORTD &= (0b11011111);}
	



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


 	Deb_Reset		= 0;
 	Deb_Start		= 0;
 	Deb_Cnt_Plus	= 0;
 	Deb_Cnt_Minus	= 0;
 	Deb_Display		= 0;
 	Deb_Clear		= 0;
	Deb_Buzzer		= 0;
 	Deb_Call_Buzz	= 0;
 	Deb_S30_S24		= 0;


	Count_Down= false;
	Cnt_Plus  = false; 
	Start	  = false; 
	S30_S24   = false; 
	Display   = false; 
	Buzzer    = true; 
	Cnt_Minus = false;
	Reset     = false; 
	Clear     = false; 
	Call_Buzz = false; 
	Buzzer_Ein= false;


// Bitoperatoren: >> = Rechts schieben, << = Links schieben, | = binäres ODER, & = binäres UND

	TIMER0_interrupt_init();	// Init ISR
	sei();						// Enable INR's
	
    while(1)
    {

//------------------- Anzeige des Zälerstands -------------------------------------

	switch (Counter)
	{
			  case	 0: WR_Disp_RD_Taster(Dig_Aus, Dig_Aus);break;
			  case	 1: WR_Disp_RD_Taster(Dig_Aus, Dig_1);	break;
			  case	 2: WR_Disp_RD_Taster(Dig_Aus, Dig_2);	break;
			  case	 3: WR_Disp_RD_Taster(Dig_Aus, Dig_3);	break;
			  case	 4: WR_Disp_RD_Taster(Dig_Aus, Dig_4);	break;
			  case	 5: WR_Disp_RD_Taster(Dig_Aus, Dig_5);	break;
			  case	 6: WR_Disp_RD_Taster(Dig_Aus, Dig_6);	break;
			  case	 7: WR_Disp_RD_Taster(Dig_Aus, Dig_7);	break;
			  case	 8: WR_Disp_RD_Taster(Dig_Aus, Dig_8);	break;
			  case	 9: WR_Disp_RD_Taster(Dig_Aus, Dig_9);	break;
			  case	10: WR_Disp_RD_Taster(Dig_1,   Dig_0);	break;
			  case	11: WR_Disp_RD_Taster(Dig_1,   Dig_1);	break;
			  case	12: WR_Disp_RD_Taster(Dig_1,   Dig_2);	break;
			  case	13: WR_Disp_RD_Taster(Dig_1,   Dig_3);	break;
			  case	14: WR_Disp_RD_Taster(Dig_1,   Dig_4);	break;
			  case	15: WR_Disp_RD_Taster(Dig_1,   Dig_5);	break;
			  case	16: WR_Disp_RD_Taster(Dig_1,   Dig_6);	break;
			  case	17: WR_Disp_RD_Taster(Dig_1,   Dig_7);	break;
			  case	18: WR_Disp_RD_Taster(Dig_1,   Dig_8);	break;
			  case	19: WR_Disp_RD_Taster(Dig_1,   Dig_9);	break;
			  case	20: WR_Disp_RD_Taster(Dig_2,   Dig_0);	break;
			  case	21: WR_Disp_RD_Taster(Dig_2,   Dig_1);	break;
			  case	22: WR_Disp_RD_Taster(Dig_2,   Dig_2);	break;
			  case	23: WR_Disp_RD_Taster(Dig_2,   Dig_3);	break;
			  case	24: WR_Disp_RD_Taster(Dig_2,   Dig_4);	break;
			  default : WR_Disp_RD_Taster(0x7F,    0x7F);
	}	

/*

		if (Reset)		{WR_Disp_RD_Taster(Dig_Aus, Dig_1);}
		if (Start)		{WR_Disp_RD_Taster(Dig_Aus, Dig_2);}
		if (Cnt_Plus)	{WR_Disp_RD_Taster(Dig_Aus, Dig_3);}
		if (Cnt_Minus)	{WR_Disp_RD_Taster(Dig_Aus, Dig_4);}
		if (Display)	{WR_Disp_RD_Taster(Dig_Aus, Dig_5);}
		if (Clear)		{WR_Disp_RD_Taster(Dig_Aus, Dig_6);}
		if (Buzzer)		{WR_Disp_RD_Taster(Dig_Aus, Dig_7);}
		if (Call_Buzz)	{WR_Disp_RD_Taster(Dig_Aus, Dig_8);}
		if (S30_S24)	{WR_Disp_RD_Taster(Dig_Aus, Dig_9);} else {WR_Disp_RD_Taster(0x00,    0x00);}
*/


		//----------------- Tasterentprellung, setze Flags ------------------------
/*
		if (Deb_Cnt_Plus  >= Deb_ok) {Cnt_Plus	= true;} else {if (Deb_Cnt_Plus  == 0 ) {Cnt_Plus  = false;} } 
		if (Deb_Start	  >= Deb_ok) {Start		= true;} else {if (Deb_Start	  == 0 ) {Start	    = false;} } 
		if (Deb_S30_S24	  >= Deb_ok) {S30_S24	= true;} else {if (Deb_S30_S24	  == 0 ) {S30_S24   = false;} } 

		if (Deb_Display	  >= Deb_ok) {Display	= true;} else {if (Deb_Display	  == 0 ) {Display   = false;} } 
		if (Deb_Buzzer	  >= Deb_ok) {Buzzer	= true;} else {if (Deb_Buzzer	  == 0 ) {Buzzer    = false;} } 

		if (Deb_Cnt_Minus >= Deb_ok) {Cnt_Minus	= true;} else {if (Deb_Cnt_Minus == 0 ) {Cnt_Minus = false;} } 
		if (Deb_Reset	  >= Deb_ok) {Reset		= true;} else {if (Deb_Reset	  == 0 ) {Reset     = false;} } 
		if (Deb_Clear	  >= Deb_ok) {Clear		= true;} else {if (Deb_Clear	  == 0 ) {Clear     = false;} } 
		if (Deb_Call_Buzz >= Deb_ok) {Call_Buzz	= true;} else {if (Deb_Call_Buzz == 0 ) {Call_Buzz = false;} } 

*/

//--------------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------------


		if (Start  ) { Counter	= 24; z	= 0; Count_Down = 1;}
		if (Display) { Counter	= 14; z	= 0; Count_Down = 1;}

		 // Umschalten von Count_Down
		if (Reset) { if (Count_Down ) {Count_Down = false;} else {Count_Down = true;}}


		 // Umschalten von Buzzer Ein/Aus
		if (Buzzer) { if (Buzzer_Ein ) {Buzzer_Ein = false; Led_Buzzer_Aus();}	 // Set Bit D5, LED Buzzer = Aus
								  else {Buzzer_Ein = true;  Led_Buzzer_Ein();}}  // Set Bit D5, LED Buzzer = Ein

		 // Call Buzzer
		if (Call_Buzz )  {Output_Buzzer_Ein();} // Set Bit D7, Buzzer = Ein
					else {Output_Buzzer_Aus();} // Set Bit D7, Buzzer = Aus


		 // Buzzer bei Counter = 0
//		if ((Counter = 0) & (Count_Down))  {Output_Buzzer_Ein();} // Set Bit D7, Buzzer = Ein
//									  else {Output_Buzzer_Aus();} // Set Bit D7, Buzzer = Aus



/*
		if ((Counter >= 0  ) || (Counter <=  9))	{Dig_L = 0x00; Dig_R = Dez_7Seg[Counter];}
		if ((Counter >= 10 ) || (Counter <= 19))	{Dig_L = 0x06; Dig_R = Dez_7Seg[Counter-10];}
		if ((Counter >= 20 ) || (Counter <= 24))	{Dig_L = 0x5B; Dig_R = Dez_7Seg[Counter-20];}
		else										{Dig_L = 0x7F; Dig_R = 0x7F;}

*/


}
}