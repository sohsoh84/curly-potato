#include "branch.h"
#include "../branch.h"
#include "../constants.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../commits.h"
#include "../vcs.h"

#include <stdio.h>
#include <string.h>

int branchCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                printf("you should be inside a cupot repository\n");
                return 1;
        }

        if (argc == 0) {
                char* all_brances[MAX_BRANCHES];
                int branch_cnt = branches(all_brances);

                printf("All branches: \n");
                char* cwb = getCWB();
                for (int i = 0; i < branch_cnt; i++) {
                        int x = (!strcmp(cwb, all_brances[i]));
                        if (x) printf(CYAN), printf("*");
                        printf("\t%s", all_brances[i]);
                        if (x) printf(RESET);
                        printf("\n");
                }
       
                return 0;
        }

        if (argc != 1) {
                printf("too much arguments\n");
                return 1;
        }

        if (getHead(argv[0])) {
                printf("there is already a branch called %s\n", argv[0]);
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
        
        CommitConfigs* configs = createCommitConfigs(getHead(getCWB()) -> id, argv[0], commit_message, name, email);
        emptyCommit(configs); 
        printf(GREEN "Created Branch <%s>\n" RESET, argv[0]);
        return 0;
}