#include "reset.h"
#include "../syscalls.h"
#include "../staging_area.h"
#include "../paths.h"
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commit.h"

static int reset(char* file_path) {
        glob_t globbuf;
        glob(file_path, 0, NULL, &globbuf);
        
        int result = 0;
        if (globbuf.gl_pathc == 0) {
                fprintf(stderr, "invalid file path!\n");
                return 0;
        }

        for (int i = 0; i < globbuf.gl_pathc; i++) {
                char* stage_path = stageFilePath(globbuf.gl_pathv[i]);
                if (stage_path == NULL) continue;
                if (!fileExists(stage_path)) {
                        fprintf(stderr, "the file/directory %s does not exist in staging area!\n", globbuf.gl_pathv[i]);
                        continue;
                }

                if (removeFileDir(stage_path) == 0) {
                        result++;
                        fprintf(stderr, "%s unstaged successfully!\n", globbuf.gl_pathv[i]);
                }
        }       

        globfree(&globbuf);
        return result;
}

int resetCommand(int argc, char *argv[]) {
        int unstaged_successfully = 0;
        if (argc <= 0) {
                fprintf(stderr, "too few arguments\n");
                return 1;
        }

        if (!strcmp(argv[0], "-undo")) {
                fprintf(stderr, "use --undo instead\n");
                return 1;
        }

        if (!strcmp(argv[0], "--undo")) {

                if (undoBackupStagingArea()) fprintf(stderr, "there is no backup staging area!\n");
                else fprintf(stderr, "rolled back to backup staging area\n");
                return 0;
        }

        if (!strcmp(argv[0], "-f")) {
                if (argc == 1) {
                        fprintf(stderr, "too few arguments\n");
                        return 1;
                }

                for (int i = 1; i < argc; i++)
                        unstaged_successfully += reset(argv[i]);
        } else {
                for (int i = 0; i < argc; i++)
                        unstaged_successfully += reset(argv[i]);
        }

        // if the code reaches this point, then the .tmp_stage directory has been created and should \
                be dealt with // TODO:
        if (unstaged_successfully == 0) {
                //undoBackupStagingArea();
                printf("nothing unstaged!\n");
        }
        return 0;
}