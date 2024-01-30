#include "constants.h"

#ifndef COMMIT_H
#define COMMIT_H

struct CommitConfigs {
        char id[COMMIT_ID_LEN + 1];
        char merge_id[COMMIT_ID_LEN + 1];
        char branch_name[MAX_CONFIG_LENGHT];
        char author_name[MAX_CONFIG_LENGHT];
        char author_email[MAX_CONFIG_LENGHT];
        char time[MAX_CONFIG_LENGHT];
        char message[MAX_COMMIT_MESSAGE_LEN];
        char parent_id[COMMIT_ID_LEN + 1];
};

typedef struct CommitConfigs CommitConfigs;

CommitConfigs *createCommitConfigs(char *parent_id, char* branch, char *message, char* author_name, char *author_email);

int commit(char* path, CommitConfigs* configs);
int emptyCommit(CommitConfigs* configs);

CommitConfigs* getCommitConfigs(char* commit_id);

int getCommitCounts();
char** getAllCommitIDs();
CommitConfigs** getAllCommitConfigs();

#endif