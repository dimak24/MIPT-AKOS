#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int cmp(const void *a, const void *b) {
    const char** ia = (const char**)a;
    const char** ib = (const char**)b;
    return strcmp(*ia, *ib);
}

void fillWithZeros(char* s, const int cur_size, const int real_size) {
    for (unsigned i = real_size; i < cur_size; ++i)
        s[i] = 0;
}

int main() {
    int ch, last = 0;
    char** s;
    int str_number = 1;
    int cur_size = 2;
    int last_word = 0;
    int last_pos = 0;
    int spaced = 0;
    char open = 0;

    s = calloc(str_number, sizeof(char*));
    s[0] = calloc(cur_size, sizeof(char));

    while ((ch = getchar()) != EOF) {
        if (!open && (ch == '\'' || ch == '\"'))
            open = ch;
        else if (open == ch)
            open = 0;
        else if (!open && isspace(ch)) {
            if (last_pos) {
                fillWithZeros(s[last_word], cur_size, last_pos);
                ++last_word;
                cur_size = 2;
                last_pos = 0;
                if (last_word >= str_number)
                    s = realloc(s, (str_number *= 2) * sizeof(char*));
                s[last_word] = calloc(cur_size, sizeof(char));
            }
        }
        else if (!open && (ch == '&' || ch == '|')) {
            if (ch == last && s[last_word - 1][0] == ch && !s[last_word - 1][1])
                s[last_word - 1][1] = ch;
            else if (!last_pos) {
                s[last_word][0] = ch;
                ++last_word;
                cur_size = 2;
                last_pos = 0;
                if (last_word >= str_number)
                    s = realloc(s, (str_number *= 2) * sizeof(char*));
                s[last_word] = calloc(cur_size, sizeof(char));
            }
            else {
                fillWithZeros(s[last_word], cur_size, last_pos);
                last_word += 2;
                cur_size = 2;
                last_pos = 0;
                if (last_word >= str_number)
                    s = realloc(s, (str_number *= 2) * sizeof(char*));
                s[last_word] = calloc(cur_size, sizeof(char));
                s[last_word - 1] = calloc(cur_size, sizeof(char));
                s[last_word - 1][0] = ch;
            }
        }
        else if (!open && ch == ';') {
            if (!last_pos) {
                s[last_word][0] = ch;
                ++last_word;
                cur_size = 2;
                last_pos = 0;
                if (last_word >= str_number)
                    s = realloc(s, (str_number *= 2) * sizeof(char*));
                s[last_word] = calloc(cur_size, sizeof(char));
            }
            else {
                fillWithZeros(s[last_word], cur_size, last_pos);
                last_word += 2;
                cur_size = 2;
                last_pos = 0;
                if (last_word >= str_number)
                    s = realloc(s, (str_number *= 2) * sizeof(char*));
                s[last_word] = calloc(cur_size, sizeof(char));
                s[last_word - 1] = calloc(cur_size, sizeof(char));
                s[last_word - 1][0] = ch;
            }
        }
        else {
            s[last_word][last_pos++] = ch;
            if (last_pos == cur_size)
                s[last_word] = realloc(s[last_word], (cur_size *= 2) * sizeof(char));
        }
        last = ch;
    }

    fillWithZeros(s[last_word], cur_size, last_pos);

    if (open)
        puts("Error: unclosed quote!");
    else {
        qsort(s, last_word + (last_pos > 0), sizeof(char*), cmp);

        for (size_t i = 0; i < last_word + (last_pos > 0); ++i)
            printf("%c%s%c%c", '"', s[i], '"', '\n');
    }

    return 0;
}