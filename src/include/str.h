#ifndef STR_H_
#define STR_H_

unsigned int split_string(char *input, char **output, const char *delimiter, const unsigned int max_length);
unsigned int calculate_size_of_split_string(char *input, const char *delimiter);

void remove_newline(char *input);

void free_array_of_strings(char **input, unsigned int input_length);

#endif
