#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH_ENTERED_BY_USER 100

void string_to_lower(char* str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; ++i) {
        str[i] = tolower(str[i]);
    }
}

void create_file() {

}

void delete_file() {

}

void show_file() {

}

void record_file() {

}

void execute_command() {
    printf("\n>>> ");
    char input_string[MAX_LINE_LENGTH_ENTERED_BY_USER];
    if (fgets(input_string, sizeof(input_string), stdin) == NULL) {
        printf("Error when reading a line.\n");
        return;
    }
    char *command = strtok(input_string, " \t\n");
    string_to_lower(command);
    if (command == NULL) {
        return;
    }
    else if (!strcmp(command, "create")) {
        create_file();
    }
    else if (!strcmp(command, "delete")) {
        delete_file();
    }
    else if (!strcmp(command, "read")) {
        show_file();
    }
    else if (!strcmp(command, "write")) {
        record_file();
    }
    else {
        printf("The \"%s\" command was not found.", command);
    }
}