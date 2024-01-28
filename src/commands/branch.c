#include "branch.h"
#include "../branch.h"
#include "../constants.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../commits.h"

#include <stdio.h>

int branchCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                printf("you should be inside a cupot repository\n");
                return 1;
        }

        if (argc == 0) {
                char* all_brances[MAX_BRANCHES];
                int branch_cnt = branches(all_brances);

                printf("All branches: \n");
                for (int i = 0; i < branch_cnt; i++)
                        printf("\t%s\n", all_brances[i]);
       
                return 0;
        }

        if (argc != 1) {
                printf("too much arguments\n");
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

        char commit_message[MAX_COMMIT_MESSAGE_LEN];
        snprintf(commit_message, MAX_COMMIT_MESSAGE_LEN, "Created branch <%s>", argv[0]);
        
        CommitConfigs* configs = createCommitConfigs(getHead(), argv[0], commit_message, name, email);
        emptyCommit(configs); 
        return 0;
}