#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char const *argv[])
{
    char *args[MAXARG];
    char buf[MAXBUF] = { 0 };
    int i = 0, index = 0;
    int pid;
    
    for (i = 0; i < MAXARG; i++)
    {
        args[i] = (char *)malloc(MAXARG_LEN);
        memset(args[i], 0, MAXARG_LEN);
    }
    
    for (i = 1; i < argc; i++)
    {
        memcpy(args[index++], argv[i], strlen(argv[i]));
    }

    while (read(0, buf, MAXBUF) != 0) {
        char *word = buf;
        char *letter = buf;

        while (*letter != '\0')
        {
            if (*letter == '\n') { 
                memset(args[index], 0, MAXARG_LEN);
                memcpy(args[index++], word, letter - word);
                word = letter + 1;
            }
            letter++;
        }

        if ((pid = fork()) == 0 ){
            args[index] = 0; 
            exec(args[0], args);
        }
        else {
            index = argc - 1;
            memset(buf, 0, MAXBUF);
            wait(&pid);
        }
    }
    exit(0);
}

