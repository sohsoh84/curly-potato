#ifndef DIFF_H
#define DIFF_H

#include <stdio.h>

void diffStrip(char* s);

int lineCounts(char* path);

char* readRealLine(FILE* file, int* lc);

int absoluteDiff(char* file1, char* file2);

#endif