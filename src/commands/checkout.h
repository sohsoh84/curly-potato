#ifndef CHECKOUT_CMD_H
#define CHECKOUT_CMD_H

int checkoutCommand(int argc, char *argv[]);
int checkout(char* commit_id, int revert_state);

#endif