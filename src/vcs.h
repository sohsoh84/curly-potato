#ifndef VCS_H
#define VCS_H

int addFilesFromCommit(char* path, char* commit_id);
int buildProjectFromCommit(char* path, char* commit_id);

#endif