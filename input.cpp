#include "input.h"
namespace lcc{
    int infd;
    char *firstfile;
    char *file;
    char *line;
    int lineno;

    char *cp;
    char *limit;
    int bsize;
    unsigned char buffer[MAXLINE + 1 + BUFSIZE + 1];
    void inputInit(){
        limit = cp = (char *)&buffer[MAXLINE+1];
        bsize = -1;
        lineno = 0;
        file = NULL;
        fillbuf();
        if (cp >= limit)
        {
            cp = limit;
        }
        nextline();
    }
    void fillbuf(){
        if (bsize == 0)
        {
            return 0;
        }
        if (cp >= limit)
        {
            cp = &buffer[MAXLINE+1];
        }
        else
        {
            // move thee tail portion
        }
        bsize = read(infd, &buffer[MAXLINE+1], BUFSIZE);
        if (bsize < 0){
            error("read error\n");
            exit(1);
        }
        limit = &buffer[MAXLINE+1+bsize];
        *limit = '\n';
    }

}