#include "vcs.h"
#include "commits.h"
#include "paths.h"
#include "dotcupot.h"
#include "syscalls.h"
#include "tracker.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>

int addFilesFromCommit(char *path, char *commit_id) {
        CommitConfigs* commitConfigs = getCommitConfigs(commit_id);

        if (copyDirWithoutOverwrite(mergePaths(mergePaths(commitsAreaPath(cwdPath()), commit_id), projectName(cwdPath())),
                path)) return 1;


        if (strlen(commitConfigs->parent_id))
                return addFilesFromCommit(path, commitConfigs->parent_id);

        return 0;
}

int buildProjectFromCommit(char* path, char* commit_id) {
        if (fileExists(mergePaths(path, projectName(cwdPath()))))
                if (removeFileDir(mergePaths(path, projectName(cwdPath()))))
                        return 1;
        
        makeDirectory(path);
        if (addFilesFromCommit(path, commit_id)) return 1;

        char* base_path = mergePaths(path, projectName(cwdPath()));
        if (cleanWithTracker(base_path, commitTrackerPath(commit_id))) return 1;
        return removeEmptyDirs(base_path);

}

CommitConfigs *getHead(char* branch) {
        int cnt = getCommitCounts();
        CommitConfigs** all_commits = getAllCommitConfigs();
        for (int i = 0; i < cnt; i++)
                if (!strcmp(branch, all_commits[i]->branch_name))
                        return all_commits[i];

        return NULL;
}
