#include "stash.h"
#include "checkout.h"
#include "diff.h"
#include "../stash.h"
#include "../vcs.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../constants.h"
#include "../syscalls.h"
#include "../tracker.h"
#include "../branch.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int applyStash(int index) {
        if (index >= stashSize()) {
                printf("Invalid stash index %d\n", index);
                return 1;
        }

        if (stashDiff(1, stdout, index)) {
                printf("There are conflicts in stash %d\n", index);
                return 1;
        }

        char** result;
        int count = allTrackedFiles(stashTrackerPath(index), &result);
        for (int i = 0; i < count; i++) {
                if (!fileExists(dirName(result[i])))
                        createRequiredDirectories(result[i]);

                char* rel_path = trackRelativePath(result[i]);
                char* full_path = mergePaths(mergePaths(getStashDirPath(index), projectName(cwdPath())), rel_path);
                copyDirWithName(full_path, rel_path);
        }

        popStashDir(index);
        printf("Stash Applied Successfully\n");    
        return 0;   
}

int stashCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, RED "You should be inside a cupot repository to run this command\n" RESET "try running cupot init\n");
                return 1;
        }

        if (argc == 0) {
                printf("Invalid usage of stash command\n");
                return 1;
        }

        if (!strcmp(argv[0], "push")) {
                char message[MAX_COMMIT_MESSAGE_LEN] = "";
                if (argc != 1) {
                        if (argc == 3 && !strcmp(argv[1], "-m")) {
                                strcpy(message, argv[2]);
                        } else {
                                printf("Invalid usage of stash command\n");
                                return 1;
                        }
                }

                StashConfigs* configs = createStashConfigs(getCWB(), message);
                addStashDir();
                copyDirWithName(projectPath(cwdPath()), mergePaths(getStashDirPath(0), projectName(cwdPath()))); 
                removeFileDir(mergePaths(mergePaths(getStashDirPath(0), projectName(cwdPath())), ".cupot"));
                addRecursivlyToTrackedFile(mergePaths(getStashDirPath(0), "tracker"), projectPath(cwdPath()));
                writeConfigsToStashTop(configs);
                printf("Changes Stashed Successfully\n");
                checkout(getHead(getCWB()) -> id, 0);
                return 0;
        } else if (!strcmp(argv[0], "list")) {
                int count = stashSize();
                for (int i = 0; i < count; i++) {
                        StashConfigs* configs = readStashConfigs(i);
                        printf("Stash %d", i);
                        if (strlen(configs->message)) printf(": %s\n", configs->message);
                        else printf("\n");
                }

                return 0;
        } else if (!strcmp(argv[0], "show")) {
                int index = 0;
                if (argc != 2 || sscanf(argv[1], "%d", &index) != 1) {
                        printf("Invalid usage of stash command\n");
                        return 0;
                }

                stashDiff(0, stdout, index);
        } else if (!strcmp(argv[0], "pop")) {
                int index = 0;
                if (argc == 1 || sscanf(argv[1], "%d", &index) != 1) index = 0;
                applyStash(index);
        } else if (!strcmp(argv[0], "branch")) {
                if (argc == 1 || !branchExists(argv[1])) {
                        printf("Invalid branch name\n");
                        return 1;
                }

                if (checkout(getHead(argv[1]) -> id, 0)) {
                        printf("Unexpected error happended when swtiching to branch %s\n", argv[1]);
                        return 1;
                }

                printf("Checked out to branch %s\n", argv[1]);
                int index = 0;
                if (argc == 2 || sscanf(argv[2], "%d", &index) != 1) index = 0;
                applyStash(index);
        } else if (!strcmp(argv[0], "clear")) {
                while (stashSize()) popStashDir(0);
                printf("Stash Area Clared Successfully\n");
        } else if (!strcmp(argv[0], "drop")) {
                int index = 0;
                if (argc != 2 || sscanf(argv[1], "%d", &index) != 1) index = 0;
                popStashDir(index);
                printf("Dropped Stash %d\n", index);
        } else {
                printf("Invalid usage of stash command\n");
                return 1;
        }

        return 0;
}