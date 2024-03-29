#include <stdio.h>
#include "constants.h"

#ifndef CONFIGS_H
#define CONFIGS_H


struct ConfigEntry {
        char key[MAX_CONFIG_LENGHT];
        char value[MAX_CONFIG_LENGHT];
};

typedef struct Config {
        struct ConfigEntry entries[MAX_CONFIG_ENTRIES];
        int len;
} Config;

Config* createConfig();
int addEntry(Config* config, const char* key, const char* value);
int editEntry(Config* config, const char* key, const char* value);
char* getEntry(Config* config, const char* key);

int removeEntry(Config* config, const char* key);
int readConfigFile(FILE *file, Config* config);
int writeConfigFile(FILE* file, Config* config);
#endif