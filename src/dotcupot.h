#ifndef DOTCUPOT_H
#define DOTCUPOT_H

// returns null of there is no cupot parent
char* dotCupotPath(char* path);
char* projectPath(char* path);

char* stagingAreaPath(char* path);
char* commitsAreaPath(char *path);

char* projectName(char* path);

void releativePath(char* path, char* destination);

char* dotCupotConfigEntry(char* key);

char* commitMessageAliasPath(char* path);

char* getCWB(); // TODO: implement this shit

int writeCWB(char* branch_name);
#endif