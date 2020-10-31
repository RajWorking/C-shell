#ifndef SHELL_HISTORY_H
#define SHELL_HISTORY_H

void printhistory(char **args, int argscnt, char *histfile);
void addhistory(char *inp, char *histfile);

#endif //SHELL_HISTORY_H
