#include <paths.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include "constants.h"
#include "dotcupot.h"

char *userHomePath() {
        return getpwuid(getuid()) -> pw_dir;
}

char *cwdPath() {
        char* cwd = (char*) malloc(4096);
        if (cwd == NULL || getcwd(cwd, 4096) == NULL) {
                fprintf(stderr, "unexpected error happend on cwdPath()");
                return NULL;
        }

        return cwd;
}

char* mergePaths(const char* path1, const char* path2) { // TODO: use realpath
        char* result = (char*) calloc(strlen(path1) + strlen(path2) + 2, sizeof(char));
        strcpy(result, path1);
        strcat(result, "/");
        strcat(result, path2);

        return result;
}

char* globalConfigPath() {
        return mergePaths(userHomePath(), CONFIG_FILE_NAME);
}

char* globalAliasPath() {
        return mergePaths(userHomePath(), ALIAS_FILE_NAME);
}

int fileExists(const char* filePath) {
        return access(filePath, F_OK) != -1;
}

const char* dirName(const char* path) {
        char* pathCopy = strdup(path);
        if (pathCopy == NULL) {
                perror("Error copying path");
                return NULL;
        }

        char* parentDir = dirname(pathCopy);
        const char* result = strdup(parentDir);
        free(pathCopy);

        return result;
}

int arePathsEqual(const char *path1, const char *path2) {
        char realPath1[MAX_PATH_LEN];
        char realPath2[MAX_PATH_LEN];

        if (realpath(path1, realPath1) == NULL) {
                perror("Error resolving path1!");
                return -1;
        }

        if (realpath(path2, realPath2) == NULL) {
                perror("Error resolving path2!");
                return -1;
        }

        return strcmp(realPath1, realPath2) == 0;
}

int isSubdirectory(const char *parPath, const char *childPath) {
        char realPath1[MAX_PATH_LEN];
        char realPath2[MAX_PATH_LEN];

        if (realpath(parPath, realPath1) == NULL) {
                perror("Error resolving path1!");
                return -1;
        }

        if (realpath(childPath, realPath2) == NULL) {
                perror("Error resolving path2!");
                return -1;
        }

        int n = strlen(realPath1);
        if (strlen(realPath1) > strlen(realPath2)) return 0;
        return strncmp(realPath1, realPath2, n) == 0 && (realPath2[n] == '/' || realPath2[n] == '\0');
}

char* localConfigPath() {
        if (dotCupotPath(cwdPath()) == NULL) return NULL;
        return mergePaths(dotCupotPath(cwdPath()), CONFIG_FILE_NAME);
}

char* localAliasPath() {
        if (dotCupotPath(cwdPath()) == NULL) return NULL;
        return mergePaths(dotCupotPath(cwdPath()), ALIAS_FILE_NAME);
}

int makeDirectory(char* path) {
        return mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}