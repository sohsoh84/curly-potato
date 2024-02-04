#include "hook_funcs.h"
#include "hook.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "paths.h"
#include "syscalls.h"
#include "libgen.h"

const int CHAR_LIM = 20000;
const int FILE_SIZE_LIMIT = 5 * 1024 * 1024;

int checkExtension(const char* filename, int count, ...) {
        va_list extensions;
        va_start(extensions, count);
        const char* extension;
        int match = 0;

        char* file_extension = strrchr(filename, '.');
        if (file_extension == NULL) {
                return 0;
        }

        file_extension++;
        while (count--) {
                extension = va_arg(extensions, const char*);
                if (strcmp(file_extension, extension) == 0) {
                        return 1;
                }
        }

        va_end(extensions);

        return match;
}


HOOK_RESULT eof_blank_space(char *path) {
        if (!checkExtension(path, 3, "cpp", "c", "txt")) return HOOK_SKIPPED;
        
        FILE* fp = fopen(path, "r");
        char ch;
        HOOK_RESULT result = HOOK_PASSED;
        while ((ch = fgetc(fp)) != EOF) {
                if (ch == ' ') result = HOOK_FAILED;
                else if (ch != '\n') result = HOOK_PASSED;
        }

        fclose(fp);
        return result;
}

char brance_not(char c) {
        if (c == ')') return '(';
        if (c == ']') return '[';
        if (c == '}') return '{';
        if (c == '(') return ')';
        if (c == '{') return '}';
        if (c == '[') return ']';
        return '(';
}

HOOK_RESULT balance_braces(char* path) {
        if (!checkExtension(path, 3, "cpp", "c", "txt")) return HOOK_SKIPPED;
        
        FILE* fp = fopen(path, "r");
        char ch;
        char br_stack[4096];
        int stack_ptr = 0;

        while ((ch = fgetc(fp))!= EOF) {
                if (ch == '{' || ch == '[' || ch == '(')
                        br_stack[stack_ptr++] = ch;
                else if (ch == '}' || ch == ']' || ch == ')') {
                        if (!stack_ptr || br_stack[stack_ptr - 1] != brance_not(ch)) {
                                fclose(fp);
                                return HOOK_FAILED;
                        }

                        stack_ptr--;
                }
        }
 
        fclose(fp);
        return stack_ptr == 0 ? HOOK_PASSED : HOOK_FAILED;
}

HOOK_RESULT character_limit(char* path) {
        if (!checkExtension(path, 3, "cpp", "c", "txt")) return HOOK_SKIPPED;
        
        FILE* fp = fopen(path, "r");
        char ch;
        int count = 0;
        while ((ch = fgetc(fp))!= EOF) {
                if (ch == '\n') count = 0;
                else count++;
                if (count > CHAR_LIM) {
                        fclose(fp);
                        return HOOK_FAILED;
                }
        }

        fclose(fp);

        return HOOK_PASSED;
}

HOOK_RESULT file_size_check(char* path) {
        struct stat st;
        stat(path, &st);
        int size = st.st_size;
        return size > FILE_SIZE_LIMIT ? HOOK_FAILED : HOOK_PASSED;
}

HOOK_RESULT todo_check(char *path) {
        if (!checkExtension(path, 3, "cpp", "c", "txt")) return HOOK_SKIPPED;
        FILE* fp = fopen(path, "r");
        if (checkSubstringInFile(fp, "TODO")) return HOOK_FAILED;
        fclose(fp);
        return HOOK_PASSED;
}

void remove_doto(char* path_) {
        char* path = strdup(path_);
        char* name = basename(path);
        int lst_dot = strrchr(name, '.') - name;
        name[lst_dot + 1] = 'o';
        name[lst_dot + 2] = '\0';

        if (fileExists(name))
                removeFileDir(name);
}

HOOK_RESULT static_error_check(char *path) {
        if (!checkExtension(path, 2, "cpp", "c")) return HOOK_SKIPPED;
        char command[4096];
        sprintf(command, "%s -w -c %s 2>tmp_res_file > /dev/null", checkExtension(path, 1, "cpp") ? "g++" : "gcc", path);
        system(command);
        if (!fileExists("tmp_res_file")) return HOOK_PASSED;
        FILE* fp = fopen("tmp_res_file", "r");
        if (checkSubstringInFile(fp, "error")) {
                fclose(fp);
                removeFileDir("tmp_res_file");
                remove_doto(path);
                return HOOK_FAILED;
        }

        fclose(fp);
        removeFileDir("tmp_res_file");
        remove_doto(path);
        return HOOK_PASSED;
}

HOOK_RESULT time_limit(char* path) {
        if (!checkExtension(path, 3, "wav", "mp4", "mp3")) return HOOK_SKIPPED;
        int duration = getVideoDuration(path);
        if (duration > 10 * 60) return HOOK_FAILED;
        return HOOK_PASSED;
}

HOOK_RESULT indentation_check(char* path) {
        if (!checkExtension(path, 2, "cpp", "c")) return HOOK_SKIPPED;
        int req_t = 0;

        FILE* file = fopen(path, "r");
        char line[4096];
        int style = 0; // 0: nothing, 1: sohsoh, 2: mellat
        while (fgets(line, sizeof(line), file) != NULL) {
                strip(line);
                if (!strlen(line)) continue;
                int open_cnt = 0, close_cnt = 0, non_tab = 0, tabs = 0;
                for (int i = 0; i < strlen(line); i++) {
                        if (line[i] == '{') open_cnt++;
                        if (line[i] == '}') close_cnt++;
                        if (line[i] == '\t') {
                                if (!non_tab) 
                                        tabs++;
                        } else non_tab = 1;
                }


                if (tabs != req_t - close_cnt) return HOOK_FAILED;
                if (open_cnt + close_cnt > 1) return HOOK_FAILED;
                if (open_cnt + close_cnt == 0) continue;

                if (close_cnt) {
                        if (strlen(line) != tabs + 1 || line[strlen(line) - 1] != '}') return HOOK_FAILED;
                        req_t--;
                        continue;
                }

                if (line[strlen(line) - 1] != '{') return HOOK_FAILED;

                if (strlen(line) == tabs + 1) {
                        if (style == 0 || style == 2) style = 2;
                        else return HOOK_FAILED;
                } else {
                        if (style == 0 || style == 1) style = 1;
                        else return HOOK_FAILED;
                }

                req_t++;
        }

        return req_t > 0 ? HOOK_FAILED : HOOK_PASSED;
}

HOOK_RESULT format_check(char* path) {
        if (strchr(BaseName(path), '.') == NULL) return HOOK_SKIPPED;
        return (checkExtension(path, 6, "txt", "c", "cpp", "wav", "mp4", "mp3") || strchr(path, '.') == NULL) ? HOOK_PASSED : HOOK_FAILED;
}