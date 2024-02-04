#include "diff.h"
#include "../diff.h"
#include "../paths.h"
#include "../utils.h"
#include "../constants.h"
#include "../commits.h"
#include "../vcs.h"
#include "../tracker.h"
#include "../dotcupot.h"
#include "../stash.h"
#include "../syscalls.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printLine(FILE* stream, char* file_name, char* line, int lc, const char* COL, int file_name_ignore) {
        fprintf(stream, "%s", COL);
        fprintf(stream, "%s-%d\n", file_name + file_name_ignore, (line ? lc : 0));
        fprintf(stream, "%s\n", (line ? line : "EOF"));
        fprintf(stream, RESET);
}

int commitDiff(int ignore_unmatch_files, FILE* stream, char* commit_id1, char* commit_id2, int conflict_manager) {
        if (!dotCupotPath(cwdPath())) {
                fprintf(stderr, RED "You should be inside a cupot repository to run this command\n" RESET "try running cupot init\n");
                return 1;
        }

        if (!getCommitConfigs(commit_id1)) {
                printf("Invalid commit 1\n");
                return 0;
        }

        if (!getCommitConfigs(commit_id2)) {
                printf("Invalid commit 2\n");
                return 0;
        }

        char* track1 = commitTrackerPath(commit_id1), *track2 = commitTrackerPath(commit_id2);        
        char** all_tracked1, **all_tracked2;
        int count1 = allTrackedFiles(track1, &all_tracked1);
        int count2 = allTrackedFiles(track2, &all_tracked2);

        for (int i = 0; i < count1; i++) {
                if (!isTracked(track2, all_tracked1[i])) {
                        if (!ignore_unmatch_files) 
                                fprintf(stream, YELLOW "%s exists in first commit but not in second one\n" RESET, all_tracked1[i]);
                }
        }

        for (int i = 0; i < count2; i++) {
                if (!isTracked(track1, all_tracked2[i])) {
                        if (!ignore_unmatch_files)
                                fprintf(stream, YELLOW "%s exists in second commit but not in first one\n" RESET, all_tracked2[i]);
                }
        }

        char* proj_path1 = mergePaths(dotCupotPath(cwdPath()), "temp1");
        char* proj_path2 = mergePaths(dotCupotPath(cwdPath()), "temp2");

        int result = 0;
        buildProjectFromCommit(proj_path1, commit_id1);
        buildProjectFromCommit(proj_path2, commit_id2);
        for (int i = 0; i < count1; i++) {
                if (isTracked(track2, all_tracked1[i])) {
                        char* path1 = mergePaths(mergePaths(proj_path1, projectName(cwdPath())), trackRelativePath(all_tracked1[i]));
                        char* path2 = mergePaths(mergePaths(proj_path2, projectName(cwdPath())), trackRelativePath(all_tracked1[i]));
                        result += fileDiff(stream, path1, path2, 1, lineCounts(path1), 1, lineCounts(path2), strlen(proj_path1) + 1, conflict_manager);                        
                }
        }

        return result;
}


// DUPLICATED CODE

int stashDiff(int ignore_unmatch_files, FILE* stream, int stash_id) {
        if (!dotCupotPath(cwdPath())) {
                printf("you should be inside a cupot repository\n");
                return 1;
        }

        if (!fileExists(getStashDirPath(stash_id))) {
                printf("Invalid stash_id: %d\n", stash_id);
                return 1;
        }

        char* proj_tracker_path = mergePaths(dotCupotPath(cwdPath()), "proj_tracker");
        FILE* proj_tracker_builder = fopen(proj_tracker_path, "w");
        fclose(proj_tracker_builder);
        addRecursivlyToTrackedFile(proj_tracker_path, projectPath(cwdPath()));

        char* track1 = proj_tracker_path, *track2 = stashTrackerPath(stash_id);        
        char** all_tracked1, **all_tracked2;
        int count1 = allTrackedFiles(track1, &all_tracked1);
        int count2 = allTrackedFiles(track2, &all_tracked2);

        for (int i = 0; i < count1; i++) {
                if (!isTracked(track2, all_tracked1[i])) {
                        if (!ignore_unmatch_files) 
                                fprintf(stream, YELLOW "%s exists in project but not in the stash\n" RESET, all_tracked1[i]);
                }
        }

        for (int i = 0; i < count2; i++) {
                if (!isTracked(track1, all_tracked2[i])) {
                        if (!ignore_unmatch_files)
                                fprintf(stream, YELLOW "%s exists in stash but not in the project\n" RESET, all_tracked2[i]);
                }
        }

        char* proj_path1 = projectPath(cwdPath());
        char* proj_path2 = getStashDirPath(stash_id);

        int result = 0;
        for (int i = 0; i < count1; i++) {
                if (isTracked(track2, all_tracked1[i])) {
                        char* path1 = mergePaths(proj_path1, trackRelativePath(all_tracked1[i]));
                        char* path2 = mergePaths(mergePaths(proj_path2, projectName(cwdPath())), trackRelativePath(all_tracked1[i]));
                        result += fileDiff(stream, path1, path2, 1, lineCounts(path1), 1, lineCounts(path2), strlen(proj_path1) + 1, 0);                        
                }
        }

        return result;
}

