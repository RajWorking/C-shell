#include "headers.h"
#include "ls.h"

extern int exitcode;

void printlist(char **dir, int cnt, char *home, char *actdir) {

    unsigned int flags = 0;
    int dircnt = 0;
    for (int i = 1; i < cnt; i++) {
        if (strcmp(dir[i], "-l") == 0)
            flags |= 1U;
        else if (strcmp(dir[i], "-a") == 0)
            flags |= 2U;
        else if (strcmp(dir[i], "-al") == 0)
            flags |= 3U;
        else if (strcmp(dir[i], "-la") == 0)
            flags |= 3U;
        else
            dircnt++;
    }

    if (dircnt == 0)
        printentity(".", flags);
    else {
        char newdir[dirsize];
        for (int i = 1; i < cnt; i++) {

            if (dir[i][0] == '-')
                continue;
            else if (dir[i][0] == '~') {
                strcpy(newdir, actdir);
                strcat(newdir, dir[i] + 1);
            }
            else
                strcpy(newdir, dir[i]);

            if (stat(newdir, &statbuf)) {
                perror("error in stat");
                exitcode = 0;
                continue;
            }

            if (S_ISDIR(statbuf.st_mode)) {
                printf("%s:\n", dir[i]);
                if (strcmp(newdir, "~") == 0)
                    printentity(home, flags);
                else
                    printentity(newdir, flags);
            }
            else
                printf("%s\n\n", newdir);
        }
    }
}

void printentity(char *name, unsigned int flags) {

    struct dirent **file;
    int cnt = scandir(name, &file, NULL, alphasort);
    char curdirname[dirsize];
    getcwd(curdirname, dirsize);

    chdir(name);

    long long memorysum = 0;
    for (int i = 0; i < cnt; i++) {
        if (!(flags & 2UL) && (file[i]->d_name)[0] == '.')
            continue;
        if(stat(file[i]->d_name, &statbuf))
        {
            perror("error in stat");
            exitcode = 0;
            continue;
        }
        memorysum += statbuf.st_size;
    }
    memorysum /= 1024;
    if (flags & 1UL)
        printf("total %lld\n", memorysum);

    for (int i = 0; i < cnt; i++) {
        if (!(flags & 2UL) && (file[i]->d_name)[0] == '.')
            continue;

        if (flags & 1UL) {
//            printf("%lu \t %s \t\t %ld \t %hu \t hi%d\n",file[i]->d_ino, file[i]->d_name, file[i]->d_off, file[i]->d_reclen, file[i]->d_type);

            if (stat(file[i]->d_name, &statbuf))
            {
                perror("error in stat");
                exitcode = 0;
                continue;
            }

            long int mode = statbuf.st_mode;
            char perm[11] = "----------";

            if (S_ISDIR(mode))perm[0] = 'd';
            else if (S_ISBLK(mode))perm[0] = 'b';
            else if (S_ISCHR(mode))perm[0] = 'c';
            else if (S_ISLNK(mode))perm[0] = 'l';
            else if (S_ISSOCK(mode))perm[0] = 's';
            else if (S_ISFIFO(mode))perm[0] = 'p';

            if (mode & S_IRUSR)perm[1] = 'r';
            if (mode & S_IWUSR)perm[2] = 'w';
            if (mode & S_IXUSR)perm[3] = 'x';

            if (mode & S_IRGRP)perm[4] = 'r';
            if (mode & S_IWGRP)perm[5] = 'w';
            if (mode & S_IXGRP)perm[6] = 'x';

            if (mode & S_IROTH)perm[7] = 'r';
            if (mode & S_IWOTH)perm[8] = 'w';
            if (mode & S_IXOTH)perm[9] = 'x';

            struct passwd *username;
            struct group *grpname;
            char mdatetime[36];
            username = getpwuid(statbuf.st_uid);
            grpname = getgrgid(statbuf.st_gid);

            printf("%s \t %lu \t %s \t %s \t %ld \t %s \t %s\n", perm, statbuf.st_nlink, username->pw_name,
                   grpname->gr_name, statbuf.st_size, formatdate(mdatetime, statbuf.st_mtime), file[i]->d_name);
        }
        else {
            printf("%s\n", file[i]->d_name);
        }
    }
    chdir(curdirname);

    printf("\n");
}

char *formatdate(char *str, time_t val) {
    strftime(str, 36, "%b %e %H:%M", localtime(&val));
    return str;
}