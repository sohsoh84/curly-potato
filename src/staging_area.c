#include "staging_area.h"
#include "syscalls.h"
#include "paths.h"
#include "constants.h"
#include "diff.h"
#include "dotcupot.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int removeStage(char *stageName) {
        return removeFileDir(mergePaths(dotCupotPath(cwdPath()), stageName));
}

int copyStage(char* sourceStageName, char* destStageName) {
        return copyDirWithName(mergePaths(dotCupotPath(cwdPath()), sourceStageName), 
                mergePaths(dotCupotPath(cwdPath()), destStageName));
}

char* ith_backup_stage(int i) {
        char* destination = (char*) malloc(sizeof(char) * PATH_MAX);
        snprintf(destination, PATH_MAX, "%s.%c", BACKUP_STAGE_NAME, (char)(i + 'a'));
        return destination;
}

void backupStageArea() {
        for (int i = STAGING_AREA_BACKUP_SIZE - 1; i >= 0; i--) {
                if (fileExists(mergePaths(dotCupotPath(cwdPath()), ith_backup_stage(i)))) {
                        if (i < STAGING_AREA_BACKUP_SIZE - 1) 
                                copyStage(ith_backup_stage(i), ith_backup_stage(i + 1));
                }

                removeStage(ith_backup_stage(i));
        }


        copyStage(STAGE_NAME, ith_backup_stage(0));
}

int undoBackupStagingArea() {
        if (!fileExists(mergePaths(dotCupotPath(cwdPath()), ith_backup_stage(0)))) return 1;
        removeStage(STAGE_NAME);
        copyStage(ith_backup_stage(0), STAGE_NAME);
        for (int i = 0; i < STAGING_AREA_BACKUP_SIZE; i++) {
                removeStage(ith_backup_stage(i));
                if (i < STAGING_AREA_BACKUP_SIZE - 1 && fileExists(mergePaths(dotCupotPath(cwdPath()), ith_backup_stage(i + 1))))
                         copyStage(ith_backup_stage(i + 1), ith_backup_stage(i));
        }
        
        return 0;
}

char* stageFilePath(char* complete_path) {
        char real_path[MAX_PATH_LEN];
        realpath(complete_path, real_path);

        char path[MAX_PATH_LEN];
        releativePath(real_path, path);
        char* stage_path = stagingAreaPath(cwdPath());
        return mergePaths(stage_path, path);
}

int stagingStatus(char *complete_path) {
        if (!fileExists(complete_path)) return -1;

        char path[MAX_PATH_LEN];
        releativePath(complete_path, path);
        char* stage_path = stagingAreaPath(cwdPath());
        if (!fileExists(mergePaths(stage_path, path))) return 2;
        if (absoluteDiff(mergePaths(stage_path, path), complete_path)) {
                printf("%s\n%s\n", mergePaths(stage_path, path), complete_path);
                return 1;
        }
        return 0;
}

int clearStageingAreas() {
        removeStage(STAGE_NAME);
        for (int i = 0; i < STAGING_AREA_BACKUP_SIZE; i++)
                if (fileExists(mergePaths(dotCupotPath(cwdPath()), ith_backup_stage(i))))
                        removeStage(ith_backup_stage(i));

        makeDirectory(stagingAreaPath(cwdPath()));
        return 0;
}
