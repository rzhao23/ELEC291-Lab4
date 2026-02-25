#include "utils.h"
#include "uart.h"
#include <string.h>

// convert a float number to string with two decimal places
void float_to_str(float number, char* buffer){
	unsigned int number_holder;
	char temp[16];
	char i;
	char j;
	char print_length = 0;

	number_holder = (unsigned int)number;

	if(number_holder == 0) {
		temp[0] = '0';
		i = 1;
	}

	else {
		i = 0;
		while(number_holder > 0){
			temp[i] = (number_holder % 10) + '0';
			number_holder = number_holder / 10;
			i++;
		}

		number_holder = (unsigned int)(number * 100) - (unsigned int)(number) * 100;
		for(j = 0; j < 2; j++){
			temp[i] = (number_holder % 10) + '0';
			number_holder = number_holder / 10;
			i++;
		}

		print_length = i - 1;
	}

	j = 0;
	for(i = print_length-2; i >= 0; i--){
		buffer[j] = temp[i];
		j++;	
	}

	buffer[j] = '.';

	// print decimal point numbers
	buffer[j+1] = temp[print_length];
	buffer[j+2] = temp[print_length-1];
	buffer[j+3] = '\0';
}

void cmd_help(unsigned char argc, char *argv[]) reentrant {
	unsigned char i;
	UART_send_string("Available commands:\r\n");
    for (i = 0; i < cli_cmd_count; i++)
    {
        UART_send_string("  ");
        UART_send_string(cli_commands[i].name);
        UART_send_string(" - ");
        UART_send_string(cli_commands[i].help);
        UART_send_string("\r\n");
    }
}

void cmd_mode(unsigned char argc, char *argv[]) reentrant {
}

void cmd_round(unsigned char argc, char *argv[]) reentrant {
}

const cli_cmd_t code cli_commands[] = {
    {"help",  "Show all commands", cmd_help},
    {"mode",  "Measure C or L",    cmd_mode},
    {"round", "Round fragile",     cmd_round},
};

const unsigned char cli_cmd_count =
    sizeof(cli_commands) / sizeof(cli_commands[0]);

void cli_parse_and_dispatch(char *input) {
	unsigned char argc = 0;
	char *argv[8];
	unsigned char i;
	char in_arg = 0;

	for (i = 0; input[i] != '\0'; i++) {
		if (input[i] == ' ') {
			input[i] = '\0';
			in_arg = 0;
		}
		else if (!in_arg) {
			if (argc < 8) {
				argv[argc++] = &input[i];
			}
			in_arg = 1;
		}
	}

	if (argc == 0) return;

	for (i = 0; i < cli_cmd_count; i++) {
		if (strcmp(argv[0], cli_commands[i].name) == 0)
        {
            cli_commands[i].handler(argc, argv);
            return;
        }
	}
	UART_send_string("Unknown command: ");
    UART_send_string(argv[0]);
    UART_send_string("\r\n");
}
