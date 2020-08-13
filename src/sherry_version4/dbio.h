#pragma once
#include "allocate.h"
#include "common.h"
#include <sys/types.h>    
#include <sys/stat.h>  
#include <sys/mman.h>  
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


typedef struct IndexNode
{
    uint64_t key;
    uint64_t version;
}IndexNode;


typedef struct Dbio
{
    //DataFile's return code
    int fdDelta;
    uint64_t deltaPosition = 0;
    //catche's mmap header
    DeltaItem *catcheList;
    uint64_t catchePosition = 0;
    //index's mmap header
    IndexNode *indexList; 
    uint64_t indexPosition = 0;
    //zero's mmap header
    int fdZero;
}Dbio;

int createDir(const char *dir);
int createDoc(const char *dir, const char *filename, uint64_t dataSize);
void* createMap(int fd, size_t len);
Dbio* initDbio(const char *dir);
void recoverDbioIndex(Dbio* dbio, uint64_t indexPosition);
bool writeIO(Dbio *dbio, const DeltaItem &deltaItem, uint64_t version);
bool readIO(Dbio *dbio, uint64_t offset, DeltaItem *deltaItem);
void readZero(Dbio *dbio,DeltaItem *deltaItem);