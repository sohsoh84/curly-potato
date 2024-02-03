#include "tree.h"
#include "../commits.h"
#include "../constants.h"

#include <stdio.h>

int count = 0;
CommitConfigs** configs;

char* firstTwoLetters(char* str) {
        char* result = strdup(str);
        result[2] = '\0';
        return result;
}

int is_par_of_any(CommitConfigs* mas) {
        for (int i = 0; i < count; i++)
                if (!strcmp(configs[i]->parent_id, mas->id) && !strcmp(configs[i]->branch_name, MASTER_BRANCH_NAME))
                        return 1;

        return 0;
}

void dfs(CommitConfigs* mas, CommitConfigs* brn, int new_mas, int new_brn) {
        if (!new_mas && !new_brn) return;
        if (new_mas) printf("%s ", firstTwoLetters(mas->id));
        else {


                printf(is_par_of_any(mas) ? "|  " : "   ");
        }
        if (new_brn) printf("%s ", firstTwoLetters(brn->id));
        else if (brn) printf("|");
        printf("\n");

        CommitConfigs* nex_mas = mas, *nex_brn = brn;
        for (int i = 0; i < count; i++) {
                if (!strcmp(configs[i]->parent_id, mas->id) && !strlen(configs[i]->merge_id) &&
                         !strcmp(configs[i]->branch_name, MASTER_BRANCH_NAME)) {
                        nex_mas = configs[i];
                }

                if (brn && !strcmp(configs[i]->parent_id, brn->id)) {
                        nex_brn = configs[i];
                }
        }

        int branched = 0;
        for (int i = 0; i < count; i++) {
                if (!strcmp(configs[i]->parent_id, mas->id) && !brn && !strlen(configs[i]->merge_id) 
                        && strcmp(configs[i]->branch_name, MASTER_BRANCH_NAME)) {
                        nex_brn = configs[i];
                        branched = 1;
                }
        }

        if (branched) {
                printf(is_par_of_any(mas) ? "|.\\\n" : "  \\\n");
        }

        else if (nex_brn != brn && nex_mas != mas) printf("|  |\n");
        else if (nex_brn != brn) printf(is_par_of_any(mas) ? "|  |\n" : "   |\n");
        else if (nex_mas != mas) printf("|  %s\n", (nex_brn ? "|" : ""));
        else {
                for (int i = 0; i < count; i++) {
                        if (!strcmp(configs[i]->parent_id, mas->id) && brn && !strcmp(configs[i]->merge_id, brn->branch_name)) {
                                printf("|./\n");
                                nex_brn = NULL;
                                nex_mas = configs[i];
                        }
                }
        }

        if (nex_mas == mas && (nex_brn && nex_brn == brn)) return;
        dfs(nex_mas, nex_brn, nex_mas != mas && nex_mas, nex_brn != brn && nex_brn);
}

int treeCommand(int argc, char *argv[]) {
        count = getCommitCounts();
        configs = getAllCommitConfigs();
        
        dfs(configs[count - 1], NULL, 1, 0);
        return 0;
}