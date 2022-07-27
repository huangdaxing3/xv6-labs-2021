#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

void runexec(char *argv, char **order)
{
    if (fork() == 0) {
        exec(argv, order);
        exit(0);
    }
    return;
}

int main(int argc, char *argv[])
{
    char *order[MAXARG];
    char buf1[MAXARG], buf2[MAXARG];
    int m, count = 0;
    int j = 0;
    char *p = buf2;

    for (int i = 1; i < argc; i++) {
        order[count++] = argv[i];
        // fprintf(2, "|| %s ", argv[i]);
    }

    while ((m = read(0, buf1, sizeof(buf1))) > 0) {
        for (int i = 0; i < m; i++) {
            // fprintf(2, " %s ", buf1);
            if (buf1[i] == ' ') {
                buf2[j++] = 0;
                order[count++] = p;
                p = buf2 + j;
            } else if (buf1[i] == '\n') {
                buf2[j] = 0;
                order[count++] = p;
                order[count] = 0;
                j = 0;
                p = buf2;
                count = argc - 1;
                runexec(argv[1], order);
                wait(0);
            } else {
                buf2[j++] = buf1[i];
            }
        }
    }

    exit(0);
}