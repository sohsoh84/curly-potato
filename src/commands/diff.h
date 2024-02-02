#ifndef DIFF_CMD_H
#define DIFF_CMD_H

#include <stdio.h>

int diffCommand(int argc, char* argv[]);
int fileDiff(FILE* stream, char* path1, char* path2, int s1, int e1, int s2, int e2, int file_name_ignore);
int stashDiff(int ignore_unmatch_files, FILE* stream, int stash_id);

int commitDiff(int ignore_unmatch_files, FILE* stream, char* commit_id1, char* commit_id2);

#endif