#include "dotcupot.h"
#include "paths.h"
#include <libgen.h>
#include <string.h>
#include <stdio.h>

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

char *projectName(char* path) {
        char* tmp = projectPath(path);
        if (!tmp) return NULL;
        return basename(tmp);
}
