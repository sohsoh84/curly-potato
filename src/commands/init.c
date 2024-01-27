#include "init.h"
#include "../paths.h"
#include "../dotcupot.h"
#include <stdio.h>

int initCommand(int argc, char *argv[]) {
        if (!isSubdirectory(userHomePath(), cwdPath())) {
                fprintf(stderr, "you should be in your User path or one of it's subdirectories to run this command\n");
                return 1;
        }

        if (dotCupotPath(cwdPath()) != NULL) {
                fprintf(stderr, "there is already a cupot repository in this directory or one of it's parents!\n");
                return 1;
        }

        makeDirectory(".cupot");
        makeDirectory(".cupot/staging_area");
        printf("cupot repository initialized successfully!\n");
        return 0;
}