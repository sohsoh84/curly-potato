#include "grep.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../commits.h"
#include "../vcs.h"
#include "../utils.h"

#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>

void printGreen(char* word) {
        printf(GREEN "%s" RESET, word);
}

int grepLine(char* line_, char* word) {
        char* line = strdup(line_);
        char* ptr = strtok(line, " \t\n");
        while (ptr) {
                if (match_wc(word, ptr, 0, 0)) return 1;
                ptr = strtok(NULL, " \t\n");
        }

        return 0;
}

void grepLineFunc(char *line_, char *word, void (*command_func)(char *), char* pre_print) {
        char* line = strdup(line_);
        if (!grepLine(line_, word)) return;

        printf("%s", pre_print);
        char* ptr = strtok(line, " \t\n");
        while (ptr) {
                if (match_wc(word, ptr, 0, 0)) command_func(ptr);
                else printf("%s", ptr);

                char* tmp_ptr = ptr + strlen(ptr);
                ptr = strtok(NULL, " \t\n");

                while (line_[tmp_ptr - line] != '\0' && tmp_ptr != ptr) {
                        printf("%c", line_[tmp_ptr - line]);
                        tmp_ptr++;
                }
        }
}

int grepCommand(int argc, char* argv[]) {
        if (!dotCupotPath(cwdPath())) {
                printf("you should be inside a cupot repository\n");
                return 1;
        }

        const char USAGE[] = "cupot grep -f <file> -p <word> [-c <commit-id>] [-n]";
        char* file_path = NULL;
        char* word = NULL;
        int print_line_number = 0;
        char* commit_id = NULL;

        int arg_ptr = 0;
        while (arg_ptr < argc) {
                if (!strcmp(argv[arg_ptr], "-n")) {
                        print_line_number = 1;
                        arg_ptr++;
                } else if (!strcmp(argv[arg_ptr], "-f")) {
                        if (arg_ptr == argc - 1) {
                                printf("%s\n", USAGE);
                                return 1;
                        }

                        file_path = argv[arg_ptr + 1];
                        arg_ptr += 2;
                } else if (!strcmp(argv[arg_ptr], "-p")) {
                        if (arg_ptr == argc - 1) {
                                printf("%s\n", USAGE);
                                return 1;
                        }

                        word = argv[arg_ptr + 1];
                        arg_ptr += 2;
                } else if (!strcmp(argv[arg_ptr], "-c")) {
                        if (arg_ptr == argc - 1) {
                                printf("%s\n", USAGE);
                                return 1;
                        }

                        commit_id = argv[arg_ptr + 1];
                        arg_ptr += 2;
                } else {
                        printf("%s\n", USAGE);
                        return 1;
                }
        }        

        if (file_path == NULL || word == NULL) {
                printf("%s\n", USAGE);
                return 1;
        }

        if (commit_id != NULL) {
                buildProjectFromCommit(mergePaths(dotCupotPath(cwdPath()), TEMP_LATEST_VERSION), commit_id);
                file_path = latestVersionPath(file_path, TEMP_LATEST_VERSION);
        }

        FILE* fp = fopen(file_path, "r");
        if (fp == NULL) {
                fprintf(stderr, "Couldn't open the file\n");
                return 1;
        }

        char line[4096];
        int lc = 0;
        while (fgets(line, 4096, fp) !=  NULL) {
                lc++;
                char res[40] = "";
                if (print_line_number) snprintf(res, sizeof(res), "%d > ", lc);
                grepLineFunc(line, word, printGreen, res);
        }
        return 0;
}
