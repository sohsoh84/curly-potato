#include "precommit.h"
#include "../hook.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../tracker.h"
#include "../constants.h"
#include "../hook_funcs.h"
#include "../hook_fixes.h"
#include "../utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define hook_cnt        7
char hook_names[hook_cnt][50] = {"eof-blank-space", "balance-braces", "character-limit", "file-size-check", "todo-check", "static-error-check", "time-limit"};
HOOK_RESULT (*hook_funcs[hook_cnt]) (char*) = {eof_blank_space, balance_braces, character_limit, file_size_check, todo_check, static_error_check, time_limit};
HOOK_FIX_RESULT (*fix_funcs[hook_cnt]) (char*) = {eof_blank_space_fix, balance_braces_fix, NULL,          NULL,      NULL,        NULL        ,     NULL };

int validHook(char* hook) {
        for (int i = 0; i < hook_cnt; i++)
                if (!strcmp(hook, hook_names[i]))
                        return 1;

        return 0;
}

int runHooks(char* file, int try_fix) {
        printf("%s:\n", file + strlen(stagingAreaPath(cwdPath())) + 1);
        int fixed = 0;
        for (int j = 0; j < hook_cnt; j++) {
                if (hookExists(hook_names[j])) {
                        printf("\t%s:", hook_names[j]);
                        
                        int x = 16 + strlen(hook_names[1]);
                        for (int i = 0; i < x - strlen(hook_names[j]); i++)
                                printf(" ");

                        HOOK_RESULT res = hook_funcs[j](file);
                        if (res == HOOK_PASSED) printf(GREEN "PASSED\n" RESET);
                        if (res == HOOK_SKIPPED) printf(YELLOW "SKIPPED\n" RESET);
                        if (res == HOOK_FAILED) {
                                printf(RED "FAILED" RESET);
                                if (try_fix) {
                                        printf(BLUE " > fix: " RESET);
                                        if (fix_funcs[j] == NULL) printf(YELLOW "UNAVAILABLE" RESET);
                                        else {
                                                HOOK_FIX_RESULT res = fix_funcs[j](file);
                                                if (res == FIX_FAILED) printf(RED "FAILED" RESET);
                                                else printf(GREEN "FIXED" RESET), fixed++;
                                        }
                                }

                                printf("\n");
                        }
                }
        }

        return fixed;
}

int precommitCommand(int argc, char* argv[]) {
        if (!dotCupotPath(cwdPath())) {
                printf("you should be inside a cupot repository\n");
                return 1;
        }

        if (argc == 1 && !strcmp(argv[0], "list")) {
                for (int i = 0; i < hook_cnt; i++) {
                        printf("%s\n", hook_names[i]);
                }

                return 0;
        }

        if (argc == 2 && !strcmp(argv[0], "applied") && !strcmp(argv[1], "hooks")) {
                for (int i = 0; i < hook_cnt; i++) {
                        if (hookExists(hook_names[i])) {
                                printf("%s\n", hook_names[i]);
                        }
                }

                return 0;
        }

        if (argc == 3 && !strcmp(argv[0], "add") && !strcmp(argv[1], "hook")) {
                if (!validHook(argv[2])) {
                        printf("invalid hook: %s\n", argv[2]);
                        return 1;
                }

                if (hookExists(argv[2])) {
                        printf("hook %s already exists\n", argv[2]);
                        return 1;
                }

                addHook(argv[2]);
                printf("hook %s added\n", argv[2]);
                return 0;
        }

        if (argc == 3 && !strcmp(argv[0], "remove") && !strcmp(argv[1], "hook")) {
                if (!validHook(argv[2])) {
                        printf("invalid hook: %s\n", argv[2]);
                        return 1;
                }

                if (!hookExists(argv[2])) {
                        printf("hook %s does not exist\n", argv[2]);
                        return 1;
                }

                removeHook(argv[2]);
                printf("hook %s removed\n", argv[2]);
                return 0;
        }

        if (argc == 0 || (argc == 1 && !strcmp(argv[0], "-u"))) {
                char** tracked_filse;
                int fixed = 0;
                int count = allTrackedFiles(stageTrackerPath(), &tracked_filse);
                for (int i = 0; i < count; i++) {
                        char rel_path[MAX_PATH_LEN];
                        releativePath(tracked_filse[i], rel_path);

                        char* stage_path = mergePaths(stagingAreaPath(cwdPath()), rel_path);
                        if (fileExists(stage_path)) fixed += runHooks(stage_path, (argc == 1) ? 1 : 0);
                }       

                if (fixed)
                        printf(CYAN "there are some fixed hooks, the changes are not applied in main files, to see new files,\n"
                        "run \"cupot commit\" and then \"cupot checkout HEAD --force\"\n" RESET);
                return 0;
        }

        if (!strcmp(argv[0], "-f")) {
                for (int i = 1; i < argc; i++) {
                        if (!fileExists(argv[i])) {
                                printf("Invalid file path: %s\n", argv[i]);
                                continue;
                        }

                        char rel_path[MAX_PATH_LEN];
                        releativePath(argv[i], rel_path);

                        char* stage_path = mergePaths(stagingAreaPath(cwdPath()), rel_path);
                        if (fileExists(stage_path)) runHooks(stage_path, 0);
                        else printf("File %s is not staged\n", argv[i]);
                }

                return 0;
        }

        printf("Invalid usage of precommit\n");
        return 0;
}

int checkHooks() {
        FILE *pipe = popen("cupot pre-commit", "r");
        if (pipe == NULL) {
                perror("popen");
                return EXIT_FAILURE;
        }

        if (checkSubstringInFile(pipe, "FAILED")) {
                fclose(pipe);
                return 0;
        }

        pclose(pipe);
        return 1;
}