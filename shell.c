#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "tokenize_imp.h"

//counts the number of tokens
int getTokenCount(char **tokens) {
    int count = 0;
    while (tokens[count] != NULL) {
        count++;
    }
    return count;
}

void executePipe(char **tokens, int token_count);
void executeRedirection(char **tokens, int token_count);

// displays the mini shell commands
void executeHelp() {
    printf("Mini Shell Commands\n");
    printf("  cd <directory>  Change the current directory\n");
    printf("  exit            Exit the shell\n");
    printf("  help            Display help information\n");
    printf("  prev            Repeat the previous command\n");
}

void executeCommand(char **tokens) {
    //checking for cd command
    if (strcmp(tokens[0], "cd") == 0) {
        if (tokens[1] == NULL) {
            fprintf(stderr, "cd: missing operand\n");
        } else if (chdir(tokens[1]) != 0) {
            perror("cd failed");
        }
        return;
    }

    //checking for 'help' command
    if (strcmp(tokens[0], "help") == 0) {
        executeHelp();
        return;
    }

    //checking for 'exit' command
    if (strcmp(tokens[0], "exit") == 0) {
        printf("Bye bye.\n");
        exit(0);
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        execvp(tokens[0], tokens);
        fprintf(stderr, "%s: command not found\n", tokens[0]); //if command not found
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("fork failed");
    }
}

void execute(char **tokens, int token_count) {

    //checking for multiple commands in a sequence to be executed
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], ";") == 0) {
            tokens[i] = NULL;
            execute(tokens, i);
            execute(tokens + i + 1, token_count - i - 1);
            return;
        }
    }

    //checking for pipe command
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            executePipe(tokens, token_count);
            return;
        }
    }
    
    //checking for redirection command
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0) {
            executeRedirection(tokens, token_count);
            return;
        }
    }
    
    //executing the command
    if (token_count > 0 && tokens[0] != NULL) {
        executeCommand(tokens);
    }
}

// handles commands with pipes '|'
void executePipe(char **tokens, int token_count) {
    int numPipes = 0;
    // counts the total number of pipes by iterating through the token array
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "|") == 0) numPipes++;
    }

    int numCommands = numPipes + 1;
    char ***commands = malloc(numCommands * sizeof(char **));
    int cmd_start = 0, cmd_index = 0;
    
    // splits the token into seperate commands 
    for (int i = 0; i <= token_count; i++) {
        if (i == token_count || strcmp(tokens[i], "|") == 0) {
            tokens[i] = NULL;
            commands[cmd_index++] = &tokens[cmd_start];
            cmd_start = i + 1;
        }
    }
    
    // creates pipes
    int pipefds[2 * numPipes];
    for (int i = 0; i < numPipes; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe failed");
            exit(1);
        }
    }
    
    for (int i = 0; i < numCommands; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO); //redirecting the input of the command
            } 
            if (i < numCommands - 1) { //redirecting the output of the command
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO); 
            }
            
            for (int j = 0; j < 2 * numPipes; j++) {
                close(pipefds[j]); 
            }
            executeRedirection(commands[i], getTokenCount(commands[i])); 
            exit(1);
        }
    }
    
    for (int i = 0; i < 2 * numPipes; i++) {
        close(pipefds[i]);
    }
    for (int i = 0; i < numCommands; i++) {
        wait(NULL);
    }
    
    free(commands);
}

// handles commands with redirection '<' and '>'
void executeRedirection(char **tokens, int token_count) {
    char *inputFile = NULL, *outputFile = NULL;
    char *commands[255];
    int j = 0;
    
   // processes the command tokens and extracts input/output redirection.
    for (int i = 0; i < token_count; i++) {
        // If '<' is found then the next token is set as inputFile.
        if (strcmp(tokens[i], "<") == 0) {
            if (tokens[i+1] != NULL) {
                inputFile = tokens[i+1];
                i++;
            } else {
                fprintf(stderr, "Missing input file\n");
                return;
            }
        }
        // If '>' is found then the next token is set as outputFile.
        else if (strcmp(tokens[i], ">") == 0) {
            if (tokens[i+1] != NULL) {
                outputFile = tokens[i+1];
                i++;
            } else {
                fprintf(stderr, "Missing output file\n");
                return;
            }
        } else {
            commands[j++] = tokens[i];
        }
    }
    commands[j] = NULL;
    
    pid_t pid = fork();
    if (pid == 0) {
        if (inputFile) {
            int fd_in = open(inputFile, O_RDONLY); //opening the input file
            if (fd_in < 0) {
                perror("Failed to open input file");
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO); //redirecting the input file to stdin
            close(fd_in);
        }
        if (outputFile) {
            int fd_out = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644); //opening the output file
            if (fd_out < 0) {
                perror("Failed to open output file");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO); //redirecting the output file to stdout
            close(fd_out);
        }
        execvp(commands[0], commands);
        perror("execvp failed");
        exit(1);
    }
    wait(NULL);
}

int main(void) {
    char input[255];
    char prevCommand[255] = "";
    
    printf("Welcome to mini-shell.\n");
    
    //running the shell unitl exit command is given
    while (1) {
        printf("shell $ ");
        fflush(stdout);
        
        //if cannot read input, exit the shell
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Bye bye.\n");
            break; 
        }
        
        //if command is exit, exit the shell
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "exit") == 0) {
            printf("Bye bye.\n");
            break;
        }
        
        //handling previous command
        if (strcmp(input, "prev") == 0) {
            if (strlen(prevCommand) > 0) {
                printf("%s\n", prevCommand);
                strcpy(input, prevCommand);
            } else {
                fprintf(stderr, "No previous command.\n");
                continue;
            }
        } else {
            strcpy(prevCommand, input);
        }
        
        //tokenize the input and execute the command
        char **tokens = tokenize(input);
        if (tokens) {
            int token_count = getTokenCount(tokens);
            if (token_count > 0) {
                execute(tokens, token_count);
            }
            freeTokens(tokens);
        }
    }
    return 0;
}