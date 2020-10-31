#include "headers.h"
#include "func.h"
#include "prompt.h"
#include "inbuilt.h"
#include "ls.h"
#include "pinfo.h"
#include "proc.h"
#include "history.h"

proc proclist[procmax];

int procscnt = 0, jobcnt = 0, childid = 0;

int exitcode = 2;

char **args;

sigjmp_buf ctrlc_buf;

void sig_handler(int signum) {
    switch (signum) {
        case SIGCHLD:
            signal(SIGCHLD, sig_handler);
            procchk(proclist, &procscnt, &jobcnt);
            break;
        case SIGINT:
        case SIGTSTP:
            signal(SIGINT, sig_handler);
            fflush(stdout);
            printf("\n");
            siglongjmp(ctrlc_buf, 1);
        default:
            printf("unhandled signal\n");
    }
}

int main() {
    int std_out = dup(STDOUT_FILENO), std_in = dup(STDIN_FILENO);
    size_t argscnt, inpbufsz = 0;
    char *inpbuf = NULL, *cmd, *subcmd;
    char dirname[dirsize], actdir[dirsize], olddir[dirsize];
    int forkstatus = 0;
    int st = 0; //status
    int pipefd[2], pipeprev;

    memset(actdir, '\0', sizeof(actdir));
    getcwd(actdir, dirsize);
    olddir[0] = '\0';

    char histfile[dirsize + 10];
    strcpy(histfile, actdir);
    strcat(histfile, "/history.txt");

    if (signal(SIGCHLD, sig_handler) == SIG_ERR)
        perror("Error in signal syscall");
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        perror("Error in signal syscall");
    if (signal(SIGTSTP, sig_handler) == SIG_ERR)
        perror("Error in signal syscall");

    args = (char **) malloc(arglimit * sizeof(char *));

    while (1) {
        while (sigsetjmp(ctrlc_buf, 1) != 0);

        if (prompt(dirname, actdir, &inpbuf, &inpbufsz) == -1) {
            quit(procscnt, std_out, std_in);
            break;
        }

        addhistory(inpbuf, histfile);

        cmd = strtok_r(inpbuf, ";", &inpbuf);
        while (cmd != NULL) {
            pipeprev = dup(STDIN_FILENO);

            if (syntaxchk(cmd)) {
                fprintf(stderr, "Incorrect syntax of command.\n");
                break;
            }

            subcmd = strtok_r(cmd, "|", &cmd);
            while (subcmd != NULL) {
                splitargs(args, &argscnt, " ", subcmd);
                subcmd = strtok_r(cmd, "|", &cmd);

                if (pipe(pipefd)) {
                    perror("Error with pipe");
                    break;
                }
                if (dup2(pipeprev, STDIN_FILENO) == -1) {
                    perror("Error with dup2");
                    break;
                }
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("Error with dup2");
                    break;
                }
                pipeprev = dup(pipefd[0]);
                if (subcmd == NULL)
                    dup2(std_out, STDOUT_FILENO);

                st = io_redirect(args, &argscnt);
                if (!st) {
                    if (args[0] == NULL) continue;
                    else if (strcmp(args[0], "echo") == 0) echo(args, argscnt);
                    else if (strcmp(args[0], "pwd") == 0) pwd(dirname);
                    else if (strcmp(args[0], "cd") == 0) cd(args, argscnt, actdir, olddir);
                    else if (strcmp(args[0], "ls") == 0) printlist(args, argscnt, dirname, actdir);
                    else if (strcmp(args[0], "pinfo") == 0) pinfo(args, argscnt);
                    else if (strcmp(args[0], "history") == 0) printhistory(args, argscnt, histfile);
                    else if (strcmp(args[0], "setenv") == 0) envset(args, argscnt);
                    else if (strcmp(args[0], "unsetenv") == 0) envunset(args, argscnt);
                    else if (strcmp(args[0], "jobs") == 0) jobs(proclist, procscnt, argscnt);
                    else if (strcmp(args[0], "kjob") == 0) kjob(args, argscnt, proclist, procscnt);
                    else if (strcmp(args[0], "fg") == 0) fg(args, argscnt, proclist, &procscnt, &jobcnt);
                    else if (strcmp(args[0], "bg") == 0) bg(args, argscnt, proclist, procscnt);
                    else if (strcmp(args[0], "overkill") == 0) overkill(proclist, procscnt, argscnt);
                    else if (strcmp(args[0], "quit") == 0) {
                        quit(procscnt, std_out, std_in);
                        return 0;
                    }
                    else {
                        childid = fork();

                        if (setpgid(childid, childid))
                            perror("Unsuccessful bg process");

                        if (childid < 0) {
                            perror("Error while forking");
                            exitcode = 0;
                        }
                        else if (childid == 0) {
                            signal(SIGINT, SIG_DFL);
                            signal(SIGTSTP, SIG_DFL);
                            signal(SIGCHLD, SIG_DFL);
                            if (strcmp(args[argscnt - 1], "&") == 0) {
                                args[argscnt - 1] = NULL;
                            }
                            execvp(args[0], args);
                            perror("execvp");
                            exit(1);
                        }
                        else {
                            io_restore(std_out, std_in);
                            if (strcmp(args[argscnt - 1], "&") != 0)    // foreground process
                            {
                                signal(SIGTTOU, SIG_IGN);
                                tcsetpgrp(0, getpgid(childid));
                                signal(SIGTTOU, SIG_DFL);

                                if (waitpid(childid, &forkstatus, WUNTRACED) < 0)
                                    perror("Error in waiting");

                                if (WIFSTOPPED(forkstatus)) {
                                    procadd(proclist, &procscnt, &jobcnt, args[0], childid);
                                    printf("\n[%d] Stopped %d : %s\n", jobcnt, childid, args[0]);
                                    exitcode = 0;
                                }

                                signal(SIGTTOU, SIG_IGN);
                                tcsetpgrp(0, getpgid(getpid()));
                                signal(SIGTTOU, SIG_DFL);
                            }
                            else {
                                procadd(proclist, &procscnt, &jobcnt, args[0], childid);
                                printf("[%d] %d\n", jobcnt, getpid());
                            }
                        }
                        childid = 0;
                    }
                }
                close(pipefd[0]);
                close(pipefd[1]);
            }
            close(pipeprev);

            cmd = strtok_r(inpbuf, ";", &inpbuf);
            io_restore(std_out, std_in);
        }
    }
}