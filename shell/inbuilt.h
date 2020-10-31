#ifndef SHELL_INBUILT_H
#define SHELL_INBUILT_H

void echo(char **args, int argscnt);
void pwd(char *dirname);
void cd(char **args, int argscnt, char *actdir, char *olddir);
void envset(char **args, int argscnt);
void envunset(char **args, int argscnt);

#endif //SHELL_INBUILT_H
