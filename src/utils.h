#ifndef UTILS_H
#define UTILS_H

void strip(char* s);
int compareTimes(char* time_str1, char* time_str2);

int removeEmptyDirs(char* base_path);

char* latestVersionPath(const char* complete_path);

int match_wc(char *pattern, char *candidate, int p, int c);


#endif