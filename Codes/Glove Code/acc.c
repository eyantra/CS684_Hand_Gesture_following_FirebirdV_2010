/***************************ACC.C*************************
* This file contains the hardware handling functions of the accelerometers
* 
FUNCTIONS defined:
void init_acc(void)
char read_x(void)
char read_y(void)
char read_z(void)


*/


//Include files

//Conditional includes to prevent including twice
#ifdef		BIT_FUNCTIONS_DEFINED
	#define 	dprint(str, ...)	/* empty */
#else
	#include	"bit_functions.h"
#endif

#ifdef		AVR_INCLUDED
	#define 	dprint(str, ...)	/* empty */
#else
	#include	"avr_includes.h"
#endif

//==================Configuration===========================
//------------Mapping of ports and pins--------------------

#define		ACC_PORT	PORTC
#define		ACC_DDR		DDRC

#define		PIN_X		PC0
#define		PIN_Y		PC1
#define		PIN_Z		PC2

#define		MUX_X		0b00000000
#define		MUX_Y		0b00000001
#define		MUX_Z		0b00000010
//---------------config end---------------------------------

//===================FUNCTIONS===========================
void init_adc(void);
void init_acc(void)
{
	//Initializing the various ports
	//setting as input
	cbi(ACC_DDR,PIN_X);
	cbi(ACC_DDR,PIN_Y);
	cbi(ACC_DDR,PIN_Z);
	
	//and pulling them low
	cbi(ACC_PORT,PIN_X);
	cbi(ACC_PORT,PIN_Y);
	cbi(ACC_PORT,PIN_Z);
	
	//initializing the adc
	init_adc();

	return;
}

void init_adc(void)
{
	//pls comment on each pin
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR) | (0<<MUX3) | (0<<MUX3) |(0<<MUX1) |(0<<MUX0);
	ADCSRA = _BV(ADEN) | (0<<ADSC) | (0<<ADFR) | _BV(ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
	return;
}

//This function takes input as which pin to read and returns the corr ADC value
char get_adc_data(char mux_num)
{
	char acc_data;
	//Setting config registers
	ADMUX = (0b01110000 + mux_num);
	//ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR) | (0<<MUX3) | (0<<MUX3) |(0<<MUX1) |(0<<MUX0);
	ADCSRA = _BV(ADEN) | (0<<ADSC) | (0<<ADFR) | _BV(ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
	ADCSRA |=  _BV(ADSC);	
	
	_delay_us(1);
	while(!(isHigh(ADCSRA,ADIF)));	//Wait for conversion to complete
	//now data is available
	acc_data = ADCH;
	ADCSRA |=  _BV(ADIF);

	return (acc_data);
}

char read_x(void)
{
	//cli();
	char x_value;
	x_value = get_adc_data(MUX_X);
	return (x_value);
}

char read_y(void)
{
	//cli();
	char y_value;
	y_value = get_adc_data(MUX_Y);
	return (y_value);
}

char read_z(void)
{
	//cli();
	char z_value;
	z_value = get_adc_data(MUX_Z);
	return (z_value);
}
//-------------------------file ends----------------------------//
