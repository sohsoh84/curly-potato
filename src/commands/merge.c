#include "merge.h"
#include "diff.h"
#include "commit.h"
#include "../dotcupot.h"
#include "../branch.h"
#include "../paths.h"
#include "../vcs.h"
#include "../utils.h"
#include "../tracker.h"
#include "../staging_area.h"
#include "../syscalls.h"
#include "checkout.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mergeCommand(int argc, char *argv[]) {
        char USAGE[] = "Usage: cupot merge -b <branch1> <branch2>\n";
        if (argc != 3) {
                printf("%s", USAGE);
                return 1;
        }

        if (strcmp(argv[0], "-b")) {
                printf("%s", USAGE);
                return 1;
        }

        if (!branchExists(argv[1])) {
                printf("invalid branch name %s\n", argv[1]);
                return 1;
        }
        if (!branchExists(argv[2])) {
                printf("invalid branch name %s\n", argv[2]);
                return 1;
        }

        char* head_commit1 = getHead(argv[1])->id, *head_commit2 = getHead(argv[2])->id;
        
        int diff_res = commitDiff(1, stdout, head_commit1, head_commit2, 0);
        if (diff_res) {
                printf(RED "merge failed due to conflicts\n" RESET "do you want to resolve the conflict[y: yes, n[default]: no]? \n");
                char s[40];
                fgets(s, sizeof(s), stdin);       
                strip(s);         
                if (!strcmp(s, "y")) {
                        if (commitDiff(1, stdout, head_commit1, head_commit2, 1)) return 1;
                } else return 1;
        }

        char* proj_path1 = mergePaths(dotCupotPath(cwdPath()), "temp1");
        char* proj_path2 = mergePaths(dotCupotPath(cwdPath()), "temp2");

        clearStageingAreas(head_commit1);
        char** tracked_paths2;
        int count2 = allTrackedFiles(commitTrackerPath(head_commit2), &tracked_paths2);
        for (int i = 0; i < count2; i++)
                addTrackedFile(stageTrackerPath(), tracked_paths2[i]);

        char commit_message[1024];
        sprintf(commit_message, "Merge %s into %s", argv[2], argv[1]);

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

        if (!diff_res) {
                buildFromMerge(stageTrackerPath(), stagingAreaPath(cwdPath()), head_commit1, head_commit2);
        } else {
                copyDirWithoutOverwrite(mergePaths(proj_path1, projectName(cwdPath())), stagingAreaPath(cwdPath()));
                copyDirWithoutOverwrite(mergePaths(proj_path2, projectName(cwdPath())), stagingAreaPath(cwdPath()));
        }

        CommitConfigs* config = createCommitConfigs(head_commit1, argv[1], 
                commit_message, name, email);
        strcpy(config->merge_id, argv[2]);

        commit(mergePaths(stagingAreaPath(cwdPath()), projectName(cwdPath())), config);
        clearStageingAreas(config -> id);
        printf(GREEN "Merged %s into %s successfully!\n" RESET "Commit id: %s\nCommit time: %s\n", argv[2], argv[1], 
                config -> id, config -> time);
        writeCWC(config->id);
        checkout(config->id, 0);
        printf("you are checkedout to head branch of %s\n", config->branch_name);
        return 0;
}