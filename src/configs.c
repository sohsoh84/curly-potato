#include "configs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

struct Config* createConfig() {
        Config* config = malloc(sizeof(Config));
        if (config == NULL) {
                fprintf(stderr, "Memmory allocation failed(createConfig())\n");
                return NULL;
        }

        config -> len = 0;
        return config;
}

int addEntry(Config *config, const char *key, const char *value) {
        if (config->len >= MAX_CONFIG_LENGHT) {
                fprintf(stderr, "Error: too much config entries!\n");
                return 1;
        }

        strcpy(config->entries[config->len].key, key);
        strcpy(config->entries[config->len].value, value);

        config->len++;
        return 0;
}

// this function adds a new entry if no entry with given key exits
int editEntry(Config *config, const char *key, const char *value) {
        for (int i = 0; i < config->len; i++) {
                if (!strcmp(config->entries[i].key, key)) {
                        strcpy(config->entries[i].value, value);
                        return 0;
                }
        }

        return addEntry(config, key, value);
}

int removeEntry(Config *config, const char* key) {
        for (int i = 0; i < config->len; i++) {
                if (!strcmp(config->entries[i].key, key)) {
                        for (int j = i; j < config->len - 1; j++) {
                                strcpy(config->entries[j].key, config->entries[j + 1].key);
                                strcpy(config->entries[j].value, config->entries[j + 1].value);
                        }

                        config->len--;
                        return 1;
                }
        }

        return 0;
}

// returns NULL if it couldnt find the given key
char *getEntry(Config *config, const char *key) {
        for (int i = 0; i < config->len; i++)
                if (!strcmp(config->entries[i].key, key))
                        return config->entries[i].value;

        return NULL;
}

int readConfigFile(FILE *file, struct Config *config) {
        if (file == NULL) {
                fprintf(stderr, "Invalid File!\n");
                return 1;
        }

        char tmpKey[MAX_CONFIG_LENGHT], tmpValue[MAX_CONFIG_LENGHT];
        char line[MAX_CONFIG_LENGHT * 2];
        while (fgets(line, MAX_CONFIG_LENGHT, file)) {
                int ind = strchr(line, '=') - line;
                strcpy(tmpKey, line);
                tmpKey[ind] = '\0';
                strcpy(tmpValue, line + ind + 1);
                strip(tmpValue);
                if (addEntry(config, tmpKey, tmpValue))
                        return 1;
        }

        return 0;
}

int writeConfigFile(FILE *file, struct Config *config) {
        if (file == NULL) {
                fprintf(stderr, "Invalid File!\n");
                return 1;
        }

        for (int i = 0; i < config->len; i++)
                fprintf(file, "%s=%s\n", config->entries[i].key, config->entries[i].value);

        return 0;
}
