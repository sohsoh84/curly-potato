#ifndef TAG_H
#define TAG_H

#include "constants.h"

typedef struct Tag {
        char tag_id[MAX_COMMIT_MESSAGE_LEN];
        char commit_id[MAX_COMMIT_MESSAGE_LEN];
        char message[MAX_COMMIT_MESSAGE_LEN];
        char author_name[MAX_CONFIG_LENGHT];
        char author_email[MAX_CONFIG_LENGHT];
        char date[MAX_COMMIT_MESSAGE_LEN];

} Tag;


int tagExists(char* tag_id);

int getAllTags(Tag*** result);

int addTag(Tag* tag);
Tag* getTag(char* tag_id);
Tag* createTag(char* tag_id, char* commit_id, char* message, char* name, char* email);

#endif