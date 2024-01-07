#include <stdio.h>

#ifndef CONFIGS_H
#define CONFIGS_H

#define MAX_CONFIG_LENGHT       100
#define MAX_CONFIG_ENTRIES      20    

struct ConfigEntry {
        char key[MAX_CONFIG_LENGHT];
        char value[MAX_CONFIG_LENGHT];
};

struct Config {
        struct ConfigEntry entries[MAX_CONFIG_ENTRIES];
        int len;
};

struct Config* createConfig();
int addEntry(struct Config* config, const char* key, const char* value);
int editEntry(struct Config* config, const char* key, const char* value);
char* getEntry(struct Config* config, const char* key);
int readConfigFile(FILE *file, struct Config* config);
int writeConfigFile(FILE* file, struct Config* config);
// void freeConfig(struct Config* config); TODO:

#endif