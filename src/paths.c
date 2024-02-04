#include "paths.h"
#include "constants.h"
#include "dotcupot.h"

#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>

char *userHomePath() {
        return getpwuid(getuid()) -> pw_dir;
}

char* BaseName(char* name) {
        char *copy = strdup(name);
        return basename(copy);
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

char* dirName(const char* path_) {
        char* pathCopy = strdup(path_);
        if (pathCopy == NULL) {
                perror("Error copying path");
                return NULL;
        }

        char* parentDir = dirname(pathCopy);
        free(pathCopy);

        return parentDir;
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
                // perror("Error resolving path1!");
                return -1;
        }

        if (realpath(childPath, realPath2) == NULL) {
                // perror("Error resolving path2!");
                return -1;
        }

        int n = strlen(realPath1);
        if (strlen(realPath1) > strlen(realPath2)) return 0;
        return strncmp(realPath1, realPath2, n) == 0 && (realPath2[n] == '/' || realPath2[n] == '\0');
}

int isSubdirectoryUnsafe(const char* realPath1, const char* realPath2) {
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

int suffixPath(char *destination, char *path, char *prefix) {
        char real_path[MAX_PATH_LEN], real_prefix[MAX_PATH_LEN];
        realpath(path, real_path);
        realpath(prefix, real_prefix);

        if (strcmp(real_path, real_prefix) == 0) {
                strcpy(destination, ".");
                return 0;
        }

        int len = strlen(real_prefix);
        real_prefix[len + 1] = '\0';
        real_prefix[len] = '/';

        if (strncmp(real_path, real_prefix, len + 1) == 0) {
                strcpy(destination, real_path + len + 1);
                return 0;
        }

        perror("prefix_path isn't a prefix of real_path"); 
        return 1;
}

int isDirectory(char *path) {
        struct stat statbuf;
        if (stat(path, &statbuf) != 0)
                return 0;
        
        return S_ISDIR(statbuf.st_mode);
}

int isDirectoryEmpty(char *path) {
        DIR *dir = opendir(path);
        if (!dir) {
                return 1;
        }

        struct dirent *entry;
        int count = 0;

        while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        count++;
                }
        }

        closedir(dir);
        return count == 0;
}

char *relativePathString(char *path, char* cwd) {
        char* result = (char*) calloc(MAX_PATH_LEN, sizeof(char));
        while (!isSubdirectory(cwd, path)) {
                cwd = dirName(cwd);
                strcat(result, "../");
        }

        strcat(result, path + strlen(cwd) + (path[strlen(cwd)] == '/' ? 1 : 0));
        return result;
}

char *relativePathStringUnsafe(char *path, char *cwd) {
        char* result = (char*) calloc(MAX_PATH_LEN, sizeof(char));
        while (!isSubdirectoryUnsafe(cwd, path)) {
                cwd = dirName(cwd);
                strcat(result, "../");
        }

        strcat(result, path + strlen(cwd) + (path[strlen(cwd)] == '/' ? 1 : 0));
        return result;
}
