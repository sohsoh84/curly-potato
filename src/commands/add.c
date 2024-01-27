#include "add.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glob.h>
#include <libgen.h>
#include <dirent.h>
#include <ctype.h>
#include "../dotcupot.h"
#include "../paths.h"
#include "../staging_area.h"
#include "../syscalls.h"
#include "../constants.h"

static int addToStageingArea(char* stage_path_, char* file_path_) {
        if (stage_path_ == NULL || stage_path_ == NULL) {
                fprintf(stderr, "wrong stage_path or file_path!\n");
                return 1;
        }
        
        char stage_path[PATH_MAX], file_path[PATH_MAX];
        realpath(stage_path_, stage_path);
        realpath(file_path_, file_path);

        if (!fileExists(file_path)) {
                fprintf(stderr, "there is no such file or directory: %s\n", file_path);                
                return 1;
        }

        if (!dotCupotPath(file_path)) {
                fprintf(stderr, "%s is not in a cupot repository!!\n", file_path);
                return 1;
        }

        char relative_path[PATH_MAX];
        releativePath(file_path, relative_path);

        char* dest_path = mergePaths(stage_path, relative_path);
        if (createRequiredDirectories(dest_path)) return 1;
        if (removeFileDir(dest_path)) return 1;
        if (copyFileDir(file_path, dest_path)) return 1;
        removeFileDir(mergePaths(mergePaths(stage_path, projectName(file_path)), ".cupot")); // this line is needed to remove the .cupot file from the staging area
        printf("%s added to staging area\n", dest_path);
        return 0;
}

static int stage(char* file_path) {
        glob_t globbuf;
        char* stage_path = mergePaths(dotCupotPath(cwdPath()), STAGE_NAME);

        glob(file_path, 0, NULL, &globbuf);
        
        int result = 0;
        if (globbuf.gl_pathc == 0) {
                fprintf(stderr, "invalid file path!\n");
                return 0;
        }

        for (int i = 0; i < globbuf.gl_pathc; i++)
                result += (addToStageingArea(stage_path, globbuf.gl_pathv[i]) == 0);
        
        globfree(&globbuf);
        return result;
}

static char* concat(const char* a, const char* b) {
        char* result = malloc(strlen(a) + strlen(b) + 1);
        strcpy(result, a);
        strcat(result, b);
        result[strlen(a) + strlen(b)] = '\0';
        return result;
}

char* stagingStatusString(char* directoryName, int n, int lvl, int* total_unstaged) {
        DIR *dir = opendir(directoryName);
        if (dir == NULL) return NULL;

        struct dirent *ent;
        char* result = (char*) malloc(1);
        result[0] = '\0';

        while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                }

                char filePath[PATH_MAX];
                snprintf(filePath, sizeof(filePath), "%s/%s", directoryName, ent -> d_name);

                if (!strcmp(basename(filePath), ".cupot")) continue;

                if (lvl < n) {
                        for (int i = 0; i < lvl; i++) {
                                char* tmp = concat(result, "|--- ");
                                free(result);
                                result = tmp;
                        }
                }
                
                if (ent -> d_type == DT_DIR) {
                        int tmp_unstaged = 0;
                        char* tmp_result = stagingStatusString(filePath, n, lvl + 1, &tmp_unstaged);
                        *total_unstaged += tmp_unstaged;
                        if (lvl >= n) return result;
                        char* tmp = concat(result, basename(filePath));
                        free(result);
                        result = tmp;

                        if (!fileExists(stageFilePath(filePath)))
                                tmp_unstaged++;
                        char status_[50];
                        strcpy(status_, (tmp_unstaged ?  RED " unstaged" RESET "\n" : GREEN " staged" RESET "\n")); // TODO: more complex status
                        tmp = concat(result, status_);
                        free(result);
                        result = tmp;

                        tmp = concat(result, tmp_result);
                        free(result);
                        result = tmp;
                } else if (ent -> d_type == DT_REG) {
                        int tmp_unstaged = stagingStatus(filePath);
                        *total_unstaged += tmp_unstaged;

                        if (lvl >= n) return result;
                        char* tmp = concat(result, basename(filePath));
                        free(result);
                        result = tmp;

                        char status_[50];
                        strcpy(status_, (tmp_unstaged ?  RED " unstaged" RESET "\n" : GREEN " staged" RESET "\n")); // TODO: more complex status
                        tmp = concat(result, status_);
                        free(result);
                        result = tmp;
                }
        }

        return result;
        closedir(dir);
}

int addCommand(int argc, char *argv[]) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, "you are not in a cupot repository!!\n");
                return 1;
        }

        if (argc <= 0) {
                fprintf(stderr, "too few arguments\n");
                return 1;
        }

        if (!strcmp(argv[0], "-redo")) {
                // TODO: complete

                return 0;
        } else if (!strcmp(argv[0], "-n")) {
                int level = 1000;
                if (argc > 1) {
                        level = 0;
                        for (int i = 0; i < strlen(argv[1]); i++) {
                                if (!isdigit(argv[1][i])) {
                                        fprintf(stderr, "%s is not a number!\n", argv[1]);
                                        return 1;
                                }

                                level = level * 10 + (argv[1][i] - '0');
                        }
                }
               


                if (level < 1) {
                        fprintf(stderr, "level should be at least 1!\n");
                        return 1;
                }

                int _ = 0;
                char* result = stagingStatusString(cwdPath(), level, 0, &_);
                printf("%s", result);
                free(result);
                return 0;
        }

        backupStageArea();
        int staged_successfully = 0;
        if (!strcmp(argv[0], "-f")) {
                if (argc == 1) {
                        fprintf(stderr, "too few arguments\n");
                        return 1;
                }

                for (int i = 1; i < argc; i++)
                        staged_successfully += stage(argv[i]);
        } else {
                for (int i = 0; i < argc; i++)
                        staged_successfully += stage(argv[i]);
        }

        // if the code reaches this point, then the .tmp_stage directory has been created and should \
                be dealt with // TODO:
        if (staged_successfully == 0) {
                undoBackupStagingArea();
                printf("nothing staged!\n");
        }
        return 0;
}