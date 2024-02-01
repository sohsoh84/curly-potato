#include "hook.h"
#include "dotcupot.h"
#include "paths.h"
#include "utils.h"
#include "constants.h"
#include "syscalls.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *hookPath() {
        return mergePaths(dotCupotPath(cwdPath()), "hooks");
}

int addHook(char* hook) {
        if (!fileExists(hookPath())) {
                FILE* file = fopen(hookPath(), "w");
                fclose(file);
        }

        FILE* file = fopen(hookPath(), "a");
        if (!file) return 1;
        
        fprintf(file, "%s\n", hook);
        fclose(file);
        return 0;
}

int hookExists(char* hook) {
        FILE* file = fopen(hookPath(), "r");
        if (!file) return 0;

        char line[1024];
        while (fgets(line, 1024, file)) {
                strip(line);
                if (strcmp(line, hook) == 0) {
                        fclose(file);
                        return 1;
                }
        }

        fclose(file);
        return 0;
}

int hookCount() {
        FILE* file = fopen(hookPath(), "r");
        if (!file) return 0;

        char line[1024];
        int count = 0;
        while (fgets(line, 1024, file)) {
                strip(line);
                if (strlen(line)) count++;
        }

        fclose(file);
        return count;
}


int removeHook(char* hook) {
        char hook_bak[MAX_PATH_LEN];
        strcpy(hook_bak, hookPath());
        strcat(hook_bak, ".bak");

        FILE *fp = fopen(hookPath(), "r");
        FILE *fp_bak = fopen(hook_bak, "w");
        if (fp == NULL) {
                printf("Couldn't open file '%s'\n", hookPath());
                return -1;
        }

        if (fp_bak == NULL) {
                printf("Couldn't open file '%s'\n", hook_bak);
                return -1;
        }

        char line[MAX_PATH_LEN];
        while (fgets(line, MAX_PATH_LEN, fp) != NULL) {
                strip(line);
                if (strcmp(line, hook)) {
                        fprintf(fp_bak, "%s\n", line);
                }
        }

        fclose(fp);
        fclose(fp_bak);
        
        removeFileDir(hookPath());
        copyDirWithName(hook_bak, hookPath());
        removeFileDir(hook_bak);
        return 0;
}
