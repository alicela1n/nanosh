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

    // printenv
    if (strcmp(input[0], "printenv") == 0) {
	for (; *env; ++env) {
	    printf("%s\n", *env);
	}
	return 0;
    }

    // setenv
    if (strcmp(input[0], "setenv") == 0) {
	if (input[1] == NULL || input[2] == NULL) {
	    printf("No environment variable to be set\n");
	    return 1;
	} else {
	    setenv(input[1], input[2], true);
	    return 0;
	}
    }

    // help
    if(strcmp(input[0], "help") == 0) {
	printf("HELP: \n");
	printf("\tbuiltin commands - \n");
	printf("\tsetenv (Sets environment variables) \n");
	printf("\tprintenv (Prints out the environment variables) \n");
	printf("\tcd (Enters a directory) \n");
	printf("\texit (Exits the shell) \n");
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
