#ifndef DOTCUPOT_H
#define DOTCUPOT_H

// returns null of there is no cupot parent
char* dotCupotPath(char* path);
char* projectPath(char* path);

char* stagingAreaPath(char* path);
char* commitsAreaPath(char *path);
char* tagsAreaPath(char* path);

char* stashAreaPath(char* path);

char* projectName(char* path);

void releativePath(char* path, char* destination);

char* dotCupotConfigEntry(char* key);

char* commitMessageAliasPath(char* path);

char* getCWB();
char* getCWC();

int writeCWC(char* commit_id);
#endif