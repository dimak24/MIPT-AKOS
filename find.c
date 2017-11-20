#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void find(char* path, char* name, char* user) {
    DIR* d = opendir(path);
    if (!d) {
        perror(path);
        exit(1);
    }
    struct dirent* dir;
    while (dir = readdir(d)) {
        char* d_path = calloc(strlen(path) + strlen(dir->d_name) + 5, sizeof(char));
        if (!d_path) {
            perror("calloc error");
            exit(1);
        }
        sprintf(d_path, "%s/%s", path, dir->d_name);
        struct stat d_stat;
        if (lstat(d_path, &d_stat) == -1) {
            perror("stat error");
            exit(1);
        }
        struct passwd *user_ = getpwuid(d_stat.st_uid);
        if (!strcmp(dir->d_name, name) && !strcmp(user_->pw_name, user))
            printf("%s\n", d_path);
        if (dir->d_type == DT_DIR &&
            (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")))
            find(d_path, name, user);
    }
    closedir(d);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("%s\n", "wrong format");
        exit(1);
    }
    find(argv[1], argv[2], argv[3]);
}