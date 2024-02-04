#include "checkout.h"
#include "status.h"
#include "../vcs.h"
#include "../dotcupot.h"
#include "../syscalls.h"
#include "../paths.h"
#include "../branch.h"
#include "../constants.h"
#include "../staging_area.h"
#include "../tracker.h"

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
int checkout(char* commit_id, int revert_state) {
        clearStageingAreas(commit_id);
        if (fileExists(stageTrackerPath()))
                removeFileDir(stageTrackerPath());
        copyDirWithName(commitTrackerPath(commit_id), stageTrackerPath());

        chdir(projectPath(cwdPath()));
        buildProjectFromCommit(".tmp_project", commit_id);

        DIR *dir = opendir(".");
        if (dir == NULL) {
                printf("Error: Cannot open directory in checkout\n");
                return 1;
        }

        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }

                char filePath[PATH_MAX];
                snprintf(filePath, sizeof(filePath), "%s/%s", ".", ent -> d_name);

                if (!strcmp(basename(filePath), ".cupot") || !strcmp(basename(filePath), ".tmp_project")) continue;
                if (ent -> d_type == DT_DIR) {
                        removeFileDir(filePath);
                } else if (ent -> d_type == DT_REG) {
                        removeFileDir(filePath);
                }
        }

        closedir(dir);
        
        char* path = mergePaths(".tmp_project", projectName(cwdPath()));
        dir = opendir(path);
        if (dir == NULL) {
                printf("Error: Cannot open directory of proj in checkout\n");
                return 1;
        }

        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }

                char filePath[PATH_MAX];
                snprintf(filePath, sizeof(filePath), "%s/%s", path, ent -> d_name);

                if (ent -> d_type == DT_DIR || ent -> d_type == DT_REG) {
                        char real_path[MAX_PATH_LEN];
                        realpath(filePath, real_path);
                        
                        char proj_path[MAX_PATH_LEN];
                        suffixPath(proj_path, real_path, mergePaths(projectPath(cwdPath()), path));
                        copyFileDir(real_path, proj_path);
                }
        }

        removeFileDir(".tmp_project");
        closedir(dir);
  
        if (!revert_state) writeCWC(commit_id);
        return 0;
}

int checkoutCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, RED "You should be inside a cupot repository to run this command\n" RESET "try running cupot init\n");
                return 1;
        }

        if (strcmp(cwdPath(), projectPath(cwdPath()))) {
                printf("you should be exactly inside your cupot repository to checkout\n");
                return 1;
        }

        if (checkIfUncommitedFiles() && !(argc == 2 && !strcmp(argv[1], "--force"))) {
                printf("you have uncommitted files\n");
                return 1;
        }

        char* all_brances[MAX_BRANCHES];
        int branch_cnt = branches(all_brances);

        for (int i = 0; i < branch_cnt; i++) {
                if (!strcmp(all_brances[i], argv[0])) {
                        return checkout(getHead(all_brances[i])->id, 0);
                }
        }

        int commit_cnt = getCommitCounts();
        char** commit_ids = getAllCommitIDs();
        for (int i = 0; i < commit_cnt; i++) {
                if (!strcmp(commit_ids[i], argv[0])) {
                        return checkout(commit_ids[i], 0);
                }
        }

        if (!strcmp(argv[0], "HEAD")) {
                return checkout(getHead(getCWB())->id, 0);
        }

        if (!strncmp(argv[0], "HEAD-", 5)) {
                int lvl;
                sscanf(argv[0], "HEAD-%d", &lvl);

                CommitConfigs* config = getHead(getCWB());
                while (lvl--) {
                        if (!strlen(config->parent_id)) {
                                printf("n is too much(more than height) \n");
                                return 1;
                        }

                        config = getCommitConfigs(config->parent_id);
                }

                return checkout(config->id, 0);
        }

        fprintf(stderr, "Invalid commit/branch/arguments: %s\n", argv[0]);
        return 0;
}
