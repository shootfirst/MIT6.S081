#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main (int argc, char *argv[]) 
{   
    int ping[2];
    int pong[2];
    int pid;
    pipe(ping);
    pipe(pong);
    char buf[5];
    memset(buf, 0, 5);

    if ((pid = fork()) == 0) {
        close(ping[1]);
        read(ping[0], buf, 5);
        printf("%d: received %s\n", getpid(), buf);
        close(pong[0]);
        write(pong[1], "pong", strlen("pong"));
        // parent
    } else {
        close(ping[0]);
        write(ping[1], "ping", strlen("ping"));
        close(pong[1]);
        read(pong[0], buf, 5);
        printf("%d: received %s\n", getpid(), buf);
    }
    exit(0);
}

