/**
 * @file firebird.h
 * @author <b>Group 18:</b>
 <ul>
	<li>Milind Kothekar
	<li>Kumar Lav,  <lav.iit@gmail.com>
	<li>Surinderjeet Singh
	<li>Prithvi
 </ul>
 * @section DESCRIPTION
 This file defines various functions for bot movement and pin configurations
* @section LICENSE

   Copyright (c) 2010, ERTS Lab IIT Bombay erts@cse.iitb.ac.in              
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   * Source code can be used for academic purpose. 
	 For commercial use permission form the author needs to be taken.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. 

  Software released under Creative Commence cc by-nc-sa licence.
  For legal information refer to: 
  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode

********************************************************************************/

/**
	Assign velocity to left and right motors
	@param left_motor velocity of left motor
	@param right_motor velocity of right motor
*/
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

/**
	Move forward
*/
void move_forward()
{
	PORTA=0x06;
}

/**
	Right Turn
*/
void right_turn()
{
	PORTA=0x0A;
}

/**
	Left Turn
*/
void left_turn()
{
	PORTA=0x05;
}

/**
	Move Backward	
*/
void move_backward()
{
	PORTA = 0x09;
}

/**
	Configure pins for buzzer
*/
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
 DDRE = 0xff;
 PORTE = 0xff;
}

/**
	Configure pins for Motors and PWM
*/
void motion_pin_config (void)
{

 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

/**
	Configure pins for timer
*/
void init_timer()
{
	TCCR5B = 0x00;
	TCCR5A = 0xA1;  
	TCCR5B = 0x0B;
}

/**
	Initialize pins
*/
void port_init()
{
	motion_pin_config();
	buzzer_pin_config();
	lcd_port_config();
	init_timer();
	initarr();		// Initialize window
}
/**
	Function To Initialize UART2
 	desired baud rate:9600
 	actual baud rate:9600 (error 0.0%)
 	char size: 8 bit
	parity: Disabled
*/
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x47; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}


/** 
	Function To Initialize all The Devices
*/
void init_devices()
{
	cli(); //Clears the global interrupts
	port_init();  //Initializes all the ports
	uart0_init(); //Initailize UART1 for serial communiaction
	sei();   //Enables the global interrupts
}
