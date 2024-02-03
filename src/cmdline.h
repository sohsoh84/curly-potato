#ifndef CMDLINE_H
#define CMDLINE_H

#define CMD_CNT      22
static const char ALL_COMMANDS[CMD_CNT][100] = {
        "config",
        "init",
        "add",
        "reset",
        "add",
        "commit",
        "set",
        "replace",
        "remove",
        "status",
        "log",
        "branch",
        "checkout",
        "revert",
        "tag",
        "grep",
        "diff",
        "merge",
        "pre-commit",
        "stash",
        "tree"
};

int runCommand(int argc, char* argv[]);

#endif