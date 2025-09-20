#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Allocating memory
char *substrCopy(const char *src, size_t len) {
    char *copy = malloc(len + 1);
    if (copy) {
        memcpy(copy, src, len);
        copy[len] = '\0';
    }
    return copy;
}

void increaseCap(char ***tokens, int *count, int *capacity) {
    if (*count >= *capacity) {
        *capacity *= 2;
        *tokens = realloc(*tokens, (*capacity) * sizeof(char *));
    }
}

// Checks if a character is a Special character for the shell or not
int isSpecialChar(char ch) {
    return (ch == '(' || ch == ')' || ch == '<' || ch == '>' || ch == ';' || ch == '|');
}

char **tokenize(const char *input) {
    int capacity = 16;
    char **tokens = malloc(capacity * sizeof(char *));
    
    if (!tokens){
        return NULL;
    }

    char buffer[256];
    int currBuffer = 0;
    int flag = 0; //to check whether inside a double quote

    int i = 0;
    int count = 0;

    while (i < strlen(input)) {
        char current = input[i];

        //Checks if the current character is a double quote (")
        if (current == '"') {
            if (flag) {
                flag = 0;
                buffer[currBuffer] = '\0';
                increaseCap(&tokens, &count, &capacity);
                tokens[count++] = substrCopy(buffer, currBuffer);
                currBuffer = 0;
            } else {
                if (currBuffer > 0) {
                    buffer[currBuffer] = '\0';
                    increaseCap(&tokens, &count, &capacity);
                    tokens[count++] = substrCopy(buffer, currBuffer);
                    currBuffer = 0;
                }
                flag = 1;
            }
            i++;
            continue;
        }

        // Checks if the current character is not inside a double quotes ("")
        if(!flag){

            // Checks if the current character is a whitespace
            if (isspace(current)) {
                if (currBuffer > 0) {
                    buffer[currBuffer] = '\0';
                    increaseCap(&tokens, &count, &capacity);
                    tokens[count++] = substrCopy(buffer, currBuffer);
                    currBuffer = 0;
                }
                i++;
                continue;
            }

            // Checks if the current character is not inside double quotes and a special character ()<>|;
            if (isSpecialChar(current)) {
                if (currBuffer > 0) {
                    buffer[currBuffer] = '\0';
                    increaseCap(&tokens, &count, &capacity);
                    tokens[count++] = substrCopy(buffer, currBuffer);
                    currBuffer = 0;
                }
                if (count >= capacity) {
                    capacity *= 2;
                    tokens = realloc(tokens, capacity * sizeof(char *));
                }
                tokens[count++] = substrCopy(&input[i], 1);
                i++;
                continue;
            }
        }
        buffer[currBuffer++] = current;
        i++;
    }

    if (currBuffer > 0) {
        buffer[currBuffer] = '\0';
        increaseCap(&tokens, &count, &capacity);
        tokens[count++] = substrCopy(buffer, currBuffer);
    }
    tokens[count] = NULL;
    return tokens;
}


//free spaces for all the tokens and list
void freeTokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
