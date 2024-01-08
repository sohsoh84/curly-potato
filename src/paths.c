#include <paths.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"

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

char* mergePaths(const char* path1, const char* path2) {
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

