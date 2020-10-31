#include "headers.h"
#include "pinfo.h"
#include "func.h"

extern int exitcode;

void pinfo(char **args, int argscnt) {

    exitcode = 0;
    int pid;
    if (argscnt == 1)
        pid = getpid();
    else
        pid = numval(args[1]);

    char procfile[100];
    sprintf(procfile, "/proc/%d/stat", pid);

    FILE *file = fopen(procfile, "r");

    if (file == NULL) {
        perror("Invalid pid");
        return;
    }

    char *buf = NULL;
    char **stats = malloc(arglimit * sizeof(char *));
    size_t bufsz = 0, statscnt = 0;
    getline(&buf, &bufsz, file);
    splitargs(stats, &statscnt, " ", buf);

    printf("Pid -- %d\n", pid);
    printf("Process Status -- %s\n", stats[2]);
    printf("Memory -- %s {Virtual Memory}\n", stats[22]);

    memset(buf, '\0', bufsz);
    sprintf(procfile, "/proc/%d/exe", pid);
    if (readlink(procfile, buf, bufsz) == -1)
        printf("Executable Path NOT found.\n");
    else
    {
        printf("Executable Path -- %s\n", buf);
        exitcode = 1;
    }

    fclose(file);
    free(stats);
}