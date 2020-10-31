#ifndef SHELL_PROC_H
#define SHELL_PROC_H

#include "main.h"
void procchk(proc *proclist, int *procscnt, int *jobcnt);
void jobs(proc *proclist, int procscnt, int argscnt);
void overkill(proc *proclist, int procscnt, int argscnt);
void kjob(char **args, int argscnt, proc *proclist, int procscnt);
void bg(char **args, int argscnt, proc *proclist, int procscnt);
void fg(char **args, int argscnt, proc *proclist, int *procscnt, int *jobcnt);
void procadd(proc *proclist, int *procscnt, int *jobcnt, char *name, int id);

#endif //SHELL_PROC_H