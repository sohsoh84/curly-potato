#include "configs.h"
#include <stdio.h>

int main() {
        struct Config* config = createConfig();

        FILE* test = fopen("test.conf", "r");
        readConfigFile(test, config);
        fclose(test);

        editEntry(config, "shomare", "12");
        test = fopen("test.conf", "w");
        writeConfigFile(test, config);
        return 0;
}