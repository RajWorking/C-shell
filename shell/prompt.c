#include "headers.h"
#include "prompt.h"

extern int exitcode;

char *curdir(char *dirname, char *actdir) {
    if (strncmp(dirname, actdir, strlen(actdir)) != 0)
        return dirname;

    if (strlen(actdir) == strlen(dirname))
        return "~";

    char *str = (char *) malloc(dirsize);
    memset(str, '\0', dirsize);
    str[0] = '~';
    return strcat(str, dirname + strlen(actdir));
}

int prompt(char *dirname, char *actdir, char **buf, size_t *bufsize) {

//    printf("%d", exitcode);
    if (exitcode == 1)
        printf("\e[1;30m:')\e[0m");
    else if (exitcode == 0)
        printf("\e[1;31m:'(\e[0m");

    char *loginname, hostname[hostsize];
    memset(dirname, '\0', dirsize);
    memset(hostname, '\0', hostsize);

    loginname = getlogin();
    getcwd(dirname, dirsize);
    gethostname(hostname, hostsize);
    printf("<\e[0;32m%s@%s\e[0m:\e[0;34m%s\e[0m>", loginname, hostname, curdir(dirname, actdir));
    exitcode = 1;

    *buf = NULL;
    *bufsize = 0;

    if (getline(buf, bufsize, stdin) == -1)
        return -1;

    (*buf)[strlen(*buf) - 1] = '\0';
    return 0;
}