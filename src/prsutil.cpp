#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <stdio.h>
#include "prs.cpp"

int main(int argc, char** argv)
{
    char* srcname = argv[1];
    char destname[PATH_MAX];
    unsigned long size,size2,bytesread;
    bool compress = false,quest = false;
    if (srcname[0] == '*')
    {
        compress = true;
        srcname++;
    }
    strcpy(destname,srcname);
    if (compress) strcat(destname,".prs");
    else strcat(destname,".bin");

    printf("> fuzziqer software prs [de]compressor\n> source file: %s\n> dest file: %s\n\n",srcname,destname);
    FILE* src = fopen(srcname,"rb");
    if (!src)
    {
        printf("> could not open source file\n");
        return 0;
    }
    fseek(src, 0, SEEK_END);
    size = ftell(src);
    fseek(src, 0, SEEK_SET);
    void* srcdata = malloc(size);
    if (!srcdata)
    {
        fclose(src);
        printf("> not enough free memory to load source data\n");
        return 0;
    }
    printf("> loading %d bytes\n",size);
    bytesread = fread(srcdata,1,size,src);
    fclose(src);
    if (bytesread != size)
    {
        free(srcdata);
        printf("> could not read source data\n");
        return 0;
    }

    void* destdata = malloc(compress ? size : prs_decompress_size(srcdata));
    if (!destdata)
    {
        free(srcdata);
        printf("> not enough free memory to process data\n");
        return 0;
    }

    unsigned long start,end;
    if (compress) size2 = prs_compress(srcdata,destdata,size);
    else size2 = prs_decompress(srcdata,destdata);

    FILE* dest = fopen(destname,"wb");
    if (!dest)
    {
        fclose(src);
        printf("> could not open dest file\n");
        return 0;
    }
    printf("> saving %d bytes\n",size2);
    bytesread = fwrite(destdata,1,size2,dest);
    fclose(dest);
    if (bytesread != size2)
    {
        free(srcdata);
        free(destdata);
        printf("> could not write dest data\n");
        return 0;
    }
    printf("> done.\n",size,size2);
    free(srcdata);
    free(destdata);
    return 0;
}