int lineDiff(char* line1_, char* line2_, int* s1, int* s2, int* len1, int* len2) {
        if (line1_ == NULL || line2_ == NULL) return 0;
        char* line1 = strdup(line1_);
        char* line2 = strdup(line2_);

        char* save_ptr1, *save_ptr2;
        char* p1 = strtok_r(line1, " \t\n", &save_ptr1);
        char* p2 = strtok_r(line2, " \t\n", &save_ptr2);

        int found = 0;
        while (p1 != NULL || p2 != NULL) {
                if (p1 == NULL || p2 == NULL) return 0;
                if (strcmp(p1, p2)) {
                        if (found) return 0;
                        found = 1;
                        
                        *s1 = p1 - line1;
                        *s2 = p2 - line2;
                }

                p1 = strtok_r(NULL, " \t\n", &save_ptr1);
                p2 = strtok_r(NULL, " \t\n", &save_ptr2);

        }

        if (found) {
                *len1 = strlen(*s1 + line1);
                *len2 = strlen(*s2 + line2);
        }
        return found;
}

void printLine2(FILE* stream, char* file_name, char* line, int lc, const char* COL, int file_name_ignore, int s, int len) {
        fprintf(stream, "%s", COL);
        fprintf(stream, "%s-%d\n", file_name + file_name_ignore, (line ? lc : 0));
        fprintf(stream, "%.*s", s, line);
        fprintf(stream, CYAN ">>");
        fprintf(stream, "%.*s", len, line + s);
        fprintf(stream, "<<%s", COL);
        fprintf(stream, "%s\n", line + s + len);
        fprintf(stream, RESET);
}

void editFile(char* path, int *lc, char* str) {
        if (*lc > lineCounts(path)) {
                FILE* file = fopen(path, "a");
                fprintf(file, "\n%s", (str ? str : ""));
                fclose(file);
                (*lc)++;
                return;
        }

        char backup_path[MAX_PATH_LEN];
        strcpy(backup_path, path);
        strcat(backup_path, ".bak");

        FILE* backup_file = fopen(backup_path, "w");
        FILE* file = fopen(path, "r");
        int l = 0;
        char line[4096];
        while (fgets(line, sizeof(line), file)) {
                l++;
                if (l == *lc) fprintf(backup_file, "%s\n", (str ? str : ""));
                else fprintf(backup_file, "%s", line);
        }

        fclose(file);
        fclose(backup_file);
        removeFileDir(path);
        copyDirWithName(backup_path, path);
        removeFileDir(backup_path);
}

