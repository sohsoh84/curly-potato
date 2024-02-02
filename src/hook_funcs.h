#ifndef HOOK_FUNCS_CMD
#define HOOK_FUNCS_CMD

#include "hook.h"

HOOK_RESULT eof_blank_space(char* path);
HOOK_RESULT balance_braces(char* path);

HOOK_RESULT character_limit(char* path);

HOOK_RESULT file_size_check(char* path);

HOOK_RESULT todo_check(char* path);

HOOK_RESULT static_error_check(char *path);

#endif