#ifndef GREP_CMD_H
#define GREP_CMD_H

int grepLine(char* line, char* word);

void grepLineFunc(char* line, char* word, void (*command_func)(char*), char* pre_print);

int grepCommand(int argc, char* argv[]);
#endif