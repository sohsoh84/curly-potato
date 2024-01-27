#include "cmdline.h"

#include "commands/cupot.h"
#include "commands/config.h"
#include "commands/init.h"
#include "commands/add.h"
#include <string.h>
#include <stdio.h>

int runCommand(int argc, char* argv[]) {
        int (*command_func) (int, char*[]) = NULL;
        if (argc < 2) command_func = cupotCommand;
        else if (!strcmp(argv[1], "config")) command_func = configCommand;
        else if (!strcmp(argv[1], "init")) command_func = initCommand;
        else if (!strcmp(argv[1], "add")) command_func = addCommand;
        else {
                fprintf(stderr, "Invalid Command!\n");
                return 1;
        }

        return command_func(argc - 2, argv + 2);
}