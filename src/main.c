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
#include "cmd.h"

// value of RA and RB
#define RA 			9841
#define RB			9811
#define LA			1  // adjust to your actual value

// inductance calculation
void cal_inductance (unsigned long F, float* inductance) {
	*inductance = 1/(LA*F/1000); // inductance measured in H
	*inductance = (*inductance) * 1000; // inductance in mH
}

// capacitance calculation
// scaled by 1e6
void cal_capacitance (unsigned long F, float* capacitance) {
	*capacitance = 1440000.0/((RA+2*RB)*F);
	*capacitance = (*capacitance) * 1000;
}

void cal_freq(unsigned long* F){
	TL0 = 0;
	TH0 = 0;
	overflow_count = 0;
	TF0 = 0;
	TR0 = 1; // Start Timer/Counter 0
	waitms(1000);
	TR0 = 0; // Stop Timer/Counter 0
	*F = (overflow_count * 0x10000L) + (TH0 * 0x100L) + TL0;
}

bit do_meansure_flag = 0;  // global: set by cmd_read, cleared by cmd_kill
bit c_or_h_flag = 0; // toggle by cmd_mode

void main (void)
{
	unsigned long F;
	float capacitance;
	float inductance;
	
	
	xdata char num_buff[10];
	xdata char print_buff[17];
	xdata char freq_buff[12];
	xdata char cmd_buf[32];
	unsigned char cmd_len = 0;

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
		cal_freq(&F);

		// Process any chars buffered by the UART ISR during the 1-second cal_freq
		while (UART_available()) {
			char c = UART_read();
			if (c == '\r' || c == '\n') {
				if (cmd_len > 0) {
					cmd_buf[cmd_len] = '\0';
					UART_send_string("\r\n");
					cli_parse_and_dispatch(cmd_buf);
					cmd_len = 0;
				}
			} else if (cmd_len < 31) {
				cmd_buf[cmd_len++] = c;
				UART_send_char(c);  // echo
			}
		}

		if (c_or_h_flag) {
			cal_capacitance(F, &capacitance);
			float_to_str(capacitance, num_buff);
			if (do_meansure_flag) {
				UART_send_string(num_buff);
				UART_send_string(" nF\r\n");
			}
			// message format: C = xx.xx nF
			strcpy(print_buff, "C = ");
			strcat(print_buff, num_buff);
			strcat(print_buff, " nF");
		}
		else {
			cal_inductance(F, &inductance);
			float_to_str(inductance, num_buff);
			if (do_meansure_flag) {
				UART_send_string(num_buff);
				UART_send_string(" mH\r\n");
			}
			// message format: H = xx.xx mH
			strcpy(print_buff, "H = ");
			strcat(print_buff, num_buff);
			strcat(print_buff, " mH");
		}
		
		LCDprint(print_buff, 1, 1);

		/*
		// print frequency
		UART_send_string("\rf=");
		ulong_to_str(F, freq_buff);
		UART_send_string(freq_buff);
		UART_send_string("Hz\r\n");

		UART_send_string("\x1b[0K"); // ANSI: Clear to end of line
		*/
	}
	
}