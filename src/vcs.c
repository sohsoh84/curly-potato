#include "vcs.h"
#include "commits.h"
#include "paths.h"
#include "dotcupot.h"
#include "syscalls.h"

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
        if (fileExists(path))
                if (removeFileDir(path))
                        return 1;
        
        makeDirectory(path);
        return addFilesFromCommit(path, commit_id);
}