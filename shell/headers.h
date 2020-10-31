#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <setjmp.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

struct stat statbuf;
#define arglimit 100    // assuming max 100 arguments
#define procmax 100     // max number of processes at any time
#define hostsize 500
#define dirsize 500
