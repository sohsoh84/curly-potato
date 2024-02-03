#include "tag.h"
#include "../tag.h"
#include "../dotcupot.h"
#include "../paths.h"
#include "../commits.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_tags(const void* a, const void* b) {
        Tag* ap = (Tag*) a;
        Tag* bp = (Tag*) b;

        return strcmp(ap->tag_id, bp->tag_id);
}

void printTag(Tag* tag) {
        printf(YELLOW "Tag: %s\n" RESET, tag->tag_id);
        printf("Commit: %s\n", tag->commit_id);
        printf("Author: %s <%s>\n", tag->author_name, tag->author_email);
        strip(tag->date);
        printf("Date: %s\n", tag->date);
        if (strlen(tag->message)) printf("Message: %s\n", tag->message);
}

int tagCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                printf("you should be in a cupot repository\n");
                return 1;
        }

        if (argc == 0) {
                Tag** all_tags;
                int count = getAllTags(&all_tags);
                if (count == 0) {
                        printf("No tags found\n");
                        return 0;
                }


                qsort(all_tags, count, sizeof(Tag*), compare_tags);
                for (int i = 0; i < count; i++)
                        printf("%s\n", all_tags[i]->tag_id);

                return 0;
        } else if (!strcmp(argv[0], "show")) {
                if (argc != 2) {
                        printf("Usage: cupot tag show <tag_id>\n");
                        return 1;
                }

                Tag* tag = getTag(argv[1]);
                if (tag == NULL) {
                        printf("Tag not found\n");
                        return 1;
                }

                printTag(tag);
                return 0;
        }

        if (strcmp(argv[0], "-a") || argc < 2) {
                printf("Usage: cupot tag -a <tag-name> [-m <message>] [-c <commit-id>] [-f]");
                return 1;
        }

        char* tag_name = argv[1];
        int forced = 0;
        char* commit_id = getCWC();
        char message[MAX_COMMIT_MESSAGE_LEN];
        message[0] = '\0';

        int arg_ptr = 2;
        while (arg_ptr < argc) {
                if (!strcmp(argv[arg_ptr], "-f")) {
                        forced = 1;
                        arg_ptr++;
                } else if (!strcmp(argv[arg_ptr], "-m")) {
                        if (arg_ptr == argc - 1) {
                                printf("Usage: cupot tag -a <tag-name> [-m <message>] [-c <commit-id>] [-f]");
                                return 1;
                        }

                        strcpy(message, argv[arg_ptr + 1]);
                        arg_ptr += 2;
                } else if (!strcmp(argv[arg_ptr], "-c")) {
                        if (arg_ptr == argc - 1) {
                                printf("Please enter a valid commit id\n");
                                return 1;
                        }

                        CommitConfigs* configs = getCommitConfigs(argv[arg_ptr + 1]);
                        if (configs == NULL) {
                                printf("Invalid commit id\n");
                                return 1;
                        }

                        commit_id = configs->id;
                        arg_ptr += 2;
                } else {
                        printf("Usage: cupot tag -a <tag-name> [-m <message>] [-c <commit-id>] [-f]");
                        return 1;
                }
        }

        if (tagExists(tag_name) && !forced) {
                printf("Tag already exists, use -f to overwrite it\n");
                return 1;
        }

        char* name = dotCupotConfigEntry("user.name");
        char* email = dotCupotConfigEntry("user.email");

        if (!name) {
                fprintf(stderr, "you should set user.name variable!\n");
                return 1;
        }

        if (!email) {
                fprintf(stderr, "you should set user.email variable!\n");
                return 1;
        }


        Tag* tag = createTag(tag_name, commit_id, message, name, email);
        if (addTag(tag)) printf("unexpected error creating tag: %s\n", tag_name);
        else printf("tag added successfully\n");
        return 0;
}