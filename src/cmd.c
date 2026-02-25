#include <EFM8LB1.h>
#include <string.h>
#include "cmd.h"
#include "uart.h"

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

void cmd_kill(unsigned char argc, char *argv[]) reentrant {
    do_meansure_flag = 0;
    UART_send_string("Output stopped.\r\n");
}

void cmd_read(unsigned char argc, char *argv[]) reentrant {
    do_meansure_flag = 1;
    UART_send_string("Output started.\r\n");
}

void cmd_mode(unsigned char argc, char *argv[]) reentrant {
    c_or_h_flag = !c_or_h_flag;
}

void cmd_round(unsigned char argc, char *argv[]) reentrant {

}

const cli_cmd_t code cli_commands[] = {
    {"help",  "Show all commands", cmd_help},
    {"K", "Stop Read C/H", cmd_kill},
    {"R", "Start Read C/H", cmd_read},
    {"mode",  "Measure C or L", cmd_mode},
    {"round", "Round fragile", cmd_round},
};

const unsigned char cli_cmd_count = 5;

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
