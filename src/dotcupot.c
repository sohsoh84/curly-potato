#include "dotcupot.h"
#include "constants.h"
#include "paths.h"
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* dotCupotPath(char *path_) { // TODO: it only goes until user path
        char* path = strdup(path_);
        while (isSubdirectory(userHomePath(), path)) { // TODO: check if it is a directory
                if (fileExists(mergePaths(path, ".cupot")))
                        return mergePaths(path, ".cupot");

                char* tmp = dirName(path);
                strcpy(path, tmp);
        }

        return NULL;
}

char *projectPath(char *path) {
        char* tmp = dotCupotPath(path);
        if (tmp == NULL) return NULL;
        return dirName(tmp);
}

char *stagingAreaPath(char *path) {
        return mergePaths(dotCupotPath(path), STAGE_NAME);
}

char *projectName(char* path) {
        char* tmp = projectPath(path);
        if (!tmp) return NULL;
        return basename(tmp);
}

void releativePath(char *path, char* destination) {
        char file_path[PATH_MAX];
        realpath(path, file_path);
        suffixPath(destination, file_path, dirName(projectPath(file_path)));
}
