#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

void tail(FILE* f) {
    int ch;
    fpos_t* pos = (fpos_t*)calloc(100, sizeof(fpos_t*));
    if (pos == NULL) {
        perror("error with calloc");
        exit(1);
    }
    fgetpos(f, &pos[0]);
    int last = 1;
    while ((ch = fgetc(f)) != EOF)
        if (ch == '\n') {
            fgetpos(f, &pos[last++]);
            if (last == 100) {
                for (unsigned i = 0; i < 10; ++i)
                    pos[i] = pos[90 + i];
                last = 10;
            }
        }
    fsetpos(f, &pos[last - 10 >= 0 ? last - 10 : 0]);
    while ((ch = fgetc(f)) != EOF)
        putchar(ch);
    free(pos);
    putchar('\n');
}

int is_dir(const char* path) {
    struct stat buf;
    int s = stat(path, &buf);
    if (s == -1)
        return 0;
    return S_ISDIR(buf.st_mode);
}

int main(int argc, char** argv) {
    if (argc == 1)
        tail(stdin);
    FILE* f;
    for (unsigned i = 1; i < argc; ++i) {
        if (is_dir(argv[i])) {
            printf("%s%s\n", argv[i], ": is a directory");
            continue;
        }
        f = fopen(argv[i], "r");
        if (f == NULL) {
            perror(argv[i]);
            continue;
        }
        if (argc > 2)
            printf("%s%s%s\n", "==> ", argv[i], " <==");
        tail(f);
        fclose(f);
    }
}