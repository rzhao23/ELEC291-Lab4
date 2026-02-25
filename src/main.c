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
#define CA			1.11F
#define LA			0.269625737298F  // adjust to your actual value

void cal_resistence (unsigned long F, float* resistance) {
	*resistance = (1440000.0)/(CA * F) - 2 * RB;
}

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

// global varaibles
bit do_meansure_flag = 0;  // setb by cmd_read, clr by cmd_kill
xdata unsigned char CLR_flag = 0; // updated by cmd_mode
bit boot_flag = 0;

void main (void)
{
	unsigned long F;
	float resistance;
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
	LCDprint("Welcome to Use", 1, 1);
	LCDprint("Option C/L/R", 2, 1);

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
				// UART_send_char(c);  // echo
			}
		}

		if (boot_flag == 1) {
			if (CLR_flag == 0) {
				cal_capacitance(F, &capacitance);
				float_to_str(capacitance, num_buff);

				if (capacitance < 2000 && capacitance > 0.1) {

					// message format: C = xx.xx nF
					strcpy(print_buff, "C = ");
					strcat(print_buff, num_buff);
					strcat(print_buff, " nF");

					if (do_meansure_flag) {
						UART_send_string(num_buff);
						UART_send_string(" nF\r\n");
						LCDprint("Capacitance:", 1, 1);
						LCDprint(print_buff, 2, 1);
					}
					else {
						LCDprint("Capacitance:", 1, 1);
						LCDprint("Measure Pause   ", 2, 1);
					}
				}
				else {
					LCDprint("Capacitance:", 1, 1);
					LCDprint("Measure Pause   ", 2, 1);
				}
			}
			else if (CLR_flag == 1) {
				cal_inductance(F, &inductance);
				float_to_str(inductance, num_buff);

				if (inductance < 60000) {
					// message format: H = xx.xx mH
					strcpy(print_buff, "H = ");
					strcat(print_buff, num_buff);
					strcat(print_buff, " mH");

					if (do_meansure_flag) {
						UART_send_string(num_buff);
						UART_send_string(" mH\r\n");
						LCDprint("Inductance:", 1, 1);
						LCDprint(print_buff, 2, 1);
					}
					else {
						LCDprint("Inductance:", 1, 1);
						LCDprint("Measure Pause   ", 2, 1);
					}
				}
				else {
					LCDprint("Inductance:", 1, 1);
					LCDprint("Measure Pause   ", 2, 1);
				}
			}
			else if (CLR_flag == 2) {
				cal_resistence(F, &resistance);
				float_to_str(resistance, num_buff);

				if (resistance < 60000) {
					strcpy(print_buff, "R = ");
					strcat(print_buff, num_buff);
					strcat(print_buff, " ohm");

					if (do_meansure_flag) {
						UART_send_string(num_buff);
						UART_send_string(" ohm\r\n");
						LCDprint("Resistance:", 1, 1);
						LCDprint(print_buff, 2, 1);
					}
					else {
						LCDprint("Resistance:", 1, 1);
						LCDprint("Measure Pause   ", 2, 1);
					}
				}
				else {
					LCDprint("Resistance:", 1, 1);
					LCDprint("Measure Pause   ", 2, 1);
				}
				
			}
		}

		

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