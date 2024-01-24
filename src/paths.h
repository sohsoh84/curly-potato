#ifndef PATHS_H
#define PATHS_H

char* userHomePath(); // TODO: memory management
char* cwdPath(); // TODO: memory management
char* mergePaths(const char* path1, const char* path2);
char* globalConfigPath();
char* globalAliasPath();
int fileExists(const char* filePath);
const char* dirName(const char* path);
int arePathsEqual(const char* path1, const char* path2);
int isSubdirectory(const char* parPath, const char* childPath);

char* localConfigPath(); // TODO: memory management
char* localAliasPath(); // TODO: memory management

#endif