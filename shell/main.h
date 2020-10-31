#ifndef SHELL_MAIN_H
#define SHELL_MAIN_H

typedef struct proc {
    char *pname;
    int pid;
    int jobid;
} proc;

void sig_handler(int signum);

#endif //SHELL_MAIN_H
