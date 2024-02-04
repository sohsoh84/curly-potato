#include "config.h"
#include "../cmdline.h"
#include "../configs.h"
#include "../paths.h"
#include "../dotcupot.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// TODO: check with new argmuent structure

static int is_alias(const char* key) {
        return strncmp(key, "alias.", 6) == 0;
}

static int is_valid_alias(const char* value_) {
        char* value = strdup(value_);
        char* first_spacee = strchr(value, ' ');
        if (first_spacee != NULL) *first_spacee = '\0';
        for (int i = 0; i < CMD_CNT; i++)
                if (!strcmp(value, ALL_COMMANDS[i]))
                        return 1;

        return 0;
}

int configCommand(int argc, char *argv[]) {
        int reqArgs = 2;
        if (argc < reqArgs) {
                fprintf(stderr, "Not enough arguments!\n");
                return 1;
        }

        bool isGlobal = false;

        int argPtr = 0;
        if (!strcmp(argv[argPtr], "--global")) {
                argPtr++;
                reqArgs++;
                isGlobal = true;
        }

        if (argc < reqArgs) {
                fprintf(stderr, "Not enough arguments!\n");
                return 1;
        } else if (argc > reqArgs) {
                fprintf(stderr, "Too many arguments!\n");
                return 1;
        }

        char* key = argv[argPtr];
        char* value = argv[argPtr + 1];

        if (strchr(key, '=') != NULL || strchr(value, '=') != NULL) {
                fprintf(stderr, "the key and the value shouldn't contain = char!\n");
                return 1;
        }

        // TODO: check if the strings are user.name and user.email
        // TODO: throw some errors bruh :)


        if (is_alias(key) && !is_valid_alias(value)) {
                fprintf(stderr, "Invalid command in alias value!\n");
                return 1;
        }

        char* filePath = NULL;
        if (!isGlobal) {
                if (!dotCupotPath(cwdPath())) {
                        fprintf(stderr, RED "You should be inside a cupot repository to run this command\n" RESET "try running cupot init\n");
                        return 1;
                }

                filePath = (is_alias(key) ? localAliasPath() : localConfigPath()); 
        } else filePath = (is_alias(key) ? globalAliasPath() : globalConfigPath());
        
        struct Config* config = createConfig();
        if (fileExists(filePath)) {
                FILE* file = fopen(filePath, "r");
                if (file == NULL) {
                        perror("Error opening file");    
                        return 1;            
                }

                if (readConfigFile(file, config)) {
                        fclose(file);
                        return 1;
                }

                fclose(file);
        }

        if (editEntry(config, key, value)) return 1;

        FILE* file = fopen(filePath, "w");
        if (file == NULL) {
                perror("Error opening file");   
                return 1;            
        }

                
        if (writeConfigFile(file, config)) {
                fclose(file);
                return 1;
        }

        fclose(file);
        return 0;
}