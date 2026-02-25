// FreqEFM8.c: Measure the frequency of a signal on pin T0.
//
// By:  Jesus Calvino-Fraga (c) 2008-2018
//
// The next line clears the "C51 command line options:" field when compiling with CrossIDE
//  ~C51~

#include <EFM8LB1.h>
#include <string.h>
#include "timer.h"
#include "lcd.h"
#include "utils.h"
#include "uart.h"

// value of RA and RB
#define RA 			9841
#define RB			9811
#define LA			1  // adjust to your actual value

// helper: convert unsigned long to string
void ulong_to_str(unsigned long n, char *buf)
{
	char temp[12];
	char i = 0;
	char j = 0;

	if (n == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	while (n > 0) {
		temp[i++] = (n % 10) + '0';
		n /= 10;
	}

	// reverse
	while (i > 0) {
		buf[j++] = temp[--i];
	}
	buf[j] = '\0';
}

void main (void) 
{
	unsigned long F;
	float capacitance;
	float inductance;
	xdata char num_buff[10];
	xdata char print_buff[17];
	xdata char freq_buff[12];

	UART_init();
	TIMER0_Init();
	LCD_4BIT();

	waitms(500);

	// Clear screen
	UART_send_string("\x1b[2J");

	// Startup message
	UART_send_string("EFM8 Frequency measurement using Timer/Counter 0.\r\n");
	UART_send_string("File: ");
	UART_send_string(__FILE__);
	UART_send_string("\r\nCompiled: ");
	UART_send_string(__DATE__);
	UART_send_string(", ");
	UART_send_string(__TIME__);
	UART_send_string("\r\n\r\n");

	while(1)
	{
		TL0=0;
		TH0=0;
		overflow_count=0;
		TF0=0;
		TR0=1; // Start Timer/Counter 0
		waitms(1000);
		TR0=0; // Stop Timer/Counter 0
		F = overflow_count*0x10000L+TH0*0x100L+TL0;

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
		UART_send_string(print_buff);
		UART_send_string("\r\n");
		*/

		// inductance calculation
		inductance = 1/(LA*F/1000); // inductance measured in H
		inductance = inductance * 1000; // inductance in mH

		// print inductance
		float_to_str(inductance, num_buff);
		UART_send_string(num_buff);
		UART_send_string(" mH\r\n");

		// print frequency
		UART_send_string("\rf=");
		ulong_to_str(F, freq_buff);
		UART_send_string(freq_buff);
		UART_send_string("Hz\r\n");

		UART_send_string("\x1b[0K"); // ANSI: Clear to end of line
	}
	
}