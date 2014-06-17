/**
@file test_uart.c
@author Milind Kothekar

@section Description
* This file contains testing of uart.c code
*/

//=======================Includes===========================
#include	"uart.c"
#include	<stdio.h>
#include	"acc_ana_ext.c"

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


//======================MAIN FUNCTION===========================
/**
 Main function. It reads the (X,Y,Z) values and sends them to the buffer to be transmitted
*/

int main(void)
{
	stdout = &mystdout;
	init_uart();
	init_acc_ana();
	//output LED's
	sbi(DDRD,PD2);
	sbi(DDRD,PD3);
	sbi(DDRD,PD4);
	cbi(PORTD,PD2);
	cbi(PORTD,PD3);
	cbi(PORTD,PD4);
	
	char a = 'A';
	char b = 'a';
	while(1)
	{
		//printf("%c\t",a);
		//_delay_ms(200);
//		_delay_ms(100);
		
		//printf("b");
		tbi(PORTD,PD4);
		
//		b = (int)read_x_ana();
		//b = read_x_ana();
//		printf("%dx",b);
		//printf("%d\t",b);
		//_delay_ms(1);
		//b = (int)read_y_ana();
		//printf("%d\t",b);
		//_delay_ms(1);
		//b = (int)read_z_ana();
		//printf("%d\n\r",b);
		//_delay_ms(5);
		//printf("%c",'a');
		b = read_z_ana();
		printf("%c",b);
		_delay_ms(1);
		b = read_y_ana();
		printf("%c",b);
		_delay_ms(1);
		b = read_x_ana();
		printf("%c",b);
		_delay_ms(10);
		
	}
	
	return 0;
}
