#include "stash.h"
#include "constants.h"
#include "dotcupot.h"
#include "paths.h"
#include "syscalls.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StashConfigs* createStashConfigs(char* branch_name, char* message) {
        StashConfigs* configs = (StashConfigs*)malloc(sizeof(StashConfigs));
        strcpy(configs->branch_name, branch_name);
        strcpy(configs->message, message);
        return configs;
}


int stashSize() {
        char* stash_path = stashAreaPath(cwdPath());
        for (int i = 0; ; i++)
                if (!fileExists(getStashDirPath(i)))
                        return i;

        return 0;
}

int addStashDir() {
        int k = stashSize();
        for (int i = k - 1; i >= 0; i--) {
                if (fileExists(getStashDirPath(i + 1)))
                        removeFileDir(getStashDirPath(i + 1));
                copyDirWithName(getStashDirPath(i), getStashDirPath(i + 1));
        }

        if (fileExists(getStashDirPath(0)))
                removeFileDir(getStashDirPath(0));

        makeDirectory(getStashDirPath(0));
        FILE* tracker = fopen(mergePaths(getStashDirPath(0), "tracker"), "w");
        fclose(tracker);
        return 0;
}

int popStashDir(int ind) {
        int k = stashSize();
        removeFileDir(getStashDirPath(ind));
        for (int i = ind + 1; i < k; i++) {
                copyDirWithName(getStashDirPath(i), getStashDirPath(i - 1));
                removeFileDir(getStashDirPath(i));
        }

        return 0;
}

char *getStashDirPath(int i) {
        char tmp[10];
        sprintf(tmp, "%d", i);
        return mergePaths(stashAreaPath(cwdPath()), tmp);
}

int writeConfigsToStashTop(StashConfigs *configs) {
        FILE* fp = fopen(mergePaths(getStashDirPath(0), "configs"), "wb");
        if (!fp) {
                printf("Error opening config file in stash top\n");
                return 1;
        }

        fwrite(configs, sizeof(StashConfigs), 1, fp);
        fclose(fp);
        return 0;
}

StashConfigs* readStashConfigs(int k) {
        FILE* fp = fopen(mergePaths(getStashDirPath(k), "configs"), "rb");
        if (!fp) {
                printf("Error opening config file in stash top\n");
                return NULL;
        }

        StashConfigs* configs = (StashConfigs*)malloc(sizeof(StashConfigs));
        fread(configs, sizeof(StashConfigs), 1, fp);
        fclose(fp);
        return configs;
}