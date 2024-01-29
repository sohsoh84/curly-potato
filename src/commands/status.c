#include "status.h"
#include "../vcs.h"
#include "../constants.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../staging_area.h"
#include "../diff.h"

#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>


enum FILE_STATUS {
        NOT_CHANGED,
        ACCESS_CHANGED,
        DELETED,
        MODIFIED,
        ADDED
};

char* latestVersionPath(const char* complete_path) {
        char real_path[MAX_PATH_LEN];
        realpath(complete_path, real_path);

        char path[MAX_PATH_LEN];
        releativePath(real_path, path);
        char* proj_path = mergePaths(dotCupotPath(cwdPath()), TEMP_LATEST_VERSION);
        return mergePaths(proj_path, path);
}

int status(char* path) {
                char real_path[PATH_MAX];
                realpath(path, real_path);

                char* latest_version_path = latestVersionPath(real_path);
                char* stage_path = stageFilePath(real_path);       

                if (fileExists(real_path)) {
                        if (!fileExists(latest_version_path)) return ADDED;
                        else if (absoluteDiff(latest_version_path, real_path)) return MODIFIED;
                }

                return NOT_CHANGED;
}

int in_stage(char* path) {
        char real_path[PATH_MAX];
        realpath(path, real_path);

        char* latest_version_path = latestVersionPath(real_path);
        char* stage_path = stageFilePath(real_path);       

        if (fileExists(latest_version_path) && !absoluteDiff(real_path, latest_version_path)) return 1;
        if (fileExists(stage_path) && !absoluteDiff(real_path, stage_path)) return 1;
        return 0;
}

void printStatusOfFiles(char* directoryName) {
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

                if (!strcmp(basename(filePath), ".cupot")) continue;
                
                if (ent -> d_type == DT_DIR) {
                        printStatusOfFiles(filePath);
                } else if (ent -> d_type == DT_REG) {
                        char real_path[PATH_MAX];
                        realpath(filePath, real_path);

                        char* latest_version_path = latestVersionPath(real_path);
                        char rel_path[PATH_MAX];
                        releativePath(real_path, rel_path);
                        char* stage_path = stageFilePath(real_path);
                        
                        int status_ = status(real_path), in_stage_ = in_stage(real_path);
                        if (status_ == NOT_CHANGED) continue;
                        printf((in_stage_ ? GREEN : RED));
                        printf("%s: ", rel_path);
                        printf(RESET);

                        printf(status_ == ADDED ? GREEN : status_ == MODIFIED ? YELLOW : status_ == DELETED ? RED : BLUE);
                        printf((in_stage_ ? "+" : "-"));
                        printf(status_ == ADDED ? "A" : status_ == MODIFIED ? "M" : status_ == DELETED ? "D" : "T");
                        printf(RESET "\n");
                }
        }

        closedir(dir);
}

int statusCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, "you should be inside a cupot repository\n");
                return 1;
        }

        if (argc && !strcmp(argv[0], "--all")) {
                chroot(projectPath(cwdPath()));
        }
        
        char* starting_path = cwdPath();
        buildProjectFromCommit(mergePaths(dotCupotPath(cwdPath()), TEMP_LATEST_VERSION), getHead(getCWB())->id);
        char* lastest_version_path = mergePaths(mergePaths(dotCupotPath(cwdPath()), TEMP_LATEST_VERSION), 
                projectName(cwdPath()));

        int _ = 0;
        printStatusOfFiles(starting_path); // TODO:
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
