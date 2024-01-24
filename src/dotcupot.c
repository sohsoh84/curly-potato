#include "dotcupot.h"
#include "paths.h"
#include <string.h>
#include <stdio.h>

char* dotCupotPath(char *path) { // TODO: it only goes until user path
        while (isSubdirectory(userHomePath(), path)) { // TODO: check if it is a directory
                if (fileExists(mergePaths(path, ".cupot")))
                        return mergePaths(path, ".cupot");

                const char* tmp = dirName(path);
                strcpy(path, tmp);
        }

        return NULL;
}