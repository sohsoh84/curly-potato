#ifndef PATHS_H
#define PATHS_H

#define PATH_MAX        2048

char* userHomePath(); // TODO: memory management
char* cwdPath(); // TODO: memory management
char* mergePaths(const char* path1, const char* path2);
char* globalConfigPath();
char* globalAliasPath();
int fileExists(const char* filePath);
char* dirName(const char* path);
int arePathsEqual(const char* path1, const char* path2);
int isSubdirectory(const char* parPath, const char* childPath);

char* localConfigPath(); // TODO: memory management
char* localAliasPath(); // TODO: memory management

int makeDirectory(char* dirName); // TODO: it should be in another file

int suffixPath(char* destination, char* path, char* prefix);

int isDirectory(char* path);

int isDirectoryEmpty(char* path);

#endif