#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int parent[2];
    int child[2];
    pipe(parent);
    pipe(child);
    int pid = fork();
    char buf[] = {"0"};

    if (pid == 0) {
        close(parent[1]);
        close(child[0]);
        read(parent[0], buf, 1);
        fprintf(2, "%d: received ping\n", getpid());
        write(child[1], buf, 1);
        close(child[1]);
        exit(0);
    } else {
        close(parent[0]);
        close(child[1]);
        write(parent[1], buf, 1);
        read(child[0], buf, 1);
        fprintf(2, "%d: received pong\n", getpid());
        exit(0);
    }
    
    exit(0);
}