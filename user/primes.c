#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int *p)
{
    int m, pi[2];
    close(p[1]);

    if (read(p[0], &m, sizeof(int)) == 0) {
        return;
    }
    pipe(pi);
    if (fork() == 0) {
        prime(pi);
    } else {
        close(pi[0]);
        printf("prime %d\n", m);
        int pri = m;
        while (read((p[0]), &m, sizeof(int)) != 0) {
            if (m % pri != 0) {
                write(pi[1], &m, sizeof(int));
            }
        }
        close(pi[1]);
        wait(0);
        exit(0);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int p[2];
    pipe(p);

    if (fork() == 0) {
        prime(p);
    } else {
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
    exit(0);
}