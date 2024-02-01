#ifndef HOOK_H
#define HOOK_H

typedef enum HOOK_RESULT {
        HOOK_PASSED,
        HOOK_SKIPPED,
        HOOK_FAILED
} HOOK_RESULT;

char* hooksPath();

int addHook(char* hook);
int removeHook(char* hook);

int hookExists(char* hook);

int hookCount();

#endif