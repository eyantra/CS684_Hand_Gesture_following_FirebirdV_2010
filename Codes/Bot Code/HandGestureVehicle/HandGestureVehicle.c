/**
 * @mainpage A hand gesture following Firebird V Bot
 * @author <b>Group 18:</b>
 <ul>
	<li>Milind Kothekar
	<li>Kumar Lav,  <lav.iit@gmail.com>
	<li>Surinderjeet Singh
	<li>Prithvi
 </ul>
 * @version 1.1
 * @section DESCRIPTION
 * The following is C code for following gestures transmitted from an accelerometer module. The code runs on the Firebird V bot and needs the zigbee module to receives data from the accelerator module. It interprets the received data as various gestures and performs movements accordingly.
 *
 ******************************************************************/
 /**
  * @file gesture.c
  * @author <b>Group 18:</b>
 <ul>
	<li>Milind Kothekar
	<li>Kumar Lav,  <lav.iit@gmail.com>
	<li>Surinderjeet Singh
	<li>Prithvi
 </ul>
 * @version 1.1
 * @section DESCRIPTION
 * The following is C code for following gestures transmitted from an accelerometer module. The code runs on the Firebird V bot and needs the zigbee module to receives data from the accelerator module. It interprets the received data as various gestures and performs movements accordingly.
 *
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


#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "lcd.h"
#include "firebird.h"

#define X_MAX 145	//!< Maximum value for X-axis gravity for base conditions
#define X_MIN 110	//!< Minimum value for X-axis gravity for base conditions
#define Y_MAX 145	//!< Maximum value for Y-axis gravity for base conditions
#define Y_MIN 105	//!< Minimum value for Y-axis gravity for base conditions
#define Z_MAX 180	//!< Maximum value for Z-axis gravity for base conditions
#define Z_MIN 160	//!< Minimum value for Z-axis gravity for base conditions

#define STOP 0		//!< STOP the bot

#define DYNAMIC 1	//!< Bot will respond to dynamic gestures
#define STATIC 2	//!< Bot will respond to static gestures

#define ACC 1		//!< Accelerating mode
#define DEACC 2		//!< Deaccelerating mode

#define RIGHT_TURN 1	//!< Right turn mode
#define LEFT_TURN 2		//!< Left turn mode

#define WINDOW_SIZE 10	//!< Size of the values to be sampled


unsigned char data; 	//!< To store received data from UDR0
unsigned char  X, Y, Z;	//!< Last recieved values

unsigned int aCount = 0;	
unsigned int dCount = 0;

unsigned int accL = 0;	//!< Level of acceleration/deacceleration

unsigned int status = STATIC;	//!< Which gestures will be accepted by Bot

unsigned int flagG=0;

unsigned int turn=0;	//!< Right/Left Turn

unsigned int acc = 0;	//!< Acceleration/ Deacceleration

int left = 0;		//!< Speed of left wheel 
int  right = 0;		//!< Speed of right wheel




int avger = 0;
int xsum = 0;
int xavg = 0;
int ysum = 0;
int zsum = 0;
int yavg = 0;
int x_arr[WINDOW_SIZE]; //!< Last 10 X values
int y_arr[WINDOW_SIZE];	//!< Last 10 Y values
int z_arr[WINDOW_SIZE];	//!< Last 10 Z values
int x_count = 0;
int y_count = 0;
int z_count = 0;

/**
	Initialize the window values for X, Y and Z
*/
void initarr()
{
	int i;
	for (i = 0; i < WINDOW_SIZE; i++)		// Base values are 120
	{
		x_arr[i] = 120;
		y_arr[i] = 120;
		z_arr[i] = 120;
	}
	xsum = 120 * WINDOW_SIZE;				// sum of window values
	ysum = 120 * WINDOW_SIZE;
	zsum = 120 * WINDOW_SIZE;
}

/**
	Add new triplet(X,Y,Z) into window. This is implemented as a queue. Last entered value is removed and new value is added
	@param x X value
	@param y Y value
	@param z Z value
*/
void shiftVal(int x, int y, int z)
{
	xsum = xsum - x_arr[x_count] + x;	// update the sum
	ysum = ysum - y_arr[y_count] + y;
	zsum = zsum - z_arr[z_count] + z;
	x_arr[x_count] = x;					// update window
	y_arr[y_count] = y;
	z_arr[z_count] = z;
	x_count++;							// increment pointer to next value to be removed
	y_count++;
	z_count++;
	y_count = y_count % WINDOW_SIZE;	// pointer % window size
	x_count = x_count % WINDOW_SIZE;
	z_count = z_count % WINDOW_SIZE;
}

/**
	Moves the bot according to Dynamic gestures.

*/

