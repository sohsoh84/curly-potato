#include "paths.h"
#include <stdio.h>
#include <stdlib.h>
#include "syscalls.h"

int createRequiredDirectories(char* path) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "mkdir -p %s", dirName(path));
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to create directory %s\n", path);
                return 1;
        }

        return 0;
}

// TODO: keep permissions

int removeFileDir(char* path) {
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

int copyFileDir(char* src, char* dest) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "cp -r %s %s", src, dirName(dest));
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to copy %s to %s\n", src, dest);
                return 1;
        }

        return 0;
} 

int copyDirWithName(char *src, char *dest) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "cp -R %s %s", src, dest);
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to copy -R %s to %s\n", src, dest);
                return 1;
        }
        
        return 0;
}

int copyDirWithoutOverwrite(char *src, char *dest) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "cp -rpn %s %s", src, dest);
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to copy -R %s to %s\n", src, dest);
                return 1;
        }

        return 0;
}