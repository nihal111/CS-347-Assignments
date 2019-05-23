#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int parseCommand(char raw_command[], char* args[]);

void printArray(char* args[], int numArgs);

void execute(char* args[]);

int main() {
    while (1) {
        char raw_command[200];
        printf("CS347(M)$ ");
        fgets(raw_command, 200, stdin);
        strtok(raw_command, "\n");

        char* args[100];
        int numArgs = parseCommand(raw_command, args);

        if (strcmp(args[0], "exit") == 0) {
            exit(0);
        } else if (strcmp(args[0], "name") == 0) {
            printf("Arpan Banerjee\nNihal Singh\nSrivatsan Sridhar\n");
        } else if (strcmp(args[0], "roll") == 0) {
            printf("%s - %s\n%s - %s\n%s - %s\n", "150070011", "Arpan", "150040015", "Nihal", "150070005", "Srivatsan");
        } else {
            execute(args);
        }
    }
    return 0;
}

void execute(char* args[]) {
    pid_t pid;
    int status;
    char commandDir[] = "/bin/";
    strcat(commandDir, args[0]);
    strcpy(args[0], commandDir);
    if ((pid = fork()) == 0) {
        execvp(commandDir, args);
        printf("Invalid command.\n");
    } else {
        while(wait(&status) != pid);
    }
}

int parseCommand(char raw_command[], char* args[]) {
    int i = 0;
    int ARG_SIZE = 200;
    char* p = strtok(raw_command, " ");

    while (p != NULL)
    {
        args[i] = malloc((ARG_SIZE+1) * sizeof(char));
        strcpy(args[i++], p);
        p = strtok(NULL, " ");
    }
    args[i] = NULL;
    return i+1;
}

void printArray(char* args[], int numArgs) {
    int i;
    for (i=0 ; i < numArgs-1 ; i++) {
        printf("%s ", args[i]);
    }
    printf("%s", args[i]);
}