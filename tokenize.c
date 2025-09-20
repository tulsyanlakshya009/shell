#include "tokenize_imp.h"
#include <stdio.h>
#include <string.h>

//print all tokens
void printTokens(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        printf("%s\n", tokens[i]);
    }
}

int main(void) {
    char inputLine[255];

    if (!fgets(inputLine, sizeof(inputLine), stdin)) {
        fprintf(stderr, "Error reading input.\n");
        return 1;
    }
    inputLine[strcspn(inputLine, "\n")] = '\0';

    char **tokens = tokenize(inputLine);
    printTokens(tokens);
    freeTokens(tokens);
    return 0;
}
