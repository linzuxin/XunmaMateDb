#pragma once
#include "store_intf.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DATA_FILE_COUNT 1
#define DATA_FILE_SIZE 6400 * DATA_FILE_COUNT
#define CATCHE_FILE_SIZE 100

struct IndexNode
{
    uint64_t key;
    uint64_t version;
};

struct ZeroNode
{
    uint64_t dataVersion;
    uint64_t dataSize;
    DeltaPacket zeroPacket;
};

struct Dbio
{
    //DataFile's return code
    int fdData;
    int fdIndex;
    int fdCatche;
    int fdZero;
    uint64_t dataPosition;
    //catche's mmap header
    Data *catcheList;
    uint64_t catchePosition;
    //index's mmap header
    IndexNode *indexList;
    uint64_t indexPosition;
    //zero Node
    ZeroNode *zeroNode;
};

int createDir(const char *dir);
int createDoc(const char *dir, const char *filename, uint64_t dataSize);
void *createMap(int fd, size_t len);
Dbio *initDbio(const char *dir);
char* loadZeroNode(const char *dir, const char *filename);
void deinitDbio(Dbio *);
bool writeData(Dbio *dbio, const DeltaItem *deltaItem, uint64_t version);
bool readData(Dbio *dbio, uint64_t offset, Data *data);