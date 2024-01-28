#include "commits.h"
#include "constants.h"
#include "dotcupot.h"
#include "syscalls.h"
#include "paths.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char random_char() {
        int x = rand() % 36;
        if (x < 10) return (char) x + '0';
        return (char) (x - 10 + 'a');
}

CommitConfigs *createCommitConfigs(char *parent_id, char *message, char* author_name, char* author_email) {
        CommitConfigs *configs = malloc(sizeof(CommitConfigs));
        configs->id[COMMIT_ID_LEN] = '\0';
        for (int i = 0; i < COMMIT_ID_LEN; i++)
                configs->id[i] = random_char();

        strcpy(configs->message, message);
        strcpy(configs->parent_id, parent_id);

        time_t now = time(NULL);
        strcpy(configs->time, ctime(&now));
        strcpy(configs->author_name, author_name);
        strcpy(configs->author_email, author_email);

        return configs;
}

int commit(char* path, CommitConfigs* configs) {
        char* commit_path = mergePaths(commitsAreaPath(cwdPath()), configs->id);

        if (makeDirectory(commit_path)) {
                fprintf(stderr, "unexpected error creating directory %s\n", commit_path);
                return 1;
        }

        FILE* configFile = fopen(mergePaths(commit_path, COMMIT_CONF_FILE), "wb");
        if (configFile == NULL) {
                fprintf(stderr, "unable to create config file %s\n", mergePaths(commit_path, COMMIT_CONF_FILE));
                return 1;
        }

        fwrite(configs, sizeof(CommitConfigs), 1, configFile);
        fclose(configFile);

        return copyDirWithName(path, commit_path);
}

CommitConfigs* getCommitConfigs(char* commit_id) {
        CommitConfigs* configs = (CommitConfigs*) malloc(sizeof(CommitConfigs));
        FILE *file = fopen(mergePaths(mergePaths(commitsAreaPath(cwdPath()), commit_id), COMMIT_CONF_FILE), "rb");
        if (!file) {
                printf("Couldn't open the commit\n");
                return NULL;
        }

        fread(configs, sizeof(CommitConfigs), 1, file);
        return configs;
}