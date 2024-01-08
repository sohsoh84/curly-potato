#ifndef PATHS_H
#define PATHS_H

char* userHomePath(); // TODO: memory management
char* cwdPath(); // TODO: memory management
char* mergePaths(const char* path1, const char* path2);
char* globalConfigPath();
char* globalAliasPath();
int fileExists(const char* filePath);


#endif