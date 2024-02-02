#ifndef STASH_H
#define STASH_H

#include "constants.h"

typedef struct StashConfigs {
        char branch_name[MAX_COMMIT_MESSAGE_LEN];
        char message[MAX_COMMIT_MESSAGE_LEN];
} StashConfigs;

StashConfigs* createStashConfigs(char* branch_name, char* message);

int stashSize();

int addStashDir();
int popStashDir(int index);

char* getStashDirPath(int k);

int writeConfigsToStashTop(StashConfigs *configs);

StashConfigs* readStashConfigs(int k);

#endif