#include "diff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int absoluteDiff(char *file1, char *file2) {
        FILE *fp1 = fopen(file1, "r");
        FILE *fp2 = fopen(file2, "r");

        if (!fp1 || !fp2) {
                fprintf(stderr, "Error opening files in absolute Diff\n");
                return 0;
        }

        char buffer1[1024], buffer2[1024];
        int bytes_read1, bytes_read2;
        while (1) {
                bytes_read1 = fread(buffer1, sizeof(char), 1024, fp1);
                bytes_read2 = fread(buffer2, sizeof(char), 1024, fp2);

                if (bytes_read1 != bytes_read2) return 1;
                if (bytes_read1 == 0 && bytes_read2 == 0) break;
                if (memcmp(buffer1, buffer2, bytes_read1) != 0) return 1;
        }

        fclose(fp1);
        fclose(fp2);
        return 0;
}