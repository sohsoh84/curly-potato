#include "config.h"
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


        char* filePath = NULL;
        if (!isGlobal) {
                if (!dotCupotPath(cwdPath())) {
                        fprintf(stderr, "there is no cupot repository initalized here!!\n");
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