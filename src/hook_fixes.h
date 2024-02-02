#ifndef HOOK_FIXES_H
#define HOOK_FIXES_H

typedef enum HOOK_FIX_RESULT {
        FIX_FAILED,
        FIX_SUCCEEDED,
        FIX_UNAVAILABLE
} HOOK_FIX_RESULT;

HOOK_FIX_RESULT eof_blank_space_fix(char* path);
HOOK_FIX_RESULT balance_braces_fix(char* path);

#endif