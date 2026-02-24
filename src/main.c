// FreqEFM8.c: Measure the frequency of a signal on pin T0.
//
// By:  Jesus Calvino-Fraga (c) 2008-2018
//
// The next line clears the "C51 command line options:" field when compiling with CrossIDE
//  ~C51~

#include <EFM8LB1.h>
#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "lcd.h"
#include "utils.h"

// value of RA and RB
#define RA 			9841
#define RB			9811

#define LA			0.269625737298F

void main (void) 
{
	unsigned long F;
	float capacitance;
	float inductance;
	char num_buff[10];
	char print_buff[17];
	
	TIMER0_Init();
	LCD_4BIT();

	waitms(500); // Give PuTTY a chance to start.
	printf("\x1b[2J"); // Clear screen using ANSI escape sequence.

	printf ("EFM8 Frequency measurement using Timer/Counter 0.\n"
	        "File: %s\n"
	        "Compiled: %s, %s\n\n",
	        __FILE__, __DATE__, __TIME__);

	while(1)
	{
TL0=0;
		TH0=0;
		overflow_count=0;
		TF0=0;
		TR0=1; // Start Timer/Counter 0
		waitms(1000);
		TR0=0; // Stop Timer/Counter 0
		F=overflow_count*0x10000L+TH0*0x100L+TL0;

		/*
		// capacitance calculation
		// scaled by 1e6
		capacitance = 1440000.0/((RA+2*RB)*F);
		capacitance = capacitance * 1000;

		// put the number into a string
		float_to_str(capacitance, num_buff);
		
		// setup the print message
		// message format: C = xx.xx nF
		strcpy(print_buff, "C = ");
		strcat(print_buff, num_buff);
		strcat(print_buff, " nF");
		
		LCDprint(print_buff, 1, 1);
		printf("%s\n", print_buff); 
		*/

		inductance = 1/(LA*F/1000); // inductance measured in H
		inductance = inductance * 1000; // inductance in mH
		printf("%f mH\n", inductance);

		printf("\rf=%luHz\n", F);
		printf("\x1b[0K"); // ANSI: Clear from cursor to end of line.
	}
	
}
