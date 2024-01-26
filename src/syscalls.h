#ifndef SYSCALLS_H
#define SYSCALLS_H

int createRequiredDirectories(char* path);
int removeFileDir(char* path);
int copyFileDir(char* src, char* dest);

int copyDirWithName(char* src, char* dest);
#endif