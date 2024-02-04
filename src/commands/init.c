#include "init.h"
#include "../paths.h"
#include "../dotcupot.h"
#include "../commits.h"
#include "../configs.h"
#include "../constants.h"
#include "../tracker.h"

#include <stdio.h>

char* createInitCommit(char* name, char* email) {
        CommitConfigs* configs = createCommitConfigs("", MASTER_BRANCH_NAME, "cupot Init Commit :')", name, email);
        emptyCommit(configs);
        return configs->id;
}

int initCommand(int argc, char *argv[]) {
        if (!isSubdirectory(userHomePath(), cwdPath())) {
                fprintf(stderr, "you should be in your User path or one of it's subdirectories to run this command\n");
                return 1;
        }

        if (dotCupotPath(cwdPath()) != NULL) {
                fprintf(stderr, "there is already a cupot repository in this directory or one of it's parents!\n");
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


        makeDirectory(".cupot");
        makeDirectory(".cupot/staging_area");
        makeDirectory(mergePaths(".cupot/staging_area", projectName(cwdPath())));
        makeDirectory(".cupot/commits");
        makeDirectory(".cupot/tags");
        makeDirectory(".cupot/stash");

        FILE* file = fopen(stageTrackerPath(), "w");
        fclose(file);

        char* commit_id = createInitCommit(name, email);
        writeCWC(commit_id);
        printf("cupot repository initialized successfully!\n");
        return 0;
}