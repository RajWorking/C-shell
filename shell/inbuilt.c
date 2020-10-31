#include "headers.h"
#include "inbuilt.h"

extern int exitcode;

void echo(char **args, int argscnt) {
    for (int i = 1; i < argscnt; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    exitcode = 1;
}

void pwd(char *dirname) {
    printf("%s\n", dirname);
    exitcode = 1;
}

void cd(char **args, int argscnt, char *actdir, char *olddir) {

    exitcode = 0;
    char curdir[dirsize], newdir[dirsize];
    getcwd(curdir, dirsize);

    if (argscnt == 1) {
        if (chdir(actdir) == -1) {
            perror("Error");
            return;
        }
        else
            strcpy(olddir, curdir);
    }
    else if (argscnt == 2) {
        if (args[1][0] == '~') {
            strcpy(newdir, actdir);
            strcat(newdir, args[1] + 1);
        }
        else if (strcmp(args[1], "-") == 0) {
            if (olddir[0] == '\0') {
                fprintf(stderr, "OLDPWD not set!\n");
                return;
            }
            else
                strcpy(newdir, olddir);
        }
        else
            strcpy(newdir, args[1]);

        if (chdir(newdir) == -1) {
            perror("Error in chdir");
            return;
        }
        else
            strcpy(olddir, curdir);
    }
    else {
        printf("Too many arguments!\n");
        return;
    }

    exitcode = 1;
}

void envset(char **args, int argscnt) {

    exitcode = 0;
    int st = 0;
    if (argscnt == 1 || argscnt > 3) {
        fprintf(stderr, "Incorrect arguments!\n");
        return;
    }
    else if (argscnt == 3)
        st = setenv(args[1], args[2], 1);
    else if (argscnt == 2)
        st = setenv(args[1], "", 1);

    if (st == -1) {
        perror("Error in setting environment variable");
        return;
    }

    exitcode = 1;
}

void envunset(char **args, int argscnt) {
    exitcode = 0;
    if (argscnt != 2) {
        fprintf(stderr, "Incorrect arguments!\n");
        return;
    }
    else if (getenv(args[1]) == NULL) {
        fprintf(stderr, "This env variable does not exist!\n");
        return;
    }
    else if (unsetenv(args[1]) == -1) {
        perror("Error in unsetting environment variable");
        return;
    }
    exitcode = 1;
}

