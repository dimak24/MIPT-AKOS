#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

struct name_and_stat {
    struct stat st;
    char* name;
};

int cmp_by_size(const void* a, const void* b) {
    return ((struct name_and_stat*)a)->st.st_size <= 
        ((struct name_and_stat*)b)->st.st_size;
}

int max(int a, int b) {
    return a >= b ? a : b;
}

int get_number_lenght(int n) {
    int ans = 1;
    while (n /= 10)
        ++ans;
    return ans;
}

int len(int n) {
    int ans = 1;
    while(n /= 10)
        ++ans;
    return ans;
}

void print_dirs_info(struct name_and_stat* stats, int size) {
    int max_link_number = 0, max_user = 0, max_group = 0, max_size = 0;
    for (int i = 0; i < size; ++i) {
        struct passwd *user = getpwuid(stats[i].st.st_uid);
        struct group *group = getgrgid(stats[i].st.st_gid);
        max_link_number = max(max_link_number, stats[i].st.st_nlink);
        max_user = max(max_user, user ? strlen(user->pw_name) : 0);
        max_group = max(max_group, group ? strlen(group->gr_name) : 0);
        max_size = max(max_size, stats[i].st.st_size);
    }
    max_link_number = get_number_lenght(max_link_number);
    max_size = get_number_lenght(max_size);
    int total_calloc_size = len(max_link_number) + len(max_user) + len(max_group) +
                            len(max_size) + 100;
    char* format = calloc(total_calloc_size, sizeof(char));
    if (!format) {
        perror("calloc error");
        exit(1);
    }
    sprintf(format, "%s%d%s%d%s%d%s%d%s", "%s %", max_link_number, "d %", max_user,
            "s %", max_group, "s %", max_size, "lld %s %s\n");
    for (int i = 0; i < size; ++i) {
        char* type = calloc(10, sizeof(char));
        if (!type) {
            perror("calloc error");
            exit(1);
        }
        switch (stats[i].st.st_mode & S_IFMT) {
            case S_IFDIR: 
                type[0] = 'd';
                break;
            case S_IFCHR:
                type[0] = 'c';
                break;
            case S_IFBLK:
                type[0] = 'b';
                break;
            case S_IFLNK:
                type[0] = 'l';
                break;
            case S_IFREG:
                type[0] = '-';
                break;
            case S_IFSOCK:
                type[0] = 's';
                break;
        }
        type[1] = stats[i].st.st_mode & S_IRUSR ? 'r' : '-';
        type[2] = stats[i].st.st_mode & S_IWUSR ? 'w' : '-';
        type[3] = stats[i].st.st_mode & S_IXUSR ? 'x' : '-';
        type[4] = stats[i].st.st_mode & S_IRGRP ? 'r' : '-';
        type[5] = stats[i].st.st_mode & S_IWGRP ? 'w' : '-';
        type[6] = stats[i].st.st_mode & S_IXGRP ? 'x' : '-';
        type[7] = stats[i].st.st_mode & S_IROTH ? 'r' : '-';
        type[8] = stats[i].st.st_mode & S_IWOTH ? 'w' : '-';
        type[9] = stats[i].st.st_mode & S_IXOTH ? 'x' : '-';
        struct passwd *user = getpwuid(stats[i].st.st_uid);
        struct group *group = getgrgid(stats[i].st.st_gid);
        char* access_time = ctime(&stats[i].st.st_atime);
        access_time[strlen(access_time) - 1] = '\0';
        printf(format, type, stats[i].st.st_nlink, user ? user->pw_name : "",
            group ? group->gr_name : "", (long long)stats[i].st.st_size,
            access_time, stats[i].name);
    }
    free(format);
}

void process_directory(char* path) {
    DIR* d = opendir(path);
    FILE* f = fopen(path, "r");
    if (!d && !f) {
        perror(path);
        return;
    }
    struct name_and_stat* stats = calloc(1, sizeof(struct name_and_stat));
    int last = 0, size = 1, last1 = 0, size1 = 1;
    if (!stats) {
        perror("calloc error");
        exit(1);
    }
    if (!d) {
        struct stat dir_stat;
        if (lstat(path, &dir_stat) == -1) {
            perror("stat error");
            exit(1);
        }
        stats[0].st = dir_stat;
        stats[0].name = path;
        ++last;
    }
    else
        printf("%s%s\n", path, ":");
    char** sub_dirs = calloc(1, sizeof(char*));
    if (!sub_dirs) {
        perror("calloc error");
        exit(1);
    }
    struct dirent* dir;
    long long total = 0;
    while (d && (dir = readdir(d))) {
        struct stat dir_stat;
        char* d_path = calloc(strlen(path) + strlen(dir->d_name) + 5, sizeof(char));
        if (!d_path) {
            perror("calloc error");
            exit(1);
        }
        sprintf(d_path, "%s/%s", path, dir->d_name);
        if (lstat(d_path, &dir_stat) == -1) {
            perror("stat error");
            exit(1);
        }
        stats[last++].st = dir_stat;
        stats[last - 1].name = dir->d_name;
        if (last == size) {
            stats = realloc(stats, (size *= 2) * sizeof(struct name_and_stat));
            if (!stats) {
                perror("realloc error");
                exit(1);
            }
        }
        total += dir_stat.st_blocks;
        if (dir->d_type == DT_DIR && 
            strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
            sub_dirs[last1++] = d_path;
            if (last1 == size1) {
                sub_dirs = realloc(sub_dirs, (size1 *= 2) * sizeof(char*));
                if (!sub_dirs) {
                    perror("realloc error");
                    exit(1);
                }
            }
        }
    }
    if (d)
        printf("%s%lld\n", "total ", total / 2);
    qsort(stats, last, sizeof(struct name_and_stat), cmp_by_size);
    print_dirs_info(stats, last);
    printf("%c", '\n');
    for (int i = 0; i < last1; ++i)
        process_directory(sub_dirs[i]);
    fclose(f);
    closedir(d);
    free(sub_dirs);
    free(stats);
}

int main(int argc, char** argv) {
    if (argc == 1)
        process_directory(".");
    else
        for (int i = 1; i < argc; ++i)
            process_directory(argv[i]);
}