void dynamicMotion()
{
	int i, j;
	int max = 0;
	int dy=0;
	int l1 = 0;
	int l2 = 0;
	int l3 = 0;
	for(i = 6; i < WINDOW_SIZE; i++)
	{
		if(z_arr[i] > 165)		// there is spike in the Z value. It means there was dynamic acceleration
		{
			//lcd_string("DY ");
			l1 = l3 = 0;
			for(j = 6; j < WINDOW_SIZE; j++)
			{
				if(max < y_arr[j])
				{
					max = y_arr[j];		// get the peak acceleration
				}
				if(y_arr[j] > 155)		// count number of values above threshold. Helps in reducing noise values. Change in Y signifies lateral movement. Turn right.
				{
					//lcd_string("XG ");
					l1++;
				}
			}
		//	lcd_print(1,1,l1,2);
			if(l1 >= 3)					// Genuine dynamic acceleration
			{
				lcd_string("GR ");
				int le = 100;
				int ri = 100;
				velocity((unsigned char)200, (unsigned char)200);	// turn right
				right_turn();
				_delay_ms(1000);
				/*for(l2 = 0; l2 < (l1 * 10); l2++)		// move forward with acceleration
				{
				*/
					//lcd_string("he ");
					velocity((unsigned char)200, (unsigned char)200);
					move_forward();
					_delay_ms(l1*500);
				//}
				/*
				for(l2 = 0; l2 < (l1 * 10); l2++)		// deaccelerate to stop
				{
					velocity((unsigned char)200, (unsigned char)200);
					move_forward();
				}
				*/
				stop();
				initarr();	// remove the values to prevent propagation in subsequent frames. Else same motion will be repeated unless all of them ar out of window.
			} // end (l1 >= 3)	
			else
			{
				//lcd_string("DY ");
				for(j = 6; j < WINDOW_SIZE; j++)
				{
					if(max < x_arr[j])
					max = x_arr[j];
					if(x_arr[j] > 155)	// change in X means forward movement
					{
					//	lcd_string("XG ");
						l3++;
					}
				}
				lcd_print(2,1,l3,2);
				if(l3 >= 3)
				{
					lcd_string("GL ");
					int le = 100;
					int ri = 100;
				/*	for(l2 = 0; l2 < (l3 * 10); l2++)
					{
					*/
					//	lcd_string("hi ");
						velocity((unsigned char)200, (unsigned char)200);
						move_forward();
					_delay_ms(l3*500);
					/*}
					for(l2 = 0; l2 < (l3 * 10); l2++)
					{
						velocity((unsigned char)200, (unsigned char)200);
						move_forward();
					}
					*/
					stop();
					l3=0;
					initarr();				
				} // end if(l3 >= 3)
			} // end else
		} // end (z_arr[i] > 165)	
	} // end for loop
} // end function

/**
	For Static gestures. 
	Decides the movement to be followed by the bot based on input values.
	@return STOP or MOVE Wheter to Stop or not
*/

unsigned int getDirection()
{
	xavg = xsum / WINDOW_SIZE;	// get average values
	yavg = ysum / WINDOW_SIZE;
	int zavg = zsum / WINDOW_SIZE;

	if(zavg < 100)		// hand is upside down. Stop
	{
		return STOP;		
	}
	else
	{
		if(xavg > X_MAX)		// Accelerate
		{
	//		lcd_string("AC ");
			acc = ACC;
			if(xavg < (X_MAX + 15))			// decide on the acceleration levels
			{
				accL = 3;
			}
			else if(xavg < (X_MAX + 30))
			{
				accL = 2;
			}
			else if(xavg < (X_MAX + 45))
			{
				accL =1;
			}
			else 
			{
				accL = 1;
			}
		} // end if(xavg > X_MAX )
		else if(xavg < X_MIN)	//	Deaccelerate 
		{
	//		lcd_string("DE ");
			acc = DEACC;
			if(xavg > (X_MIN - 10))			// decide on deacceleration levels
			{
				accL = 3;
			}
			else if(xavg > (X_MIN - 20))
			{
				accL = 2;
			}
			else if(xavg > (X_MIN - 30))
			{
				accL = 1;
			}
			else
			{ 
				accL = 1;
			}
		} // end else if(xavg < X_MIN)
		else 
		{
	//		lcd_string("Z ");			// Same movement as previous instance
			acc = 0;
		}
 		if(yavg < Y_MIN)				// Left Turn
		{
			turn = LEFT_TURN;;
		}
		else if(yavg > Y_MAX)			// Right Turn
		{
			turn = RIGHT_TURN;
		} 
		else
		{
			turn = 0;					// Same movement as previous instance
		}
		return 5;
	}
}

/**
	Accelerate the bot
*/
void accelerate()
{
	aCount++;
	if((aCount % accL) == 0 )		// Acceleration levels
	{
		aCount = 0;
		left = left + 10;			// Acceleration 
		right = right + 10;
	}
	if(left > 254 )					// Upper bound values of velocity
	{
		left = 254;
	}
	if(right > 254 )
	{
		right = 254;
	}
	velocity((unsigned char)left, (unsigned char)right);	// Assign Velocity
	move_forward();					// move forward
}

