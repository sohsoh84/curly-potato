#include "config_cmd.h"
#include "../configs.h"
#include "../paths.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int configCommand(int argc, char *argv[]) {
        int reqArgs = 2 + 2;
        if (argc < reqArgs) {
                fprintf(stderr, "Not enough arguments!\n");
                return 1;
        }

        bool isGlobal = false;

        int argPtr = 2;
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


        if (!isGlobal) {
                fprintf(stderr, "there is no support for local files!"); // TODO:
                return 1;
        }

        char* filePath = NULL;
        if (strlen(key) > 6 && key[0] == 'a' && key[1] == 'l' && key[2] == 'i' && key[3] == 'a' 
                && key[4] == 's' && key[5] == '.') {
                        key += 6;

               filePath = globalAliasPath();
        } else filePath = globalConfigPath();

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