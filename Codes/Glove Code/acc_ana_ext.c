/**
@file acc_ana_ext.c
@author Milind Kothekar

@section Description
* This file contains the hardware handling functions of the accelerometers
* 
FUNCTIONS defined:
void init_acc_ana(void)
char read_x_ana(void)
char read_y_ana(void)
char read_z_ana(void)


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

#define		ACC_PORT	PORTC		//!< \def Accelerometer port
#define		ACC_DDR		DDRC		//!< \def Data Direction from accelerometer
#define 	ACC_CONTROL_PORT1		PORTB	//!< \def Accelerometer control pins  
#define 	ACC_CONTROL_DDR1		DDRB	//!< \def Data direction of Accelerometer control pins
#define 	ACC_CONTROL_PORT2		PORTB	//!< \def Accelerometer control pins
#define 	ACC_CONTROL_DDR2		DDRB	//!< \def Data direction of Accelerometer control pins

#define		PIN_X		PC3			//!< \def X-axis value for gravity
#define		PIN_Y		PC4			//!< \def Y-axis value for gravity
#define		PIN_Z		PC5			//!< \def Z-axis value for gravity

#define		PIN_SLEEP_B	PD5			//!< \def accelerometer sleep mode control pin connections
#define		PIN_ZEROG	PB1			//!< \def accelerometer free-fall detection
#define		PIN_GSEL	PD7			//!< \def accelerometer g select
#define		PIN_SELF	PB2			//!< \def acceleromter self-test pin


#define		MUX_X		0b00000101	//!< \def Multiplexer select for X-axis
#define		MUX_Y		0b00000011	//!< \def Multiplexer select for Y-axis
#define		MUX_Z		0b00000100	//!< \def Multiplexer select for Z-axis

//#define		MUX_X		0b00000000

//---------------config end---------------------------------

//===================FUNCTIONS===========================
void init_adc(void);

/**
	Initializes Micrcontroller ports and pins for interfacing with the accelerometer
*/
void init_acc_ana(void)
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
	
	//Setting the other control signals
	//setting controls as output
//sbi(ACC_CONTROL_DDR1,PIN_SLEEP_B);
//	cbi(ACC_CONTROL_DDR1,PIN_ZEROG);
//sbi(ACC_CONTROL_DDR1,PIN_GSEL);
//	sbi(ACC_CONTROL_DDR2,PIN_SELF);
//mili code============================================================
cbi(ACC_CONTROL_DDR1,PB2);
	
	//sleep~ is 1
//sbi(ACC_CONTROL_PORT1,PIN_SLEEP_B);
	//pulling low (note it is input)
//	cbi(ACC_CONTROL_PORT1,PIN_ZEROG);
	//setting lowerrange 1.5g set to 1 for 6g
//cbi(ACC_CONTROL_PORT1,PIN_GSEL);
	//We do not want to enable self test, so 0
//	cbi(ACC_CONTROL_PORT2,PIN_SELF);
//mili code========================================================
cbi(ACC_CONTROL_PORT1,PB2);
	
	//initializing the adc
	init_adc();

	return;
}

/**
	Initializes ADC
*/
void init_adc(void)
{
	//pls comment on each pin
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR) | (0<<MUX3) | (0<<MUX3) |(0<<MUX1) |(0<<MUX0);
	ADCSRA = _BV(ADEN) | (0<<ADSC) | (0<<ADFR) | _BV(ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);
	return;
}

//This function takes input as which pin to read and returns the corr ADC value
/**
	This function takes input as which pin to read and returns the corresponding ADC value
	@param mux_num Pin to read from
	@return char ADC value
*/
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

/**
	Read and return x-axis value from ADC
	@return x char
*/
char read_x_ana(void)
{
	//cli();
	char x_value;
	x_value = get_adc_data(MUX_X);
	return (x_value);
}

/**
	Read and return Y axis value from ADC
	@return y char
*/
char read_y_ana(void)
{
	//cli();
	char y_value;
	y_value = get_adc_data(MUX_Y);
	return (y_value);
}

/**
	Read and return Z axis value from ADC
	@return z char
*/
char read_z_ana(void)
{
	//cli();
	char z_value;
	z_value = get_adc_data(MUX_Z);
	return (z_value);
}
//-------------------------file ends----------------------------//
