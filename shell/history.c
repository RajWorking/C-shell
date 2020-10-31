#include "headers.h"
#include "history.h"
#include "func.h"

extern int exitcode;

void printhistory(char **args, int argscnt, char *histfile) {

    exitcode = 0;

    FILE *file = fopen(histfile, "r");

    if(file == NULL)
    {
        perror("Error with history record");
        return;
    }

    char *buf = NULL;
    char *histargs[25];
    size_t histcnt = 0, bufsz = 0;
    getdelim(&buf, &bufsz, '\0', file);
    splitargs(histargs, &histcnt, "\n", buf);
    fclose(file);

    if (argscnt > 2) {
        printf("Too many arguments!\n");
        return;
    }
    else {
        int prcnt = min(10, histcnt);
        if (argscnt == 2)
            prcnt = min(histcnt, numval(args[1]));

        for (int i = (int) histcnt - prcnt; i < histcnt; i++) {
            printf("%s\n", histargs[i]);
        }
    }

    free(buf);
    exitcode = 1;
}

void addhistory(char *inp, char *histfile) {
    FILE *file = fopen(histfile, "r");

    if(file == NULL)
    {
        perror("Error with history record");
        return;
    }

    char *buf = NULL;
    char *histargs[25];
    size_t histcnt = 0, bufsz = 0;
    getdelim(&buf, &bufsz, '\0', file);
    splitargs(histargs, &histcnt, "\n", buf);
    fclose(file);

    if ((histcnt == 0 || strcmp(inp, histargs[histcnt - 1]) != 0) && strlen(inp) != 0) {
        file = fopen(histfile, "a");
        fwrite(inp, 1, strlen(inp), file);
        fwrite("\n", 1, strlen("\n"), file);
        fclose(file);
        histargs[histcnt] = inp;
        histcnt++;
    }

    if (histcnt > 20) {
        file = fopen(histfile, "w");
        for (int i = (int) histcnt - 20, j = 0; i < histcnt; i++, j++) {
            fwrite(histargs[i], 1, strlen(histargs[i]), file);
            fwrite("\n", 1, strlen("\n"), file);
        }
        fclose(file);
    }
}
