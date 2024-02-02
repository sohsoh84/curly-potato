#include "tracker.h"
#include "paths.h"
#include "constants.h"
#include "utils.h"
#include "syscalls.h"
#include "commits.h"
#include "dotcupot.h"
#include "stash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>

int cleanRecursivly(char* base_path, char* path, char* track_file) {
        if (!fileExists(path)) return 1;
        if (!isDirectory(path)) {
                char real_path[MAX_PATH_LEN];
                realpath(path, real_path);
                char rel_ptah;

                char tmp_path[MAX_PATH_LEN];
                suffixPath(tmp_path, path, base_path);
                
                char* proj_path = mergePaths(projectPath(cwdPath()), tmp_path);
                if (!isTracked(track_file, proj_path)) {
                        return removeFileDir(path);
                }

                return 0;
        }


        DIR *dir = opendir(path);
        if (dir == NULL) {
                printf("Error: Cannot open directory\n");
                return 0;
        }

        struct dirent *ent;


        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }

                char filePath[PATH_MAX];
                snprintf(filePath, sizeof(filePath), "%s/%s", path, ent -> d_name);

                if (!strcmp(basename(filePath), ".cupot")) continue;
                if (ent -> d_type == DT_DIR || ent->d_type == DT_REG) cleanRecursivly(base_path, filePath, track_file);
        }

        closedir(dir);
        return 0;
        
}

int cleanWithTracker(char *base_project_path, char *track_file) {
        char* base_copy = (char*) malloc(strlen(base_project_path) + 4);
        strcpy(base_copy, base_project_path);
        cleanRecursivly(base_project_path, base_copy, track_file);
        return 0;
}

int addTrackedFile(char *track_file, char *path_)
{
        char path[MAX_PATH_LEN];
        if (realpath(path_, path) == NULL) strcpy(path, path_);
        if (isTracked(track_file, path)) return 0;

        FILE *fp = fopen(track_file, "a");
        if (fp == NULL) {
                printf("Couldn't open file '%s'\n", track_file);
                return -1;
        }

        fprintf(fp, "%s\n", path);
        fclose(fp);
        return 0;
}

int removeTrackedFile(char *track_file, char* path_) {
        char path[MAX_PATH_LEN];
        if (realpath(path_, path) == NULL) strcpy(path, path_);
        if (!isTracked(track_file, path)) return 0;

        char track_file_bak[MAX_PATH_LEN];
        strcpy(track_file_bak, track_file);
        strcat(track_file_bak, ".bak");

        FILE *fp = fopen(track_file, "r");
        FILE *fp_bak = fopen(track_file_bak, "w");
        if (fp == NULL) {
                printf("Couldn't open file '%s'\n", track_file);
                return -1;
        }

        if (fp_bak == NULL) {
                printf("Couldn't open file '%s'\n", track_file_bak);
                return -1;
        }

        char line[MAX_PATH_LEN];
        while (fgets(line, MAX_PATH_LEN, fp) != NULL) {
                strip(line);
                if (strcmp(line, path)) {
                        fprintf(fp_bak, "%s\n", line);
                }
        }

        fclose(fp);
        fclose(fp_bak);
        
        removeFileDir(track_file);
        copyDirWithName(track_file_bak, track_file);
        removeFileDir(track_file_bak);
        return 0;
}

int isTracked(char *trackFile, char *path_) {
        FILE* file = fopen(trackFile, "r");
        if (file == NULL) {
                printf("Couldn't open track file %s\n", trackFile);
                return -1;
        }

        char path[MAX_PATH_LEN];
        if (realpath(path_, path) == NULL) strcpy(path, path_);


        char line[MAX_PATH_LEN];
        while (fgets(line, MAX_PATH_LEN, file) != NULL) {
                strip(line);

                if (!strcmp(line, path)) {
                        fclose(file);
                        return 1;
                }
        }

        fclose(file);
        return 0;
}

char *commitTrackerPath(char *commit_id) {
        return mergePaths(mergePaths(commitsAreaPath(cwdPath()), commit_id), TRACK_FILE_NAME);
}

char *stageTrackerPath() {
        return mergePaths(stagingAreaPath(cwdPath()), TRACK_FILE_NAME);
}

char* stashTrackerPath(int stash_id) {
        return mergePaths(getStashDirPath(stash_id), TRACK_FILE_NAME);
}

char* trackRelativePath(char* path) {
        char* new_path = strdup(path);
        return new_path + strlen(projectPath(cwdPath())) + 1;
}

int allTrackedFiles(char* track_path, char ***result) {
        int count = 0;
        (*result) = (char**) malloc(sizeof(char*) * MAX_FILE_COUNT);
        char line[MAX_PATH_LEN];
        
        FILE* fp = fopen(track_path, "r");
        if (fp == NULL) {
                printf("Couldn't open file '%s'\n", track_path);
                return -1;
        }

        while (fgets(line, MAX_PATH_LEN, fp )!=  NULL) {
                strip(line);
                if (!strlen(line)) continue;
                (*result)[count] = (char*) malloc(MAX_PATH_LEN);
                strcpy((*result)[count], line);
                count++;
        }

        fclose(fp);
        return count;
}

int addRecursivlyToTrackedFile(char *track_path, char *directoryName) {
        if (!fileExists(directoryName)) return 1;
        if (!isDirectory(directoryName)) return addTrackedFile(track_path, directoryName);

        DIR *dir = opendir(directoryName);
        if (dir == NULL) {
                printf("Error: Cannot open directory\n");
                return 0;
        }

        struct dirent *ent;


        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }

                char filePath[PATH_MAX];
                snprintf(filePath, sizeof(filePath), "%s/%s", directoryName, ent -> d_name);

                if (!strcmp(basename(filePath), ".cupot")) continue;
                
                if (ent -> d_type == DT_DIR || ent->d_type == DT_REG) addRecursivlyToTrackedFile(track_path, filePath);
        }

        closedir(dir);
        return 0;
}
