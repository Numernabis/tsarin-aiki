/*
  author: Ludi
  file:   main.c
  start:  18.03.2018
  end:    []
  lines:  213
*/
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>
#include <linux/limits.h>

/* -------------------------------------------------------------------------- */
int equal(time_t a, time_t b) {
    return fabs(difftime(a, b)) < 0.00001 ? 1 : 0;
}
int before(time_t a, time_t b) {
    return difftime(a, b) > 0.00001 ? 1 : 0;
}
int after(time_t a, time_t b) {
    return difftime(b, a) > 0.00001 ? 1 : 0;
}

/* -------------------------------------------------------------------------- */
time_t parse_date(char* part1, char* part2) {
    char* date_str = malloc(20);
    struct tm date;

    // Parse date from argv[3] (YYYY-mm-dd) and argv[4] (HH:MM:SS)
    strcpy(date_str, part1);
    strcat(date_str, " ");
    strcat(date_str, part2);
    strptime(date_str, "%Y-%m-%d %H:%M:%S", &date);

    free(date_str);
    return mktime(&date);
}

char* get_permissions(int mode) {
    char* perm = malloc(11);
    perm = strcpy(perm, "----------\0");

    // user
    if (mode & S_IRUSR) perm[1] = 'r';
    if (mode & S_IWUSR) perm[2] = 'w';
    if (mode & S_IXUSR) perm[3] = 'x';

    // group
    if (mode & S_IRGRP) perm[4] = 'r';
    if (mode & S_IWGRP) perm[5] = 'w';
    if (mode & S_IXGRP) perm[6] = 'x';

    // others
    if (mode & S_IROTH) perm[7] = 'r';
    if (mode & S_IWOTH) perm[8] = 'w';
    if (mode & S_IXOTH) perm[9] = 'x';

    return perm;
}

void print_header() {
    printf("absolute path               | size [B] | permissions | modification date\n");
    printf("------------------------------------------------------------------------\n");
}

/* -------------------------------------------------------------------------- */
void find_files_sys(char* path, int (*compare)(time_t, time_t), time_t date) {

    DIR* dir = opendir(path);
    char abs_path[PATH_MAX + 1];
    if (dir == 0) {
        printf("Invalid directory: %s\n", path);
        return;
    }
    if(strlen(path) > PATH_MAX) {
        printf("Path too long: %s\n", path);
        return;
    }

    struct dirent *entry;
    struct stat *info = malloc(sizeof(struct stat));

    while ((entry = readdir(dir)) != 0) {
        strcpy(abs_path, path);
        strcat(abs_path, "/");
        strcat(abs_path, entry->d_name);

        if (strcmp(entry->d_name, "..")  == 0 || strcmp(entry->d_name, ".") == 0) continue;

        stat(abs_path, info);

        if (S_ISDIR(info->st_mode)) {
            find_files_sys(abs_path, compare, date);
        } else if (S_ISREG(info->st_mode)) {
            char* perm = get_permissions(info->st_mode);
            if (compare(date, info->st_mtime)) {
                printf("%s\t%ld\t%s\t%s", abs_path, info->st_size, perm, ctime(&info->st_mtime));
            }
            free(perm);
        } else {
            printf("Other entry type: %i\n", entry->d_type);
        }
    }
    free(info);
    closedir(dir);
}

/* -------------------------------------------------------------------------- */
int (*COMPARE)(time_t, time_t);
time_t DATE;

int fn(const char *path, const struct stat *info, int tflag, struct FTW *ftwbuf) {
    char *perm = get_permissions(info->st_mode);
    char abs_path[PATH_MAX + 1];
    realpath(path, abs_path);

    if ((tflag == FTW_F) && COMPARE(DATE, info->st_mtime)) {
        printf("%s\t%ld\t%s\t%s", abs_path, info->st_size, perm, ctime(&info->st_mtime));
    }
    free(perm);
    return 0;
}

void find_files_nftw(char *path, int (*compare)(time_t, time_t), time_t date) {
    COMPARE = compare;
    DATE = date;
    int nopenfd = 100;
    int flags = 0;
    nftw(path, &fn, nopenfd, flags);
}

/* -------------------------------------------------------------------------- */
int main(int argc, char **argv) {
    /*
    proper arguments:
        argv[0] - ./program
        argv[1] - path
        argv[2] - "<" / ">" / "="
        argv[3] - date (YYYY-mm-dd)
        argv[4] - date (HH:MM:SS)
        argv[5] - nftw / sys
    */
    if (argc != 6) {
        // -1, because of argv[0] is program call
        printf("5 arguments you should enter. Proper arguments:\n");
        printf("./program  path  </>/=  date_(YYYY-mm-dd)  date_(HH:MM:SS)  nftw/sys\n");
        return 2;
    }

    // variables
    char *start_path;
    DIR* start_dir;
    time_t date;
    char *sign = "";
    char *option = "";

    // load arguments
    start_path = argv[1];
    sign = argv[2];
    date = parse_date(argv[3], argv[4]);
    option = argv[5];

    if (strcmp(sign, "<") != 0 && strcmp(sign, ">") != 0 && strcmp(sign, "=") != 0) {
        printf("Unsupported sign! [available: <, >, =]\n");
        return 2;
    }
    if (strcmp(option, "sys") != 0 && strcmp(option, "nftw") != 0) {
        printf("Unsupported option! [available: sys, nftw]\n");
        return 2;
    }
    // so far, arguments are validated :)

    /* ---------------------------------------------------------------------- */

    if ((start_dir = opendir(start_path)) == 0) {
        printf("Unable to open given directory.\n");
        return 2;
    }
    closedir(start_dir);

    print_header();

    if (strcmp(option, "sys") == 0) {
        if (strcmp(sign, "<") == 0) {
            find_files_sys(start_path, &before, date);
        } else if (strcmp(sign, ">") == 0) {
            find_files_sys(start_path, &after, date);
        } else {
            find_files_sys(start_path, &equal, date);
        }
    } else { //nftw
        if (strcmp(sign, "<") == 0) {
            find_files_nftw(start_path, &before, date);
        } else if (strcmp(sign, ">") == 0) {
            find_files_nftw(start_path, &after, date);
        } else {
            find_files_nftw(start_path, &equal, date);
        }
    }

    /* ---------------------------------------------------------------------- */
    return 0;
}
