#ifndef SHELL_FUNC_H
#define SHELL_FUNC_H

#include "main.h"
void splitargs(char **args, size_t *argscnt, char* delim, char *cmd);
int min(int a, int b);
int max(int a, int b);
int numval(char *str);
void quit(int procscnt, int std_out, int std_in);
void io_restore(int std_out, int std_in);
int io_redirect(char **args, size_t *argscnt);
int syntaxchk(char *cmd);


#endif //SHELL_FUNC_H
