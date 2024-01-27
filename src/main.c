#include "cmdline.h"
#include "paths.h"
#include "dotcupot.h"
#include "diff.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commits.h"
#include <time.h>
//#include <time.h>


int main(int argc, char* argv[]) {
        srand(time(NULL));
        runCommand(argc, argv);
        return 0;
}