void deaccelerate()
{
	dCount++;
	if((dCount % accL) == 0)		// Deacceleration Levels
	{
		left = left - 10;			// deacceleration
		right = right - 10;
		dCount = 0;
	}
	if(left <= 0 && right <= 0)		// Deaccelerate to zero speed then move backward
	{
		if(left < -255 )
		{	
			left = -255;
		}
		if(right < -255)
		{
			right = -255;
		}
		move_backward();
		velocity((unsigned char)(left * -1), (unsigned char)(-1 * right));
	}
	else 							// Deaccelerate
	{
		move_forward();
		velocity((unsigned char)left, (unsigned char)right);
	}
} // end function 

/**
	Stop the Bot
*/
void stop()
{
	PORTA = 0x00;
	left = 0;
	right = 0;
}

/**
	Make right turn while accelerating
*/
void accelerateAndRight()
{
	acc = ACC;				// acclerating mode
	left = left - 30;		// lower left wheel speed
	if(left < 0)			// lower bound speed
	{
		left = 0;
	}
	accelerate();			// accelerate
}

/**
	Make left turn while accelerating
*/
void accelerateAndLeft()
{
	acc = ACC;				// accelerating mode
	right = right - 30;		// lower right wheel speed
	if(right < 0)			// lower bound speed
	{
		right = 0;
	}
	accelerate();			// accelerate
}

/**
	Make right turn while deaccelerating
*/
void deaccelerateAndRight()
{
	acc = DEACC;			// deaccelerating mode
	left += 30;				// lower right wheel speed
	if(left > 0)			// soft turn if speed becomes 0
	{
		left = 0;	
	}
	deaccelerate();			// deaccelerate
}

/**
	Make left turn while deaccelerating
*/
void deaccelerateAndLeft()
{
	acc = DEACC;			// deaccelerating mode
	right += 30;			// lower right wheel speed
	if(right > 0)			// soft turn if speed becomes 0
	{
		right = 0;
	}
	deaccelerate();			// deaccelerate
}

/**
	Moves the Bot based on the data received from the Zigbee module for Static gestures
*/
void moveBot()
{
	int i = getDirection();		// Get whether to stop or move
	if(i == 0)					// Stop. Hand is upside down
	{
		stop();
	}
	else
	{
		if(acc == ACC && (turn == 0))		// Accelerate only 
		{
			left = right;
//			lcd_string(" ");
			accelerate();
		}
		else if(acc == DEACC && (turn == 0))		// Deaccelerate only
		{
			left = right;
//			lcd_string("D ");
			deaccelerate();
		}
		else if(acc == ACC && turn == RIGHT_TURN)	// Accelerate and Right turn
		{
//			lcd_string("AR ");
			accelerateAndRight();
		}
		else if(acc == ACC && turn == LEFT_TURN)	// Accelerate and Left Turn
		{
//			lcd_string("AL ");
			accelerateAndLeft();
		}
		else if(acc == DEACC && turn == RIGHT_TURN)	// Deaccelerate and Right Turn
		{
//			lcd_string("DR ");
			deaccelerateAndRight();
		}
		else if(acc == DEACC && turn == LEFT_TURN)	// Deaccelerate and Left Turn
		{
//			lcd_string("DL ");
			deaccelerateAndLeft();
		}
		else if(acc == 0 && turn == LEFT_TURN)		// Left Turn only
		{
//			lcd_string("L ");
			left_turn();
		}
		else if(acc == 0 && turn == RIGHT_TURN)		// Right Turn only
		{
//			lcd_string("R ");
			right_turn();
		}
		else										// same as previous instance
		{
//			lcd_string("S ");
		}
	} // end else
} // end function

/**
	Interrupt Handler for the Zigbee module
*/
SIGNAL(SIG_USART0_RECV) 		// ISR for receive complete interrupt
{
	data = UDR0; 				//making copy of data from UDR0 in 'data' variable 

	UDR0 = data; 				//echo data back to PC
	
								// get X, Y, Z values 
	if(flagG== 0){
		X = data;
	}
	else if(flagG== 1){
		Y = data;
	}
	else if(flagG==2){
		Z = data;
	}
	

	flagG++;
	flagG = flagG %3;			// get next set of (X, Y, Z) values
	
	if(flagG == 0)
	{
		shiftVal(X,Y,Z);		// update window
		if(status == STATIC)	// Switch to Static or Dynamic Mode
		{
			moveBot();
		}
		if(status == DYNAMIC)
		{
			dynamicMotion();
		}
	} // end if(flag == 0) 
} // end function 

/**
	Main function
*/
int main(void)
{
	init_devices();			// init ports 
	lcd_set_4bit();			// init lcd
	lcd_init();

	while(1)
	{
		if((PINE & 0x80) == 0x00)		// if switch on bot is pressed to toggle between Static and Dynamic mode
		{
			if(status == DYNAMIC)		// Toggle Mode
			{	
				lcd_cursor(1,1);
				lcd_string("Static");
				status = STATIC;
			}
			else
			{
				lcd_cursor(1,1);
				lcd_string("Dynamic");
				status = DYNAMIC;
			}
		} // end if
	} // end while
} // end main

