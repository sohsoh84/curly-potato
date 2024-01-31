#ifndef TRACKER_H
#define TRACKER_H

int cleanWithTracker(char* base_project_path, char* track_file);

int addTrackedFile(char* track_file, char* path);

int removeTrackedFile(char* track_file, char* path);

int isTracked(char* trackFile, char* path);

char* commitTrackerPath(char* commit_id);
char* stageTrackerPath();

int allTrackedFiles(char* track_path, char*** result);

char* trackRelativePath(char* path);

int addRecursivlyToTrackedFile(char* track_path, char* path);
#endif