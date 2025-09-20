#ifndef TOKENISE_IMP_H
#define TOKENISE_IMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *substrCopy(const char *src, size_t len);
void increaseCap(char ***tokens, int *count, int *capacity);
int isSpecialChar(char ch);
char **tokenize(const char *input);
void freeTokens(char **tokens);

#endif
