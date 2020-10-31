#ifndef SHELL_PROMPT_H
#define SHELL_PROMPT_H

#include <glob.h>
int prompt(char *dirname, char* actdir, char **buf, size_t *bufsize);
char *curdir(char *dirname, char* actdir);

#endif //SHELL_PROMPT_H
