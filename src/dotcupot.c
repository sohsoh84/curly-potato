#include "dotcupot.h"
#include "constants.h"
#include "paths.h"
#include "configs.h"
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* dotCupotPath(char *path_) { // TODO: it only goes until user path
        char* path = strdup(path_);
        while (isSubdirectory(userHomePath(), path)) { // TODO: check if it is a directory
                if (fileExists(mergePaths(path, ".cupot")))
                        return mergePaths(path, ".cupot");

                char* tmp = dirName(path);
                strcpy(path, tmp);
        }

        return NULL;
}

char *projectPath(char *path) {
        char* tmp = dotCupotPath(path);
        if (tmp == NULL) return NULL;
        return dirName(tmp);
}

char *stagingAreaPath(char *path) {
        return mergePaths(dotCupotPath(path), STAGE_NAME);
}

char* commitsAreaPath(char *path) {
        return mergePaths(dotCupotPath(path), COMMITS_NAME);
}

char *projectName(char* path) {
        char* tmp = projectPath(path);
        if (!tmp) return NULL;
        return basename(tmp);
}

void releativePath(char *path, char* destination) {
        char file_path[PATH_MAX];
        realpath(path, file_path);
        suffixPath(destination, file_path, dirName(projectPath(file_path)));
}

static char* tryReadConfigEntry(char* key, char* path) {
        if (!fileExists(path)) return NULL;
        Config* config = createConfig();
        FILE* f = fopen(path, "r");
        readConfigFile(f, config);
        
        char* value = getEntry(config, key);
        fclose(f);
        return value;
}

char *dotCupotConfigEntry(char *key) {
        char* value = NULL;
        char* tmp = tryReadConfigEntry(key, globalConfigPath());
        if (tmp) value = tmp;
        tmp = tryReadConfigEntry(key, localConfigPath());
        if (tmp) value = tmp;

        return value;
}

char* getCWB() {
        return tryReadConfigEntry("cwb", localConfigPath());
}

int writeCWB(char* branch_name) {
        Config* cupotConfig = createConfig();
        FILE* file = fopen(localConfigPath(), "r");
        if (file)
                readConfigFile(file, cupotConfig);
        fclose(file);

        file = fopen(localConfigPath(), "w");
        editEntry(cupotConfig, "cwb", branch_name);
        writeConfigFile(file, cupotConfig);
        fclose(file);
        return 0;
}

char* commitMessageAliasPath(char* path) {
        return mergePaths(dotCupotPath(path), COMMIT_MESSAGE_ALIAS);
}
