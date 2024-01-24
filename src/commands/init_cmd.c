#include "init_cmd.h"
#include "../paths.h"
#include "../dotcupot.h"
#include <stdio.h>

int initCommand(int argc, char *argv[]) {
        if (!isSubdirectory(userHomePath(), cwdPath())) {
                perror("you should be in your User path or one of it's subdirectories to run this command");
                return 1;
        }

        return 0;
}