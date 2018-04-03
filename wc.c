#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>

void readFile(FILE* f, int* string_number, int* word_number, int* char_number) {
    char* s = calloc(2048, sizeof(char));
    if (!s) {
        perror("calloc error");
        exit(1);
        }
    int ch, last = 0, size = 2048;
    while ((ch = fgetc(f)) != EOF) {
        s[last++] = ch;
        if (last == size) {
            s = realloc(s, (size *= 2) * sizeof(char));
            if (!s) {
                perror("realloc error");
                exit(1);
            }
        }
    }
    char* end = s + last;
    bzero(end, size - last);
    int len;
    wchar_t wc;
    int is_word = 0;
    while ((len = mbtowc(&wc, s, end - s)) > 0) {
        *char_number += len;
        if (iswspace(wc)) {
            if (is_word)
                ++*word_number;
            if (wc == '\n' || wc == L'\n')
                ++*string_number;
            is_word = 0;
        }
        else
            is_word = 1;
        s += len;
    }
    if (is_word)
        ++*word_number;
}

int max(int a, int b) {
    return a >= b ? a : b;
}

char* int_to_string(int n) {
    int len = 1;
    int m = n;
    while (m /= 10)
        ++len;
    char* ans = calloc(len, sizeof(char));
    if (!ans) {
        perror("calloc error");
        exit(1);
    }
    m = n;
    while (m) {
        ans[--len] = ('0' + m % 10);
        m /= 10;
    }
    return ans;
}

void printWC(int* string_number, int* word_number, int* char_number,
             int max_number, char** filenames, int n) {
    int len = 1;
    while (max_number /= 10)
        ++len;
    char* len_str = int_to_string(len);
    const char* pr = "%";
    const char* d = "d";
    const char* sp = " ";
    const char* str = "%s";
    char* format = calloc(0, sizeof(char));
    if (!format) {
        perror("calloc error");
        exit(1);
    }
    strcat(format, pr);
    strcat(format, len_str);
    strcat(format, d);
    strcat(format, sp);
    if (n == 1) {
        printf(format, string_number[1]);
        printf(format, word_number[1]);
        printf(format, char_number[1]);
        puts("\n");
    }
    else {
        for (int i = 1; i < n; ++i) {
            printf(format, string_number[i]);
            printf(format, word_number[i]);
            printf(format, char_number[i]);
            printf("%s\n", filenames[i]);
        }
        if (n > 2) {
            printf(format, string_number[0]);
            printf(format, word_number[0]);
            printf(format, char_number[0]);
            printf("%s\n", filenames[0]);
        }
    }
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "en_US.utf8");
    int *string_number = calloc(2, sizeof(int)), 
        *word_number = calloc(2, sizeof(int)),
        *char_number = calloc(2, sizeof(int));
    char** filenames = calloc(2, sizeof(char*));
    int max_number = -1;
    int size = 2;
    if (!string_number || !word_number || !char_number) {
        perror("calloc error");
        exit(1);
    }
    string_number[0] = word_number[0] = char_number[0] = 0;
    filenames[0] = "total";
    FILE* f;
    if (argc == 1) {
        readFile(stdin, &string_number[1], &word_number[1], &char_number[1]);
        max_number = max(max_number, char_number[1]);
    }
    else
        for (int i = 1; i < argc; ++i)
            if ((f = fopen(argv[i], "r")) == NULL) {
                perror(argv[i]);
                continue;
            }
            else {
                readFile(f, &string_number[i], &word_number[i], &char_number[i]);
                string_number[0] += string_number[i];
                word_number[0] += word_number[i];
                char_number[0] += char_number[i];
                filenames[i] = argv[i];
                max_number = max(max_number, char_number[i]);
                if (i == size) {
                    string_number = realloc(string_number, (size *= 2) * sizeof(int));
                    word_number = realloc(word_number, (size *= 2) * sizeof(int));
                    char_number = realloc(char_number, (size *= 2) * sizeof(int));
                    filenames = realloc(filenames, (size *= 2) * sizeof(int));
                    if (!string_number || !word_number || !char_number) {
                        perror("realloc error");
                        exit(1);
                    }
                }
                fclose(f);
            }
    printWC(string_number, word_number, char_number, max_number, filenames, argc);
    free(string_number);
    free(word_number);
    free(char_number);
    free(filenames);
}