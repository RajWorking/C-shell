#include "headers.h"
#include "proc.h"
#include "func.h"
#include "main.h"

extern int exitcode;

void procchk(proc *proclist, int *procscnt, int *jobcnt) {
    int remcnt = 0, status = 0, endid;
    *jobcnt = 0;
    proc rem[procmax];

    for (int i = 0; i < *procscnt; i++) {
        endid = waitpid(proclist[i].pid, &status, WNOHANG);
        if (endid == -1) {
            perror("waitpid error");
            exit(EXIT_FAILURE);
        }
        else if (endid == proclist[i].pid) {
            if (WIFEXITED(status))
                fprintf(stderr, "%s process with pid %d ended normally.\n", proclist[i].pname, proclist[i].pid);
            else if (WIFSIGNALED(status))
                fprintf(stderr, "%s process with pid %d ended by a SIGNAL.\n", proclist[i].pname,
                        proclist[i].pid);
            else if (WIFSTOPPED(status))
                fprintf(stderr, "%s process with pid %d has STOPPED.\n", proclist[i].pname, proclist[i].pid);
            else
                fprintf(stderr, "%s process with pid %d ended ABNORMALLY.\n", proclist[i].pname, proclist[i].pid);
        }
        else if (endid == 0) {
            rem[remcnt] = proclist[i];
            remcnt++;
            *jobcnt = max(*jobcnt, proclist[i].jobid);
        }
    }

    for (int i = 0; i < remcnt; i++) {
        proclist[i] = rem[i];
    }

    *procscnt = remcnt;
}

void jobs(proc *proclist, int procscnt, int argscnt) {

    exitcode = 1;
    if (argscnt != 1) {
        fprintf(stderr, "Too many arguments!\n");
        exitcode = 0;
        return;
    }

    char state[10], procfile[100];
    char *buf = NULL;
    char **stats = malloc(arglimit * sizeof(char *));
    size_t bufsz = 0, statscnt = 0;

    for (int i = 0; i < procscnt; i++) {
        sprintf(procfile, "/proc/%d/stat", proclist[i].pid);

        FILE *file = fopen(procfile, "r");

        if (file == NULL) {
            perror("Invalid pid");
            exitcode = 0;
            continue;
        }

        getline(&buf, &bufsz, file);
        splitargs(stats, &statscnt, " ", buf);

        switch (stats[2][0]) {
            case 'R':
            case 'S':
                strcpy(state, "Running");
                break;
            case 'T':
            case 'Z':
                strcpy(state, "Stopped");
                break;
            default:
                strcpy(state, "Unknown");
        }

        printf("[%d] %s %s [%d]\n", proclist[i].jobid, state, proclist[i].pname, proclist[i].pid);

        fclose(file);
    }
    free(stats);
}

void kjob(char **args, int argscnt, proc *proclist, int procscnt) {

    exitcode = 0;
    if (argscnt != 3) {
        fprintf(stderr, "kjob should include 2 arguments jobid and signal!\n");
        return;
    }

    for (int i = 0; i < procscnt; i++) {
        if (proclist[i].jobid == numval(args[1])) {
            if (kill(proclist[i].pid, numval(args[2])))
                perror("Process could not be killed");
            else
                exitcode = 1;
            return;
        }
    }

    fprintf(stderr, "%d jobid does not exist\n", numval(args[1]));
}

void overkill(proc *proclist, int procscnt, int argscnt) {
    exitcode = 1;
    if (argscnt != 1) {
        fprintf(stderr, "Too many arguments!\n");
        exitcode = 0;
        return;
    }

    for (int i = 0; i < procscnt; i++) {
        if (kill(proclist[i].pid, SIGKILL))
            exitcode = 0;
    }
}

void fg(char **args, int argscnt, proc *proclist, int *procscnt, int *jobcnt) {

    exitcode = 0;

    if (argscnt != 2) {
        fprintf(stderr, "Incorrect number of arguments!\n");
        return;
    }

    int id = numval(args[1]), flg = 0;
    char name[500];

    for (int i = 0; i < *procscnt; i++) {
        if (proclist[i].jobid == id) {
            id = proclist[i].pid;
            strcpy(name, proclist[i].pname);
            flg = 1;

            for (int j = i + 1; j < *procscnt; j++) {
                proclist[j - 1] = proclist[j];
            }
            (*procscnt)--;

            break;
        }
    }
    *jobcnt = 0;
    for (int i = 0; i < *procscnt; i++) {
        *jobcnt = max(*jobcnt, proclist[i].jobid);
    }

    if (!flg) {
        fprintf(stderr, "Incorrect job id!\n");
        return;
    }

    if (kill(id, SIGCONT)) {
        perror("Process could not be continued");
        return;
    }

    exitcode = 1;

    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    tcsetpgrp(0, getpgid(id));
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);

    printf("%d pid was resumed in the foreground\n", id);
//    printf("...waiting for %d\n",getpgid(id));

    signal(SIGTSTP, SIG_IGN);

    int st = 0;
    if (waitpid(id, &st, WUNTRACED) < 0) {
        printf("waitpid error\n");
        exitcode = 0;
    }

    if (WIFSTOPPED(st)) {
        procadd(proclist, procscnt, jobcnt, name, id);
        printf("\n[%d] Stopped %d : %s\n", *jobcnt, id, name);
        exitcode = 0;
    }

    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    tcsetpgrp(0, getpgid(getpid()));
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);

    signal(SIGTSTP, sig_handler);
}

void bg(char **args, int argscnt, proc *proclist, int procscnt) {

    exitcode = 0;
    if (argscnt != 2) {
        fprintf(stderr, "Incorrect number of arguments!\n");
        return;
    }

    for (int i = 0; i < procscnt; i++) {
        if (proclist[i].jobid == numval(args[1])) {
            if (kill(proclist[i].pid, SIGCONT))
                perror("Process could not be continued");
            else
                exitcode = 1;
            return;
        }
    }

    fprintf(stderr, "%s jobid does not exist\n", args[1]);
}

void procadd(proc *proclist, int *procscnt, int *jobcnt, char *name, int id) {
    proclist[*procscnt].pname = malloc(sizeof(name));
    strcpy(proclist[*procscnt].pname, name);
    proclist[*procscnt].pid = id;
    proclist[*procscnt].jobid = ++(*jobcnt);
    (*procscnt)++;
}
