#include <errno.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "include/str.h"

#define MAX_CMDS 100

struct cmd {
	int number_of_commands;
	char **cmd[MAX_CMDS]; // Array of commands
	unsigned int length[MAX_CMDS]; // Length of each command
};

void dummy_handler(int dummy) { return; }

int sh_built_in(struct cmd input, char **env, unsigned int index) {
	// Exit function
	if (strcmp(input.cmd[index][0], "exit") == 0) {
		exit(0);
	}

	// CD into a directory
	if (strcmp(input.cmd[index][0], "cd") == 0) {
		if (input.cmd[index][1] == NULL) {
			char *home = getenv("HOME");
			chdir(home);
			return 0;
		} else {
			if (chdir(input.cmd[index][1]) == 0) {
				return 0;
			} else {
				printf("cd: no such directory\n");
				return 1;
			}
		}
	}

	// history
	if (strcmp(input.cmd[index][0], "history") == 0) {
		HISTORY_STATE *history_state = history_get_history_state();
		HIST_ENTRY **history = history_list();

		if (history_state->length == 0) {
			return 0;
		}

		for (int i = 0; i < history_state->length; i++) {
			printf("%s\n", history[i]->line);
			free_history_entry(history[i]);
		}

		free(history);
		free(history_state);
		return 0;
	}

	// printenv
	if (strcmp(input.cmd[index][0], "printenv") == 0) {
		for (; *env; ++env) {
			printf("%s\n", *env);
		}
		return 0;
	}

	// setenv
	if (strcmp(input.cmd[index][0], "setenv") == 0) {
		if (input.cmd[index][1] == NULL || input.cmd[index][2] == NULL) {
			printf("No environment variable to be set\n");
			return 1;
		} else {
			setenv(input.cmd[index][1], input.cmd[index][2], true);
			return 0;
		}
	}

	// help
	if (strcmp(input.cmd[index][0], "help") == 0) {
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
int sh_exec(struct cmd input, char **env, unsigned int index) {
	int status = 0;

	pid_t pid;
	pid_t wait;

	if (input.cmd[index][0] == NULL)
		return status;

	status = sh_built_in(input, env, 0);
	if (status != -1)
		return status;

	status = posix_spawnp(&pid, input.cmd[index][0], NULL, NULL, input.cmd[index], env);
	waitpid(pid, &status, 0);

	return status;
}

// sh_loop(): The actual shell loop
void sh_loop(int argc, char **argv, char **envp) {
	char *line = NULL;
	char *prompt = "> ";
	struct cmd command;
	char **env = envp;
	int status = 0;

	while (true) {
		signal(SIGINT, dummy_handler);

		line = readline(prompt);

		if (!line)
			break;

		// Remove the new line
		remove_newline(line);

		// Add command to history
		add_history(line);

		const unsigned int command_size = calculate_size_of_split_string(line, " ");
		command.cmd[0] = malloc(command_size * sizeof(char *));
		// Abort if malloc fails
		if (!command.cmd[0]) {
			printf("Failed to allocate command buffer!\n");
			abort();
		}

		// Split the line into an array with the command and it's arguments
		command.length[0] = split_string(line, command.cmd[0], " ", command_size);

		free(line);

		status = sh_exec(command, env, 0);
		printf("%i ", status);

		// Free the pointers in the command array
		free_array_of_strings(command.cmd[0], command.length[0]);
	}

	// Clean Up
	free(command.cmd[0]);
}

int main(int argc, char **argv, char **envp) {
	using_history();
	sh_loop(argc, argv, envp);
	return EXIT_SUCCESS;
}
