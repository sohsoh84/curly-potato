#include "cmdline.h"

#include <time.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
        srand(time(NULL));
        runCommand(argc, argv);
        return 0;
}