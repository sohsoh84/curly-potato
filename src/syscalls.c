#include "syscalls.h"
#include "paths.h"

#include <stdio.h>
#include <stdlib.h>

int createRequiredDirectories(char* path) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "mkdir -p \"%s\"", dirName(path));
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to create directory %s\n", path);
                return 1;
        }

        return 0;
}

int removeFileDir(char* path) {
        if (fileExists(path)) {
                char command[PATH_MAX];
                snprintf(command, sizeof(command), "rm -rf \"%s\"", path);
                if (system(command) == -1) {
                        fprintf(stderr, "something went wrong while trying to remove directory %s\n", path);
                        return 1;
                }

        }

        return 0;
}

int copyFileDir(char* src, char* dest) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "cp -rp \"%s\" \"%s\"", src, dirName(dest));
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to copy %s to %s\n", src, dest);
                return 1;
        }

        return 0;
} 

int copyDirWithName(char *src, char *dest) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "cp -Rp \"%s\" \"%s\"", src, dest);
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to copy -R %s to %s\n", src, dest);
                return 1;
        }
        
        return 0;
}

int copyDirWithoutOverwrite(char *src, char *dest) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "cp -rpn \"%s\" \"%s\"", src, dest);
        if (system(command) == -1) {
                fprintf(stderr, "something went wrong while trying to copy -R %s to %s\n", src, dest);
                return 1;
        }

        return 0;
}

int getVideoDuration(char *path) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "ffmpeg -i \"%s\" 2>&1 | grep \"Duration\"|"\
                " cut -d ' ' -f 4 | sed s/,// | sed 's@\\..*@@g' | awk '{ split($1, A, \":\"); split(A[3], B, \".\"); print 3600*A[1] + 60*A[2] + B[1] }'", path);

        FILE *fp = popen(command, "r");
        int x;
        fscanf(fp, "%d", &x);
        pclose(fp);
        return x;
}
