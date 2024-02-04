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
#include "commands/merge.h"
#include "commands/precommit.h"
#include "commands/stash.h"
#include "commands/tree.h"
#include "dotcupot.h"
#include "paths.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int alias(char* alias_name, int argc, char* argv[]) {
        char* result = tryReadConfigEntry(alias_name, globalAliasPath());
        char* tmp = tryReadConfigEntry(alias_name, localAliasPath());
        if (tmp != NULL) result = tmp;

        if (result == NULL) {
                fprintf(stderr, "Alias '%s' not found\n", alias_name);
                return 1;
        }

        char** new_argv = (char**) malloc(sizeof(char*) * 32);
        new_argv[0] = argv[0];
        int new_argc = 1;
        
        char* tok = strtok(result, " \t\n");
        while (tok) {
                new_argv[new_argc++] = strdup(tok);
                tok = strtok(NULL, " \t\n");
        }

        for (int i = 2; i < argc; i++)
                new_argv[new_argc++] = strdup(argv[i]);
        
        return runCommand(new_argc, new_argv);
}

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
        else if (!strcmp(argv[1], "merge")) command_func = mergeCommand;
        else if (!strcmp(argv[1], "pre-commit")) command_func = precommitCommand;
        else if (!strcmp(argv[1], "stash")) command_func = stashCommand;
        else if (!strcmp(argv[1], "tree")) command_func = treeCommand;
        else if (!strncmp(argv[1], "alias.", 6)) {
                return alias(argv[1], argc, argv);
        } else {
                fprintf(stderr, "Invalid Command!\n");
                return 1;
        }

        return command_func(argc - 2, argv + 2);
}