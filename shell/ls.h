#ifndef SHELL_LS_H
#define SHELL_LS_H

void printlist(char **dir, int cnt, char *home, char *actdir);
void printentity(char *name, unsigned int flags);
char *formatdate(char *str, time_t val);

#endif //SHELL_LS_H
