#ifndef PATHS_H
#define PATHS_H

#define PATH_MAX        2048

char* userHomePath();
char* cwdPath(); 
char* mergePaths(const char* path1, const char* path2);
char* globalConfigPath();
char* globalAliasPath();
int fileExists(const char* filePath);
char* dirName(const char* path);
int arePathsEqual(const char* path1, const char* path2);
int isSubdirectory(const char* parPath, const char* childPath);

int isSubdirectoryUnsafe(const char* patPath, const char* childPath);

char* localConfigPath(); 
char* localAliasPath(); 

int makeDirectory(char* dirName);

int suffixPath(char* destination, char* path, char* prefix);

int isDirectory(char* path);

int isDirectoryEmpty(char* path);

char* relativePathString(char* path, char* cwd);
char* relativePathStringUnsafe(char* path, char* cwd);

char* BaseName(char* path);
#endif