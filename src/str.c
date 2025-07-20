#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// split_string(): Take a string, array of pointers, delimiter, and max length as input,
//                 split the string based on the delimiter and return the number
//                 of strings in the array as output.
unsigned int split_string(char *input, char **output, const char *delimiter, const unsigned int max_length) {
	char *saveptr;
	char *token = strtok_r(input, delimiter, &saveptr);
	unsigned int length = 0;

	while (token != NULL) {
		if (length == max_length)
			exit(0);

		*output = strdup(token);

		if (*output == NULL)
			exit(0);

		token = strtok_r(NULL, delimiter, &saveptr);
		length++;
		output++;
	}
	*output = NULL;

	return length;
}

// calculate_size_of_split_string(): Just calculate the size of the array for the split string
unsigned int calculate_size_of_split_string(char *input, const char *delimiter) {
	char *saveptr;
	char *token = strtok_r(input, delimiter, &saveptr);
	unsigned int length = 0;

	while (token != NULL) {
		token = strtok_r(NULL, delimiter, &saveptr);
		length++;
	}
}

// remove_newline(): Remove a new line at the end of a string
void remove_newline(char *input) {
	if (input[strlen(input) - 1] == '\n')
		input[strlen(input) - 1] = '\0';
}

// free_array_of_strings(): Frees an array of strings
void free_array_of_strings(char **input, unsigned int input_length) {
	for (int i = 0; i < input_length; i++) {
		free(input[i]);
	}
}
