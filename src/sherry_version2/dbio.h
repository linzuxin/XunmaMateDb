#pragma once
#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DATA_FILE_SIZE 64000000
#define CATCHE_FILE_SIZE 100

struct IndexNode
{
    uint64_t key;
    uint64_t version;
};

struct Dbio
{
    //DataFile's return code
    int fdData;
    uint64_t dataPosition;
    //catche's mmap header
    Data *catcheList;
    uint64_t catchePosition;
    //index's mmap header
    IndexNode *indexList;
    uint64_t indexPosition;
};

int createDir(const char *dir);

int createDoc(const char *dir, const char *filename, uint64_t dataSize);
void *createMap(int fd, size_t len);
Dbio *initDbio(const char *dir);
bool writeData(Dbio *dbio, DeltaItem *deltaItem, uint64_t version);
bool readData(Dbio *dbio, uint64_t offset, Data *data);