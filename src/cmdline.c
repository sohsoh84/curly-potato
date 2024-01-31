#include "cmdline.h"

#include "commands/cupot.h"
#include "commands/config.h"
#include "commands/init.h"
#include "commands/add.h"
#include "commands/reset.h"
#include "commands/commit.h"
#include "commands/status.h"
#include "commands/log.h"
#include "commands/branch.h"
#include "commands/checkout.h"
#include "commands/revert.h"
#include "commands/tag.h"
#include "commands/grep.h"
#include "commands/diff.h"
#include <string.h>
#include <stdio.h>

int runCommand(int argc, char* argv[]) {
        int (*command_func) (int, char*[]) = NULL;
        if (argc < 2) command_func = cupotCommand;
        else if (!strcmp(argv[1], "config")) command_func = configCommand;
        else if (!strcmp(argv[1], "init")) command_func = initCommand;
        else if (!strcmp(argv[1], "add")) command_func = addCommand;
        else if (!strcmp(argv[1], "reset")) command_func = resetCommand;
        else if (!strcmp(argv[1], "commit")) command_func = commitCommand;
        else if (!strcmp(argv[1], "set")) command_func = setCommand;
        else if (!strcmp(argv[1], "replace")) command_func = replaceCommand;
        else if (!strcmp(argv[1], "remove")) command_func = removeCommand;
        else if (!strcmp(argv[1], "status")) command_func = statusCommand;
        else if (!strcmp(argv[1], "log")) command_func = logCommand;
        else if (!strcmp(argv[1], "branch")) command_func = branchCommand;
        else if (!strcmp(argv[1], "checkout")) command_func = checkoutCommand;
        else if (!strcmp(argv[1], "revert")) command_func = revertCommand;
        else if (!strcmp(argv[1], "tag")) command_func = tagCommand;
        else if (!strcmp(argv[1], "grep")) command_func = grepCommand;
        else if (!strcmp(argv[1], "diff")) command_func = diffCommand;
        else {
                fprintf(stderr, "Invalid Command!\n");
                return 1;
        }

        return command_func(argc - 2, argv + 2);
}