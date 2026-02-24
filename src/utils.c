#include "utils.h"

// convert a float number to string with two decimal places
void float_to_str(float number, char* buffer){
	unsigned int number_holder;
	char temp[16];
	char i;
	char j;
	char print_length;

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
