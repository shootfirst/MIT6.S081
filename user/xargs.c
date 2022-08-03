#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
    char *args[MAXARG];
    char buf[MAXBUF];
    memset(buf, 0, MAXBUF);
    int i = 0;
    int pid;
    
    for (i = 0; i < MAXARG; i++) 
    {
        args[i] = (char *)malloc(MAXARG_LEN);
        memset(args[i], 0, MAXARG_LEN);
    }
    
    for (i = 1; i < argc; i++) 
    {
        memcpy(args[i-1], argv[i], strlen(argv[i]));
    }
    i--;
    read(0, buf, MAXBUF);
    char *word = buf;
    char *letter = buf;

    while (*letter != '\0') 
    {
        if (*letter == '\n') {
            memset(args[i], 0, MAXARG_LEN);
            memcpy(args[i++], word, letter - word);
            word = letter + 1;
        } 
        letter++;
    }

    if ((pid = fork()) == 0) {
        args[i] = 0; 
        exec(args[0], args);
    } else {
        wait(&pid);
    }
    
    exit(0);
}



