#include "headers.h"
#include "func.h"
#include "proc.h"

extern int exitcode;

void splitargs(char **args, size_t *argscnt, char *delim, char *cmd) {
    *argscnt = 0;
    args[0] = strtok(cmd, delim);
    while (args[*argscnt] != NULL) {
        (*argscnt)++;
        args[*argscnt] = strtok(NULL, delim);
    }
}

void io_restore(int std_out, int std_in) {
    dup2(std_out, STDOUT_FILENO);
    dup2(std_in, STDIN_FILENO);
}

int io_redirect(char **args, size_t *argscnt) {
    int cnt = *argscnt, fd;
    exitcode = 0;

    for (int i = 0; i < cnt;) {
        if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            if (i + 1 >= cnt) {
                fprintf(stderr, "Incorrect syntax- please enter valid input\n");
                return 1;
            }

            if (strcmp(args[i], "<") == 0)
                fd = open(args[i + 1], O_RDONLY, 0644);
            else if (strcmp(args[i], ">") == 0)
                fd = open(args[i + 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
            else
                fd = open(args[i + 1], O_WRONLY | O_APPEND | O_CREAT, 0644);

            if (fd < 0) {
                perror("Incorrect Filename");
                return 1;
            }

            if (strcmp(args[i], "<") == 0) { if (dup2(fd, STDIN_FILENO) == -1) perror("(redirect) Error with dup2"); }
            else { if (dup2(fd, STDOUT_FILENO) == -1) perror("(redirect) Error with dup2"); }
            close(fd);

            for (int j = i + 2; j < cnt; j++)
                args[j - 2] = args[j];
            cnt -= 2;
        }
        else
            i++;
    }

    for (int i = *argscnt; i >= cnt; i--)
        args[i] = NULL;

    *argscnt = cnt;
    exitcode = 1;
    return 0;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int numval(char *str) {
    int i = (int) strlen(str) - 1;
    int baseten = 1, sum = 0;
    while (i >= 0) {
        sum += (str[i] - '0') * baseten;
        baseten *= 10;
        i--;
    }
    return sum;
}

void quit(int procscnt, int std_out, int std_in) {
    if (procscnt != 0)
        printf("\n\e[0;33mWARNING:\e[0m There are stopped jobs");

    close(std_out);
    close(std_in);
    printf("\n\e[1;35mExiting shell... \e[1;33mBYE!\e[0m\n");
}

int syntaxchk(char *cmd) {
    char *s = strrchr(cmd, '|');
    int f = 1;

    if (s != NULL) {
        for (int i = 1; i < strlen(s); i++) {
            if (s[i] != ' ') {
                f = 0;
                break;
            }
        }
    }
    else
        f = 0;

    return f;
}
