#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void 
primesieve(int num_vec[], int vec_size) {
    int pip[2];
    pipe(pip);
    int buf = 0;
    int pipe_line[34];
    int cnt = 0;
    int our = 0;

    int pid = fork();
    if (pid == 0) {
        close(pip[1]);
        read(pip[0], &buf, sizeof(int));
        printf("prime %d\n", buf);
        our = buf;
        while (read(pip[0], &buf, sizeof(int)) > 0) {
            if (buf % our != 0) {
                pipe_line[cnt++] = buf;
            }
        }
        close(pip[0]);
        if (cnt > 0) {
            primesieve(pipe_line, cnt);
        }
        exit(0);

    } else {
        close(pip[0]);
        write(pip[1], num_vec, sizeof(int) * vec_size);
        close(pip[1]);
        wait(&pid);
        exit(0);
    }

}

int 
main(int argc, char *argv[])
{   
    int num_vec[34];
    for (int i = 2; i < 36; i++) {
        num_vec[i - 2] = i;
    }
    primesieve(num_vec, 34);
    exit(0);                          
}