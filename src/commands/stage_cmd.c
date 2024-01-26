#include "stage_cmd.h"
#include <stdlib.h>
#include <stdio.h>
#include "../dotcupot.h"
#include "../paths.h"

static int createRequiredDirectories(char* path) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "mkdir -p %s", dirName(path));
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to create directory %s\n", path);
                return 1;
        }

        return 0;
}

static int removeFileDir(char* path) {
        if (fileExists(path)) {
                char command[PATH_MAX];
                snprintf(command, sizeof(command), "rm -rf %s", path);
                if (system(command) == -1) {
                        fprintf(stderr, "something went wrong while trying to remove directory %s\n", path);
                        return 1;
                }
        }

        return 0;
}

static int copyFileDir(char* src, char* dest) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "cp -r %s %s", src, dirName(dest));
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to copy %s to %s\n", src, dest);
                return 1;
        }

        return 0;
}

static int addToStageingArea(char* stage_path_, char* file_path_) { // TODO: remove .cupot from staging area
        if (stage_path_ == NULL || stage_path_ == NULL) {
                fprintf(stderr, "wrong stage_path or file_path!\n");
                return 1;
        }
        
        char stage_path[PATH_MAX], file_path[PATH_MAX];
        realpath(stage_path_, stage_path);
        realpath(file_path_, file_path);

        if (!dotCupotPath(file_path)) {
                fprintf(stderr, "%s is not in a cupot repository!!\n", file_path);
                return 1;
        }

        char relative_path[PATH_MAX];
        suffixPath(relative_path, file_path, dirName(projectPath(file_path)));

        char* dest_path = mergePaths(stage_path, relative_path);
        if (createRequiredDirectories(dest_path)) return 1;
        if (removeFileDir(dest_path)) return 1;
        if (copyFileDir(file_path, dest_path)) return 1;
        removeFileDir(mergePaths(mergePaths(stage_path, projectName(file_path)), ".cupot")); // this line is needed to remove the .cupot file from the staging area
        return 0;
}

int stageCommand(int argc, char *argv[]) {
        addToStageingArea(mergePaths(dotCupotPath(cwdPath()), "staging_area"), argv[2]);
        return 0;
}