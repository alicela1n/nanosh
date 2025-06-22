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

struct cmd {
    char **buffer;
    unsigned int length;
};

int sh_built_in(struct cmd input, char **env) {
    // Exit function
    if (strcmp(input.buffer[0], "exit") == 0) {
	exit(0);
    }

    // CD into a directory
    if (strcmp(input.buffer[0], "cd") == 0) {
	if (input.buffer[1] == NULL) {
	    char *home = getenv("HOME");
	    chdir(home);
	    return 0;
	} else {
	    if (chdir(input.buffer[1]) == 0) {
		return 0;
	    } else {
		printf("cd: no such directory\n");
		return 1;
	    }
	}
    }

    // printenv
    if (strcmp(input.buffer[0], "printenv") == 0) {
	for (; *env; ++env) {
	    printf("%s\n", *env);
	}
	return 0;
    }

    // setenv
    if (strcmp(input.buffer[0], "setenv") == 0) {
	if (input.buffer[1] == NULL || input.buffer[2] == NULL) {
	    printf("No environment variable to be set\n");
	    return 1;
	} else {
	    setenv(input.buffer[1], input.buffer[2], true);
	    return 0;
	}
    }

    // help
    if(strcmp(input.buffer[0], "help") == 0) {
	printf("HELP: \n");
	printf("\tbuiltin commands - \n");
	printf("\tsetenv (Sets environment variables) \n");
	printf("\tprintenv (Prints out the environment variables) \n");
	printf("\tcd (Enters a directory) \n");
	printf("\texit (Exits the shell) \n");
	return 0;
    }
    return -1;
}

// sh_exec(): Execute a command
int sh_exec(struct cmd input, char **env) {
    int status = 0;

    pid_t pid;
    pid_t wait;

    if (input.buffer[0] == NULL)
	return status;

    status = sh_built_in(input, env);
    if (status != -1)
	return status;

    status = posix_spawnp(&pid, input.buffer[0], NULL, NULL, input.buffer, env);
    waitpid(pid, &status, 0);

    return status;
}

// sh_loop(): The actual shell loop
void sh_loop(int argc, char **argv, char **envp) {
    char *line = NULL;
    char *prompt = "> ";
    unsigned int command_size = 512;
    struct cmd command;
    command.buffer = malloc(command_size * sizeof(char *));
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
	command.length = split_string(line, command.buffer, " ", command_size);

	free(line);

	status = sh_exec(command, env);
	printf("%i ", status);

	// Free the pointers in the command array
	free_array_of_strings(command.buffer, command.length);
    }

    // Clean Up
    free(command.buffer);
}

int main(int argc, char **argv, char **envp) {
    sh_loop(argc, argv, envp);
    return EXIT_SUCCESS;
}
