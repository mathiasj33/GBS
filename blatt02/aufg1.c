#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

char *getInput(int initialLength) {
    char *buffer;
    buffer = malloc(sizeof(*buffer) * initialLength);
    int i = 0;
    while (read(0, buffer + i, 1) != 0 && buffer[i] != '\n') { //return value of 0 => EOF
        i++;
        if (i == initialLength) {
            initialLength *= 2;
            buffer = realloc(buffer, sizeof(*buffer) * initialLength);
        }
    }
    buffer[i] = '\0';

    return realloc(buffer, sizeof(*buffer) * (i + 1));
}

void main(void) {
    char input[256];

    while(true) {
        fgets(input, 256, stdin);
        input[strlen(input)-1] = '\0';
        if(strcmp(input, "exit") == 0) {
            return;
        }
        system(input);
    }
}