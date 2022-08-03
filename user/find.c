#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void 
find(char *path, char *find_name) 
{ 
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    
    switch(st.type){
    case T_FILE:
        if (memcmp(fmtname(path), find_name, strlen(find_name)) == 0) {
            printf("%s\n",path);
        }
        break;

    case T_DIR:

        if (memcmp(fmtname(path), find_name, strlen(find_name)) == 0) {
            printf("%s\n",path);
        }

        if(strlen(path) + 1 + DIRSIZ + 1 > 512){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (memcmp(de.name, ".", 1) == 0 || memcmp(de.name, "..", 2) == 0) {
                continue;
            }

            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            find(buf, find_name);
        }
        break;

    }
    close(fd);
}

int 
main(int argc, char *argv[])
{
    if (argc < 3) {
        exit(-1);
    }
    find(argv[1], argv[2]);
    exit(0);
}