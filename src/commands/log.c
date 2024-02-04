#include "log.h"
#include "../commits.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../utils.h"
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <regex.h>
#include <dirent.h>
#include <libgen.h>



static int always_true(CommitConfigs* configs, int argc, char* argv[]) {
        return 1;
}

static int branch_check(CommitConfigs* configs, int argc, char* argv[]) {
        return !strcmp(configs->branch_name, argv[0]);
}

static int author_check(CommitConfigs* configs, int argc, char* argv[]) {
        return !strcmp(configs->author_name, argv[0]);
}

static int since_check(CommitConfigs* configs, int argc, char* argv[]) {
        strip(argv[0]);
        strip(configs->time);
        return compareTimes(argv[0], configs->time) || !strcmp(argv[0], configs->time);
}

static int before_check(CommitConfigs* configs, int argc, char* argv[]) {
        strip(argv[0]);
        strip(configs->time);
        return !compareTimes(argv[0], configs->time) || !strcmp(argv[0], configs->time);
}

static int search_check(CommitConfigs* configs, int argc, char* argv[]) {
        for (int i = 0; i < argc; i++) {
                regex_t regex;
                regcomp(&regex, argv[i], REG_EXTENDED);
                int result = regexec(&regex, configs->message, 0, NULL, 0);
                regfree(&regex);

                if (result == 0) return 1;
        }

        return 0;
}

static int countFiles(const char* directoryName) {
        int result = 0;
        DIR *dir = opendir(directoryName);
        if (dir == NULL) {
                printf("Error: Cannot open directory\n");
                return 0;
        }

        struct dirent *ent;


        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }

                char filePath[PATH_MAX];
                snprintf(filePath, sizeof(filePath), "%s/%s", directoryName, ent -> d_name);

                if (!strcmp(basename(filePath), ".cupot")) continue;
                
                if (ent -> d_type == DT_DIR) {
                        result += countFiles(filePath);
                } else if (ent -> d_type == DT_REG) {
                        result++;
                }
        }

        closedir(dir);
        return result;
}

void printCommit(CommitConfigs *config) {
        printf(YELLOW);
        printf("commit %s (", config->id);
        printf(RESET);
        printf(CYAN);
        printf("%s", config->branch_name);
        printf(RESET);
        printf(YELLOW);
        printf(")\n");
        printf(RESET);

        printf("Author: %s <%s>\n", config->author_name, config->author_email);
        printf("Date: %s\n", config->time);
        printf("#Files: %d\n", countFiles(mergePaths(mergePaths(commitsAreaPath(cwdPath()), config->id), 
                projectName(cwdPath()))));
        printf("\t%s\n", config->message);
        printf("\n\n");
}

int logCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, RED "You should be inside a cupot repository to run this command\n" RESET "try running cupot init\n");
                return 1;
        }

        int max_cnt = INT_MAX;
        int (*checker_function) (CommitConfigs*, int, char*[]) = always_true;

        if (argc == 1) {
                printf("not enough arguments\n");
                return 1;
        }

        if (argc > 1) {
                if (!strcmp(argv[0], "-n"))
                        max_cnt = atoi(argv[1]);
                else if (!strcmp(argv[0], "-branch"))
                        checker_function = branch_check;
                else if (!strcmp(argv[0], "-author"))
                        checker_function = author_check;
                else if (!strcmp(argv[0], "-since"))
                        checker_function = since_check;
                else if (!strcmp(argv[0], "-before"))
                        checker_function = before_check;
                else if (!strcmp(argv[0], "-search"))
                        checker_function = search_check;
        }

        argc--;
        argv++;

        int commits_cnt = getCommitCounts();
        CommitConfigs** commits = getAllCommitConfigs();

        for (int i = 0; i < commits_cnt; i++) {
                if (checker_function(commits[i], argc, argv) && max_cnt > 0) {
                        max_cnt--;
                        printCommit(commits[i]);
                }
        }
        return 0;
}