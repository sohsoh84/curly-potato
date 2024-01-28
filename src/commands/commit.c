#include "commit.h"
#include "../dotcupot.h"
#include "../commits.h"
#include "../paths.h"
#include "../staging_area.h"
#include "../configs.h"
#include "../vcs.h"

#include <stdio.h>
#include <string.h>

int commitCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, "you should be in a cupot repository\n");
                return 1;
        }

        if (argc != 2) {
                fprintf(stderr, "Wrong format: cupot commit -m \"message\"");
                return 1;
        }

        if (!(!strcmp(argv[0], "-m") || !strcmp(argv[0], "-s"))) {
                fprintf(stderr, "Wrong format: cupot commit -m \"message\"");
                return 1;
        }

        char* name = dotCupotConfigEntry("user.name");
        char* email = dotCupotConfigEntry("user.email");

        if (!name) {
                fprintf(stderr, "you should set user.name variable!\n");
                return 1;
        }

        if (!email) {
                fprintf(stderr, "you should set user.email variable!\n");
                return 1;
        }

        if (isDirectoryEmpty(stagingAreaPath(cwdPath()))) {
                fprintf(stderr, "staging area is empty, nothing to commit!\n");
                return 1;
        }

        char commit_message[MAX_COMMIT_MESSAGE_LEN];
        strcpy(commit_message, argv[1]);
        if (!strcmp(argv[0], "-s")) {
                commit_message[0] = '\0';
                FILE *file = fopen(commitMessageAliasPath(cwdPath()), "r");
                if (file) {
                        Config* config = createConfig();
                        readConfigFile(file, config);
                        char* tmp = getEntry(config, argv[1]);
                        strcpy(commit_message, tmp);
                }

                if (!strlen(commit_message)) {
                        fprintf(stderr, "Couldn't find commit shortcut message\n");
                        return 1;
                }
        }

        if (strlen(commit_message) > 72) {
                fprintf(stderr, "commit message is too long\n");
                return 1;
        }

        CommitConfigs* config = createCommitConfigs(getHead() -> id, getHead()->branch_name, 
                commit_message, name, email);
        commit(mergePaths(stagingAreaPath(cwdPath()), projectName(cwdPath())), config);
        clearStageingAreas();
        printf(GREEN "Commited successfully!\n" RESET "Commit id: %s\nCommit time: %s\n", config -> id, config -> time);

        return 0;
}

int setCommand(int argc, char *argv[]) {
        if (argc != 4) {
                printf("Usage: cupot set -m \"message\" -s shortcut-name\n");
                return 1;
        }

        if (strcmp(argv[0], "-m") || strcmp(argv[2], "-s")) {
                printf("Usage: cupot set -m \"message\" -s shortcut-name\n");
                return 1; 
        }

        if (strchr(argv[3], ' ')) {
                printf("the shortcut name should not contain spaces\n");
                return 1;
        }

        FILE *file = fopen(commitMessageAliasPath(cwdPath()), "r");
        Config* config = createConfig();
        if (file) readConfigFile(file, config);
        fclose(file);

        if (getEntry(config, argv[3])) {
                fprintf(stderr, "\"%s\" is already set!\n", argv[3]);
                return 1;
        }

        editEntry(config, argv[3], argv[1]);
        file = fopen(commitMessageAliasPath(cwdPath()), "w");
        writeConfigFile(file, config);
        fclose(file);
        return 0;
}

int replaceCommand(int argc, char *argv[]) {
        if (argc != 4) {
                printf("Usage: cupot replace -m \"message\" -s shortcut-name\n");
                return 1;
        }

        if (strcmp(argv[0], "-m") || strcmp(argv[2], "-s")) {
                printf("Usage: cupot replace -m \"message\" -s shortcut-name\n");
                return 1; 
        }

        if (strchr(argv[3], ' ')) {
                printf("the shortcut name should not contain spaces\n");
                return 1;
        }

        FILE *file = fopen(commitMessageAliasPath(cwdPath()), "r");
        Config* config = createConfig();
        if (file) readConfigFile(file, config);
        fclose(file);

        if (!getEntry(config, argv[3])) {
                fprintf(stderr, "\"%s\" is not a valid\n", argv[3]);
                return 1;
        }

        editEntry(config, argv[3], argv[1]);
        file = fopen(commitMessageAliasPath(cwdPath()), "w");
        writeConfigFile(file, config);
        fclose(file);
        return 0;
}

int removeCommand(int argc, char *argv[]) {
        if (argc != 2) {
                fprintf(stderr, "Usage: cupot remove -s shortcut-name\n");
                return 1;
        }

        if (strcmp(argv[0], "-s")) {
                fprintf(stderr, "Usage: cupot remove -s shortcut-name\n");
                return 1;
        }


        FILE *file = fopen(commitMessageAliasPath(cwdPath()), "r");
        Config* config = createConfig();
        if (file) readConfigFile(file, config);
        fclose(file);

         if (!removeEntry(config, argv[1])) {
                fprintf(stderr, "\"%s\" is not a valid shortcut\n", argv[1]);
                return 1;
        }

        file = fopen(commitMessageAliasPath(cwdPath()), "w");
        writeConfigFile(file, config);
        fclose(file);
        return 0;
}