#include "cmdline.h"

#include "commands/intro_cmd.h"
#include "commands/config_cmd.h"
#include "commands/init_cmd.h"
#include "commands/stage_cmd.h"
#include <string.h>
#include <stdio.h>

int runCommand(int argc, char* argv[]) {
        int (*command_func) (int, char*[]) = NULL;
        if (argc < 2) command_func = introCommand;
        else if (!strcmp(argv[1], "config")) command_func = configCommand;
        else if (!strcmp(argv[1], "init")) command_func = initCommand;
        else if (!strcmp(argv[1], "add")) command_func = stageCommand;
        else {
                fprintf(stderr, "Invalid Command!\n");
                return 1;
        }

        return command_func(argc, argv);
}