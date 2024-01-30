#include "utils.h"
#include "constants.h"

#include <string.h>
#include <stdio.h>
#include <libgen.h>
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
