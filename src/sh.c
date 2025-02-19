#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <spawn.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "include/io.h"
#include "include/str.h"

int sh_built_in(char **input, const unsigned int input_length, char **env) {
    // Exit function
    if (strcmp(input[0], "exit") == 0) {
	exit(0);
    }

    // CD into a directory
    if (strcmp(input[0], "cd") == 0) {
	if (input[1] == NULL) {
	    char *home = getenv("HOME");
	    chdir(home);
	    return 0;
	} else {
	    if (chdir(input[1]) == 0) {
		return 0;
	    } else {
		printf("cd: no such directory\n");
		return 1;
	    }
	}
    }

    return -1;
}

// sh_exec(): Execute a command
int sh_exec(char **input, const unsigned int input_length, char **env) {
    int status = 0;
    pid_t pid;
    pid_t wait;

    if (input[0] == NULL)
	return status;

    if (sh_built_in(input, input_length, env) != -1)
	return status;

    status = posix_spawnp(&pid, input[0], NULL, NULL, input, env);
    waitpid(pid, &status, 0);

    return status;
}

// sh_loop(): The actual shell loop
void sh_loop(int argc, char **argv, char **envp) {
    char *line = NULL;
    char *prompt = "> ";
    unsigned int command_size = 512;
    char **command = malloc(command_size * sizeof(char *));
    unsigned int command_length = 0;
    char **env = envp;
    int status = 0;

    while (true) {
	signal(SIGINT, dummy_handler);

	printf("%s", prompt);
	line = get_line(stdin);

	if (!line)
	    break;

	// Remove the new line
	remove_newline(line);

	// Split the line into an array with the command and it's arguments
	command_length = split_string(line, command, " ", command_size);

	free(line);

	status = sh_exec(command, command_length, env);
	printf("%i ", status);

	// Free the pointers in the command array
	free_array_of_strings(command, command_length);
    }

    // Clean Up
    free(command);
}

int main(int argc, char **argv, char **envp) {
    sh_loop(argc, argv, envp);
    return EXIT_SUCCESS;
}
