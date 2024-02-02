#include "hook_fixes.h"
#include "constants.h"
#include "syscalls.h"
#include "hook_funcs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* fixedPath(char* path) {
        char* result = (char*) malloc(MAX_PATH_LEN);
        strcpy(result, path);
        int k = strlen(result);
        result[k] = '.';
        result[k + 1] = 'c';
        result[k + 2] = '\0';

        return result;
}

HOOK_FIX_RESULT eof_blank_space_fix(char *path) {
        char* fixed_path = fixedPath(path);
        int len = 0, k = 0;
        FILE *fp = fopen(path, "r");
        if (fp == NULL) {
                return FIX_FAILED;
        }

        char ch;
        while ((ch = fgetc(fp)) != EOF) {
                if (ch == ' ' || ch == '\n') k++;
                else k = 0;
                len++;
        }

        fclose(fp);
        fp = fopen(path, "r");
        FILE* fp2 = fopen(fixed_path, "w");
        if (fp == NULL || fp2 == NULL) {
                return FIX_FAILED;
        }

        len -= k;
        while ((ch = fgetc(fp)) != EOF && len--) {
                fputc(ch, fp2);
        }

        fclose(fp);
        fclose(fp2);

        if (eof_blank_space(fixed_path) != HOOK_PASSED) {
                removeFileDir(fixed_path);
                return FIX_SUCCEEDED;
        }

        removeFileDir(path);
        copyDirWithName(fixed_path, path);
        removeFileDir(fixed_path);
        return FIX_SUCCEEDED;
}

HOOK_FIX_RESULT balance_braces_fix(char *path) {
        char* fixed_path = fixedPath(path);
        FILE* fp = fopen(path, "r");
        FILE* fp2 = fopen(fixed_path, "w");
        
        if (fp == NULL || fp2 == NULL) {
                return FIX_FAILED;
        }

        char ch;
        int st[4096];
        int st_size = 0;
        while ((ch = fgetc(fp)) != EOF) {
                if (ch == '{' || ch == '(' || ch == '[') {
                        st[st_size++] = ch;
                        fputc(ch, fp2);
                } else if (ch == ')' || ch == '}' || ch == ']') {
                        if (st_size > 0) {
                                fputc(brance_not(st[st_size - 1]), fp2);
                                st_size--;
                        }
                } else putc(ch, fp2);
        }

        while (st_size > 0) {
                fputc(brance_not(st[st_size - 1]), fp2);
                st_size--;
        }

        if (eof_blank_space(fixed_path) != HOOK_PASSED) {
                removeFileDir(fixed_path);
                return FIX_SUCCEEDED;
        }

        fclose(fp);
        fclose(fp2);
        removeFileDir(path);
        copyDirWithName(fixed_path, path);
        removeFileDir(fixed_path);
        return FIX_SUCCEEDED;
}
