#include <stdio.h>
#include <stdlib.h>

// *get_line(): Read a line and return it as a string
char *get_line(FILE *stream) {
    // For performance reasons memory is being allocated in chunks
    const size_t chunk = 4096; // Size of buffer
    size_t max_len = chunk;
    char *line = malloc(chunk + 1); // Allocate the size of the line buffer
    if (!line)
        return NULL;

    size_t i = 0;

    // Get the input from the user and store it in the line string
    for (char ch = fgetc(stream); ch != EOF; ch = fgetc(stream)) {

        // Allocate more space when the limit is reached
        if (i == max_len) {
            max_len += chunk;
            char *const tmp = realloc(line, max_len + 1);
            if (!tmp) {
                free(line);
                return NULL;
            }
            line = tmp;
        }

        // Add the character to the line
        line[i] = ch;
        i++;

        if (ch == '\n')
            break;
    }

    line[i] = '\0';
    if (!line[0]) {
        free(line);
        return NULL;
    }

    return line;
}

void dummy_handler(int dummy) {
    return;
}
