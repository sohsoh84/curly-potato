#include <stdio.h>

#ifndef UTILS_H
#define UTILS_H

void strip(char* s);
int compareTimes(char* time_str1, char* time_str2);

int removeEmptyDirs(char* base_path);

char* latestVersionPath(const char* complete_path, const char* VERSON_NAME);

int match_wc(char *pattern, char *candidate, int p, int c);

void readDummyLines(FILE* file, int k);

int checkSubstringInFile(FILE* file, char* substring);

FILE* devnull();

#endif