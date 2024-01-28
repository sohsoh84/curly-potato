#include "branch.h"
#include "commits.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>


int branches(char* dest[MAX_BRANCHES]) {
        int configs_cnt = getCommitCounts();
        CommitConfigs** allConfigs = getAllCommitConfigs();
        int result = 0;
        for (int i = 0; i < configs_cnt; i++) {
                int exists = 0;
                for (int j = 0; j < result; j++)
                        if (!strcmp(allConfigs[i]->branch_name, allConfigs[j]->branch_name))
                                exists = 1;
                
                if (!exists) {
                        dest[result] = (char*) malloc(strlen(allConfigs[i]->branch_name) + 4);
                        strcpy(dest[result], allConfigs[i]->branch_name);
                        result++;
                }
        }

        return result;
}

int branchExists(char* branch_name) {
        char* all_branches[MAX_BRANCHES];
        int branch_count = branches(all_branches);
        for (int i = 0; i < branch_count; i++)
                if (!strcmp(all_branches[i], branch_name))
                        return 1;
        
        return 0;
}
