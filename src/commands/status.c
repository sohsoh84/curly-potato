#include "status.h"
#include "../vcs.h"
#include "../constants.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../staging_area.h"
#include "../diff.h"
#include "../tracker.h"
#include "../utils.h"

#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include "sys/stat.h"


enum FILE_STATUS {
        NOT_CHANGED,
        ACCESS_CHANGED,
        DELETED,
        MODIFIED,
        ADDED
};

int status(char* path) {
        char real_path[PATH_MAX];
        realpath(path, real_path);

        char* latest_version_path = latestVersionPath(real_path, TEMP_LATEST_VERSION);
        char* stage_path = stageFilePath(real_path);       

        if (fileExists(real_path)) {
                if (!fileExists(latest_version_path)) return ADDED;
                else if (absoluteDiff(latest_version_path, real_path)) return MODIFIED;
                else {
                        struct stat fstat1;
                        struct stat fstat2;

                        if (stat(latest_version_path, &fstat1) == 0 && stat(real_path, &fstat2) == 0)
                                if (fstat1.st_mode != fstat2.st_mode) return ACCESS_CHANGED;
                }
                
        }

        return NOT_CHANGED;
}

int in_stage(char* path) {
        char real_path[PATH_MAX];
        realpath(path, real_path);

        char* latest_version_path = latestVersionPath(real_path, TEMP_LATEST_VERSION);
        char* stage_path = stageFilePath(real_path);       

        if (fileExists(latest_version_path) && !absoluteDiff(real_path, latest_version_path)) {
                struct stat fstat1;
                struct stat fstat2;

                if (stat(latest_version_path, &fstat1) == 0 && stat(real_path, &fstat2) == 0)
                        if (fstat1.st_mode == fstat2.st_mode) return 1;
        }

        if (fileExists(stage_path) && !absoluteDiff(real_path, stage_path)) {
                struct stat fstat1;
                struct stat fstat2;

                if (stat(stage_path, &fstat1) == 0 && stat(real_path, &fstat2) == 0)
                        if (fstat1.st_mode == fstat2.st_mode) return 1;
        }
        
        return 0;
}

void printStatusOfFiles(char* directoryName, char* starting_dir_) {
        DIR *dir = opendir(directoryName);
        if (dir == NULL) {
                printf("Error: Cannot open directory\n");
                return;
        }

        struct dirent *ent;


        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }

                char filePath[PATH_MAX];
                snprintf(filePath, sizeof(filePath), "%s/%s", directoryName, ent -> d_name);

                if (!strcmp(BaseName(filePath), ".cupot")) continue;
                
                if (ent -> d_type == DT_DIR) {
                        printStatusOfFiles(filePath, starting_dir_);
                } else if (ent -> d_type == DT_REG) {
                        char real_path[PATH_MAX];
                        realpath(filePath, real_path);

                        char* latest_version_path = latestVersionPath(real_path, TEMP_LATEST_VERSION);
                        char rel_path[PATH_MAX];
                        releativePath(real_path, rel_path);
                        char* stage_path = stageFilePath(real_path);
                        
                        int status_ = status(real_path), in_stage_ = in_stage(real_path);
                        if (status_ == NOT_CHANGED) continue;
                        printf((in_stage_ ? GREEN : RED));
                        printf("%s:    ", relativePathString(real_path, starting_dir_));
                        printf(RESET);

                        printf(status_ == ADDED ? GREEN : status_ == MODIFIED ? YELLOW : status_ == ACCESS_CHANGED ? BLUE : BLUE);
                        printf((in_stage_ ? "+" : "-"));
                        printf(status_ == ADDED ? "A" : status_ == MODIFIED ? "M" : status_ == ACCESS_CHANGED ? "T" : "D");
                        printf(RESET "\n");
                }
        }

        closedir(dir);
}

void printRemoveStatus(char* track_file, char* starting_path_, char* base) {
        char starting_path[PATH_MAX];
        realpath(starting_path_, starting_path);
        char** all_tracked;
        int count = allTrackedFiles(track_file, &all_tracked);
        for (int i = 0; i < count; i++) {
                if (!fileExists(all_tracked[i]) && isSubdirectoryUnsafe(starting_path, all_tracked[i])) {
                        int in_stage_ = 1;
                        if (isTracked(stageTrackerPath(), all_tracked[i]))
                                in_stage_ = 0;

                        printf((in_stage_ ? GREEN : RED));
                        printf("%s:    ", relativePathStringUnsafe(all_tracked[i], base));
                        printf(RESET);

                        printf(RED);
                        printf((in_stage_ ? "+" : "-"));
                        printf("D");
                        printf(RESET "\n");
                }
        }
}

int statusCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, "you should be inside a cupot repository\n");
                return 1;
        }

        char* starting_path = strdup(cwdPath());
        if (argc && !strcmp(argv[0], "--all")) {
                starting_path = strdup(projectPath(cwdPath()));
        }
        
        buildProjectFromCommit(mergePaths(dotCupotPath(cwdPath()), TEMP_LATEST_VERSION), getHead(getCWB())->id);
        char* lastest_version_path = mergePaths(mergePaths(dotCupotPath(cwdPath()), TEMP_LATEST_VERSION), 
                projectName(cwdPath()));

        int _ = 0;
        printStatusOfFiles(starting_path, cwdPath()); // TODO:
        printRemoveStatus(commitTrackerPath(getCWC()), starting_path, cwdPath());
        return 0;
}

int checkIfUncommitedFiles() {
        FILE *pipe = popen("cupot status", "r");
        if (pipe == NULL) {
                perror("popen");
                return EXIT_FAILURE;
        }

        char buf[1024];
        int n = fread(buf, 1, sizeof(buf), pipe);
        pclose(pipe);

        return n > 0;
}
