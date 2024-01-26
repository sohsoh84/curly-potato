#ifndef DOTCUPOT_H
#define DOTCUPOT_H

// returns null of there is no cupot parent
char* dotCupotPath(char* path);
char* projectPath(char* path);

char* stagingAreaPath(char* path);

char* projectName(char* path);

void releativePath(char* path, char* destination);
#endif