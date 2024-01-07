#include "configs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Config* createConfig() {
        struct Config* config = malloc(sizeof(struct Config));
        if (config == NULL) {
                fprintf(stderr, "Memmory allocation failed(createConfig())\n");
                return NULL;
        }

        config -> len = 0;
        return config;
}

int addEntry(struct Config *config, const char *key, const char *value) {
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
int editEntry(struct Config *config, const char *key, const char *value) {
        for (int i = 0; i < config->len; i++) {
                if (!strcmp(config->entries[i].key, key)) {
                        strcpy(config->entries[i].value, value);
                        return 0;
                }
        }

        return addEntry(config, key, value);
}

// returns NULL if it couldnt find the given key
char *getEntry(struct Config *config, const char *key) {
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
        while (fscanf(file, " %[^=]=%s", tmpKey, tmpValue) == 2)
                if (addEntry(config, tmpKey, tmpValue))
                        return 1;

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
