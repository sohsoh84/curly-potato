#include "cmdline.h"
#include "paths.h"
#include "dotcupot.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
        char ans[PATH_MAX];
        runCommand(argc, argv);
        return 0;
}