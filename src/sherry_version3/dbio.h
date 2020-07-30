
#include "common.h"
#include <sys/types.h>    
#include <sys/stat.h>  
#include <sys/mman.h>  
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DATA_FILE_SIZE 80000
#define CATCHE_FILE_SIZE 3

typedef struct IndexNode
{
    uint64_t key;
    uint64_t version;
}IndexNode;


typedef struct Dbio
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


}Dbio;

int createDir(const char *dir)
{
    //estimate dir's existence 
    int accessresult = access(dir, 0);
    int mkdirresult = 0;
    //if dir inexist, mkdir
    if (accessresult == -1)
    {
        mkdirresult = mkdir(dir, 0755);
        printf("createDir:%d,%s\n", mkdirresult, dir);
    }
    return mkdirresult;
}

int createDoc(const char *dir, const char *filename, uint64_t dataSize)
{
    //string for complete path
    char dataPath[256] = " ";
    //combine dir with filename
    sprintf(dataPath, "%s/%s",dir, filename);
    printf("datapath: %s\n",dataPath);
    //create and open file writable & readable
    int fd = open(dataPath, O_CREAT | O_RDWR | O_TRUNC, 00777);
    //allocate space
    fallocate(fd,0,0,dataSize);
    //trim purplus space
    ftruncate(fd,dataSize);
    return fd;
}

void* createMap(int fd, size_t len)
{
    void *head = (void*)mmap(NULL, len,  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return head;
}

Dbio* initDbio(const char *dir)
{
    //create dbio's pointer for storing datafile catchefile indexfile
    Dbio *dbio = (Dbio*)malloc(sizeof(Dbio));
    //create dir
    createDir(dir);
    //create & open datafile
    dbio->fdData = createDoc(dir,"dataFile",DATA_FILE_SIZE*sizeof(Data));
    dbio->dataPosition = 0;
    printf("dataPosition:%ld\n",dbio->dataPosition);
    //create & open catcheflie
    int fdCatche = createDoc(dir,"catcheFile",CATCHE_FILE_SIZE*sizeof(Data));
    dbio->catcheList = (Data*)createMap(fdCatche,CATCHE_FILE_SIZE*sizeof(Data));
    dbio->catchePosition = 0;
    printf("catchePosition:%ld\n",dbio->catchePosition);
    //create & open indexfile
    int fdIndex = createDoc(dir,"indexFile",DATA_FILE_SIZE*sizeof(IndexNode));
    dbio->indexList = (IndexNode*)createMap(fdIndex,DATA_FILE_SIZE*sizeof(IndexNode));
    dbio->indexPosition = 0;
    printf("indexPosition:%ld\n",dbio->indexPosition);
    return dbio;
}

bool writeData(Dbio *dbio, DeltaItem *deltaItem, uint64_t version)
{
    printf("enter writeData\nwrite data's info: key:%ld, version:%ld,field:%d\n",deltaItem->key,version,deltaItem->delta[0]);
    bool result = false;
    if(!deltaItem)
    {
        printf("delta is empty\n");
        return result;
    }
    //write delta's key & version into indexfile
    dbio->indexList[dbio->indexPosition].key = deltaItem->key;
    dbio->indexList[dbio->indexPosition].version = version;
    dbio->indexPosition++;
    printf("write into indexfile: key:%ld, version:%ld, indexPosition:%ld\n",dbio->indexList[dbio->indexPosition-1].key,dbio->indexList[dbio->indexPosition-1].version,dbio->indexPosition);
    //write data into catche file
    dbio->catcheList[dbio->catchePosition].key = deltaItem->key;
    dbio->catcheList[dbio->catchePosition].version = version;
    for (size_t i = 0; i < DATA_FIELD_NUM; i++)
    {
        dbio->catcheList[dbio->catchePosition].field[i] = deltaItem->delta[i];
    }
    dbio->catchePosition++;
    printf("write into catchefile: key:%ld, version:%ld, field value:%ld,catchePosition:%ld\n",dbio->catcheList[dbio->catchePosition-1].key,dbio->catcheList[dbio->catchePosition-1].version,dbio->catcheList[dbio->catchePosition-1].field[0],dbio->catchePosition);
    for (size_t j = 0; j < dbio->catchePosition; j++)
    {
        printf("catch index:%ld, key:%ld, version:%ld, field:%ld\n",j,dbio->catcheList[j].key,dbio->catcheList[j].version,dbio->catcheList[j].field[0]);
    }
    result = true;
    //if catche file is full ,write catche into datafile and clear catche
    if(dbio->catchePosition >= CATCHE_FILE_SIZE)
    {
        printf("dataPosition:%ld\n",dbio->dataPosition);
        result = pwrite(dbio->fdData,dbio->catcheList,CATCHE_FILE_SIZE*sizeof(Data),dbio->dataPosition);
        dbio->dataPosition +=  CATCHE_FILE_SIZE*sizeof(Data);
        dbio->catchePosition = 0;
        printf("catche clear\n");
    }
    return result;
}

bool readData(Dbio *dbio, uint64_t offset, Data *data)
{
    bool result = false;
    if (offset >= dbio->indexPosition)
    {
        printf("data inexist\n");
        return result;
    }
    else
    {
        //if data is in catche.get from catche.
        if(offset >= dbio->dataPosition)
        {
            //get data's offset in catche
            offset = offset % CATCHE_FILE_SIZE;
            //get data
            data->key = dbio->catcheList[offset].key;
            data->version = dbio->catcheList[offset].version;
            for (size_t i = 0; i < DATA_FIELD_NUM; i++)
            {
                data->field[i] = dbio->catcheList[offset].field[i];
            }
            result = true;
        }
        //if data is in datafile, read from datafile
        else
        {
            pread(dbio->fdData,data,sizeof(Data),sizeof(Data)*offset);
            result = true;
        }
    }
    return result;
}