#include "utils.h"
#include "constants.h"
#include "paths.h"
#include "dotcupot.h"

#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <dirent.h>

void strip(char* s) {
        while (strlen(s) && (s[strlen(s) - 1] == ' ' || s[strlen(s) - 1] == '\n'))
                s[strlen(s) - 1] = '\0';
}

static int mon(char* name) {
        char months[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

        for (int i = 0; i < 12; i++) {
                if (strcmp(name, months[i]) == 0) {
                        return i + 1;
                }
        }

        return -1;
}

int compareTimes(char* time_str1, char* time_str2) { // returns time_str1 < time_str2
        char _1[MAX_CONFIG_LENGHT], M1[MAX_CONFIG_LENGHT], _2[MAX_CONFIG_LENGHT], M2[MAX_CONFIG_LENGHT];
        int d1, d2, h1, h2, m1, m2, s1, s2, y1, y2;
        sscanf(time_str1, "%s %s %d %d:%d:%d %d", _1, M1, &d1, &h1, &m1, &s1, &y1);
        sscanf(time_str2, "%s %s %d %d:%d:%d %d", _2, M2, &d2, &h2, &m2, &s2, &y2);

        if (y1 != y2) return y1 < y2;
        if (strcmp(M1, M2)) return mon(M1) < mon(M2);
        if (d1 != d2) return d1 < d2;
        if (h1 != h2) return h1 < h2;
        if (m1 != m2) return m1 < m2;
        if (s1 != s2) return s1 < s2;

        return 0;
}

int removeEmptyDirs(char *dirPath) {
        DIR *dir = opendir(dirPath);
        if (dir == NULL) {
                printf("Error: Cannot open directory\n");
                return 0;
        }

        struct dirent *ent;

        int result = 0;

        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }

                char filePath[MAX_PATH_LEN];
                snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, ent -> d_name);
                if (!strcmp(basename(filePath), ".cupot")) continue;
                
                if (ent -> d_type == DT_DIR) {
                        result += removeEmptyDirs(filePath);
                } else if (ent -> d_type == DT_REG) result++;
        }

        closedir(dir);
        return result;
}


char* latestVersionPath(const char* complete_path, const char* VERSION_NAME) {
        char real_path[MAX_PATH_LEN];
        realpath(complete_path, real_path);

        char path[MAX_PATH_LEN];
        releativePath(real_path, path);
        char* proj_path = mergePaths(dotCupotPath(cwdPath()), VERSION_NAME);
        return mergePaths(proj_path, path);
}

void readDummyLines(FILE* file, int k) {
        while (k--) {
                char _[4096];
                fgets(_, sizeof(_), file);
        }
}

int checkSubstringInFile(FILE *file, char *substring) {
        char line[4096];
        while (fgets(line, sizeof(line), file) != NULL) {
                if (strstr(line, substring)) {
                        return 1;
                }
        }

        return 0;
}

int match_wc(char *pattern, char *candidate, int p, int c) {
        if (pattern[p] == '\0') {
                return candidate[c] == '\0';
        } else if (pattern[p] == '*') {
        for (; candidate[c] != '\0'; c++) {
                if (match_wc(pattern, candidate, p+1, c))
                        return 1;
        }
                return match_wc(pattern, candidate, p+1, c);
        } else if (pattern[p] != '?' && pattern[p] != candidate[c]) {
                return 0;
        }  else {
                 return match_wc(pattern, candidate, p+1, c+1);
        }
}

FILE* devnull() {
        FILE* result = fopen("/dev/null", "a");
        return result;
}