#ifndef VCS_H
#define VCS_H

#include "commits.h"

int addFilesFromCommit(char* path, char* commit_id);
int buildProjectFromCommit(char* path, char* commit_id);

CommitConfigs* getHead();
#endif