int fileDiff(FILE* stream, char* path1, char* path2, int s1, int e1, int s2, int e2, int file_name_ignore, int conflict_manager) {
        char backup_path[MAX_PATH_LEN];
        strcpy(backup_path, path1);
        strcat(backup_path, ".bak");
        if (conflict_manager) {
                copyDirWithName(path1, backup_path);
        }

        FILE* f1 = fopen(path1, "r");
        FILE* f2 = fopen(path2, "r");

        if (f1 == NULL || f2 == NULL) {
                return 1;
        }

        
        readDummyLines(f1, s1 - 1);
        readDummyLines(f2, s2 - 1);

        int result = 0;
        int p1 = s1 - 1, p2 = s2 - 1;
        while (p1 < e1 || p2 < e2) {
                char* l1 = readRealLine(f1, &p1);
                char* l2 = readRealLine(f2, &p2);

                if (p1 > e1) l1 = NULL;
                if (p2 > e2) l2 = NULL;

                if (l1 == NULL && l2 == NULL) {
                        free(l1);
                        free(l2);
                        return result;
                }

                if (l1 == NULL || l2 == NULL || strcmp(l1, l2)) {
                        result++;
                        fprintf(stream, "«««««\n");
                        int s1, s2;
                        int len1, len2;
                        if (lineDiff(l1, l2, &s1, &s2, &len1, &len2)) {
                                printLine2(stream, path1, l1, p1, GREEN, file_name_ignore, s1, len1);                                
                                printLine2(stream, path2, l2, p2, RED, file_name_ignore, s2, len2);                                
                        } else {
                                printLine(stream, path1, l1, p1, GREEN, file_name_ignore);
                                printLine(stream, path2, l2, p2, RED, file_name_ignore);
                        }

                        fprintf(stream, "»»»»»\n");

                        if (conflict_manager) {
                                printf(CYAN "\nthere is conflict between branch1 and branch2\n" RESET);
                                printf("options: \n\t-> 1[write line from the first branch]\n\t-> 2[write line from the second branch]\n\t"
                                        "-> edit: resolve confligt manually\n\t-> quit\n" RESET);

                                int done = 0;
                                while (!done) {
                                        done = 1;
                                        char res[40];
                                        fgets(res, sizeof(res), stdin);
                                        strip(res);

                                        if (!strcmp(res, "1")) continue;
                                        else if (!strcmp(res, "2") || !strcmp(res, "edit")) {
                                                char* fres = l2;
                                                if (!strcmp(res, "edit")) {
                                                        printf("edit box: ");
                                                        char line[4096];
                                                        fgets(line, sizeof(line), stdin);
                                                        fres = strdup(line);
                                                }

                                                editFile(backup_path, &p1, fres);
                                        } else if (!strcmp(res, "quit")) {
                                                return 1;
                                        } else {
                                                printf("invalid command, try again\n");
                                                done = 0;
                                        }
                                }
                        }
                }

                free(l1);
                free(l2);
        }

        if (conflict_manager) {
                removeFileDir(path1);
                copyDirWithName(backup_path, path1);
                removeFileDir(backup_path);
        }

        return conflict_manager ? 0 : result;
}

int diffCommand(int argc, char* argv[]) {
        char USAGE[] = "Usage:\n\tcupot diff -f <file1> <file2> –line1 <begin-end> –line2 <begin-end>\n\tcupot diff -c <commit1-id> <commit2-id>\n";
        if (argc < 3) {
                printf("%s", USAGE);
                return 1;
        }

        if (!strcmp(argv[0], "-c")) {
                commitDiff(0, stdout, argv[1], argv[2], 0);
                return 1;
        }

        int start1 = 0, end1 = -1;     
        int start2 = 0, end2 = -1;
        char* path1 = NULL, *path2 = NULL;

        int arg_ptr = 0;
        while (arg_ptr < argc) {
                if (!strcmp(argv[arg_ptr], "-f")) {
                        if (arg_ptr + 2 >= argc) {
                                printf("%s", USAGE);
                                return 1;
                        }

                        path1 = argv[arg_ptr + 1];
                        path2 = argv[arg_ptr + 2];
                        arg_ptr += 3;
                } else if (!strcmp(argv[arg_ptr], "-line1")) {
                        if (arg_ptr + 1 >= argc) {
                                printf("%s", USAGE);
                                return 1;
                        }

                        if (sscanf(argv[arg_ptr + 1], "%d-%d", &start1, &end1) != 2) {
                                printf("%s", USAGE);
                                return 1;
                        }

                        arg_ptr += 2;
                } else if (!strcmp(argv[arg_ptr], "-line2")) {
                        if (arg_ptr + 1 >= argc) {
                                printf("%s", USAGE);
                                return 1;
                        }

                        if (sscanf(argv[arg_ptr + 1], "%d-%d", &start2, &end2)!= 2) {
                                printf("%s", USAGE);
                                return 1;
                        }

                        arg_ptr += 2;
                } else {
                        printf("%s", USAGE);
                        return 1;
                }
        }

        if (!path1 || !path2) {
                printf("%s", USAGE);
                return 1;
        }
        
        if (!fileExists(path1)) {
                printf("file1 not found\n");
                return 1;
        }

        if (!fileExists(path2)) {
                printf("file2 not found\n");
                return 1;
        }

        if (end1 < 0) start1 = 1, end1 = lineCounts(path1);
        if (end2 < 0) start2 = 1, end2 = lineCounts(path2);

        if (start1 > end1 || end1 > lineCounts(path1)) {
                printf("Invalid Boundries for file1\n");
                return 1;
        }

        if (start2 > end2 || end2 > lineCounts(path2)) {
                printf("Invalid Boundries for file2\n");
                return 1;
        }

        if (!fileDiff(stdout, path1, path2, start1, end1, start2, end2, 0, 0)) {
                fprintf(stderr, "No Diff found for file :)\n");
        }
        return 0;
}