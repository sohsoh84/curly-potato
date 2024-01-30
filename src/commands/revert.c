#include "revert.h"
#include "commit.h"
#include "checkout.h"

#include "../commits.h"
#include "../vcs.h"
#include "../paths.h"
#include "../staging_area.h"
#include "../dotcupot.h"
#include "../constants.h"
#include <stdio.h>
#include <string.h>

CommitConfigs* readCommit(char* commit_id) {
        if (strncmp(commit_id, "HEAD-", 5) == 0) {
                CommitConfigs* configs = getCommitConfigs(getCWC());
                int n = 0;

                if (sscanf(commit_id, "HEAD-%d", &n) != 1) return NULL;
                while (n--) {
                        if (strlen(configs->parent_id) == 0) return NULL;
                        configs = getCommitConfigs(configs->parent_id);
                }

                return configs;
        }

        return getCommitConfigs(commit_id);
}

int revertCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, "you should be inside a cupot repository\n");
                return 1;
        }

        if (argc == 0 || argc > 3) {
                printf("too few arguments\n");
                return 1;
        }

        CommitConfigs *configs = NULL;
        char commit_message[MAX_COMMIT_MESSAGE_LEN] = "\0";
        int hard_revert = 1;
        
        if (argc == 1) configs = readCommit(argv[0]);
        else if (argc == 2) {
                if (strcmp(argv[0], "-n")) {
                        printf("invalid argument\n");
                        return 1;
                }

                hard_revert = 0;
                configs = readCommit(argv[1]);
        } else {
                if (strcmp(argv[0], "-m")) {
                        printf("invalid argument\n");
                        return 1;
                }

                configs = readCommit(argv[2]);
                strcpy(commit_message, argv[1]);
        }

        if (!configs) {
                printf("Invalid commit id\n");
                return 1;
        }


        if (strlen(configs->merge_id)) {
                fprintf(stderr, "you can't revert to a merge commit\n");
                return 1;
        }
        
        if (!strlen(commit_message)) 
                strcpy(commit_message, configs->message);

        
        checkout(configs->id, 1);
        if (hard_revert) {
                buildProjectFromCommit(stagingAreaPath(cwdPath()), configs->id);
                char* args[2];
                args[0] = strdup("-m");
                args[1] = commit_message;
                commitCommand(2, args);
        }

        printf("Revert Completed!\n");
        return 0;
}