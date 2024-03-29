#ifndef SYSCALLS_H
#define SYSCALLS_H

int createRequiredDirectories(char* path);
int removeFileDir(char* path);
int copyFileDir(char* src, char* dest);

int copyDirWithName(char* src, char* dest);

int copyDirWithoutOverwrite(char *src, char *dest);

int getVideoDuration(char* path); // Copy pasted command :) also uses ffmpeg
#endif