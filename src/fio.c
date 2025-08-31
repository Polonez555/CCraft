#include <fio.h>
#include <stdio.h>
#include <stdlib.h>

const char* FileToText(const char* path,size_t* length)
{
    FILE* fle = fopen(path,"r");
    fseek(fle,0,SEEK_END);
    size_t len = ftell(fle);
    fseek(fle,0,SEEK_SET);
    char* buf = malloc(len);
    fread(buf,1,len,fle);
    fclose(fle);
    (*length) = len;
    return (const char*)buf;
}

const char* FileToTextZE(const char* path)
{
    FILE* fle = fopen(path,"r");
    fseek(fle,0,SEEK_END);
    size_t len = ftell(fle);
    fseek(fle,0,SEEK_SET);
    char* buf = malloc(len+1);
    fread(buf,1,len,fle);
    buf[len] = 0;
    fclose(fle);
    return (const char*)buf;
}