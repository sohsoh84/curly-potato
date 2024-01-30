#include "tag.h"
#include "paths.h"
#include "constants.h"
#include "dotcupot.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dirent.h>

Tag *createTag(char *tag_id, char* commit_id, char *message, char *name, char *email) {
        Tag* tag = (Tag*) malloc(sizeof(Tag));
        strcpy(tag->tag_id, tag_id);
        strcpy(tag->commit_id, commit_id);
        strcpy(tag->message, message);
        strcpy(tag->author_name, name);
        strcpy(tag->author_email, email);

        time_t now = time(NULL);
        strcpy(tag->date, ctime(&now));
        return tag;
}

int tagExists(char* tag_id) {
        return fileExists(mergePaths(tagsAreaPath(cwdPath()), tag_id));
}

int addTag(Tag* tag) {
        char* path = mergePaths(tagsAreaPath(cwdPath()), tag->tag_id);
        FILE* fp = fopen(path, "wb");
        if (fp == NULL) return 1;
        fwrite(tag, sizeof(Tag), 1, fp);
        return 0;
}

Tag* getTag(char* tag_id) {
        FILE* fp = fopen(mergePaths(tagsAreaPath(cwdPath()), tag_id), "rb");
        if (fp == NULL) return NULL;
        Tag* tag = (Tag*) malloc(sizeof(Tag));
        fread(tag, sizeof(Tag), 1, fp);

        return tag;
}

int getAllTags(Tag*** result) {
        *result = (Tag**) malloc(sizeof(Tag*) * MAX_TAGS);
        int count = 0;

        DIR *dir;
        struct dirent *dp;

        if ((dir = opendir(tagsAreaPath(cwdPath()))) == NULL) {
                printf("Could not open directory in tag Counts\n");
                return 0;
        }

        while ((dp = readdir(dir)) != NULL) {
                if (dp->d_type == DT_REG) {
                        if (!strcmp(dp -> d_name, ".")) continue;
                        if (!strcmp(dp -> d_name, "..")) continue;

                        (*result)[count] = getTag(dp -> d_name);
                        count++;
                }
        }

        closedir(dir);
        return count;
}
