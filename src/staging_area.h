#ifndef STAGING_AREA_H
#define STAGING_AREA_H

#define STAGING_AREA_BACKUP_SIZE                12

int removeStage(char* stageName);
int copyStage(char* sourceStageName, char* destStageName);

void backupStageArea();

int undoBackupStagingArea();

int stagingStatus(char* path); // returns 0 if its in the staging area, 2 if its not, 1 if it is but it changed
#endif