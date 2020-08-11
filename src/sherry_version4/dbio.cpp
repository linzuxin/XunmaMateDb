#include <sys/types.h>    
#include <sys/stat.h>  
#include <sys/mman.h>  
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "dbio.h"

int createDir(const char *dir)
{
    //estimate dir's existence 
    int accessresult = access(dir, 0);
    int mkdirresult = -1;
    //if dir inexist, mkdir
    if (accessresult == -1)
    {
        mkdirresult = mkdir(dir, 0755);
    }
    return mkdirresult;
}//done

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
}//done

void* createMap(int fd, size_t len)
{
    void *head = (void*)mmap(NULL, len,  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return head;
}//done

Dbio* initDbio(const char *dir)
{
    //create dbio's pointer for storing datafile catchefile indexfile
    Dbio *dbio = (Dbio*)malloc(sizeof(Dbio));
    //create dir
    createDir(dir);
    //create & open datafile
    dbio->fdDelta = createDoc(dir,"dataFile",DATA_FILE_SIZE*sizeof(DeltaItem));
    dbio->deltaPosition = 0;
    //create & open catcheflie
    int fdCatche = createDoc(dir,"catcheFile",CATCHE_FILE_SIZE*sizeof(DeltaItem));
    dbio->catcheList = (DeltaItem*)createMap(fdCatche,CATCHE_FILE_SIZE*sizeof(DeltaItem));
    dbio->catchePosition = 0;
    //create & open indexfile
    int fdIndex = createDoc(dir,"indexFile",DATA_FILE_SIZE*sizeof(IndexNode));
    dbio->indexList = (IndexNode*)createMap(fdIndex,DATA_FILE_SIZE*sizeof(IndexNode));
    dbio->indexPosition = 0;
    dbio->fdZero = createDoc(dir,"zeroFile",sizeof(DeltaItem));
    return dbio;
}//done

void recoverDbioIndex(Dbio* dbio, uint64_t indexPosition)
{
    dbio->indexPosition = indexPosition;
    dbio->catchePosition = indexPosition % CATCHE_FILE_SIZE;
    dbio->deltaPosition = (indexPosition - dbio->catchePosition) * sizeof(DeltaItem);
}

bool writeIO(Dbio *dbio, const DeltaItem &deltaItem, uint64_t version)
{
    bool result = false;
    if((deltaItem.key == 0) && (version == 0))
    {
        DeltaItem *tmp = (DeltaItem*)malloc(sizeof(DeltaItem));
        // tmp->key = deltaItem.key;
        // for (size_t i = 0; i < 64; i++)
        // {
        //     tmp->delta[i] = deltaItem.delta[i];
        // } 
        result = pwrite(dbio->fdZero,&deltaItem,sizeof(DeltaItem),0);
        // free(tmp);
        return result;
    }
    //write delta's key & version into indexfile
    dbio->indexList[dbio->indexPosition].key = deltaItem.key;
    dbio->indexList[dbio->indexPosition].version = version;
    dbio->indexPosition++;
    //write data into catche file
    dbio->catcheList[dbio->catchePosition].key = deltaItem.key;
    for (size_t i = 0; i < DATA_FIELD_NUM; i++)
    {
        dbio->catcheList[dbio->catchePosition].delta[i] = deltaItem.delta[i];
    }
    dbio->catchePosition++;
    result = true;
    //if catche file is full ,write catche into datafile and clear catche
    if(dbio->catchePosition >= CATCHE_FILE_SIZE)
    {
        result = pwrite(dbio->fdDelta,dbio->catcheList,CATCHE_FILE_SIZE*sizeof(DeltaItem),dbio->deltaPosition);
        dbio->deltaPosition +=  CATCHE_FILE_SIZE*sizeof(DeltaItem);
        dbio->catchePosition = 0;
        printf("catche clear\n");
    }
    return result;
}//done

bool readIO(Dbio *dbio, uint64_t offset, DeltaItem *deltaItem)
{
    bool result = false;
    if (offset >= dbio->indexPosition)
    {
        printf("data inexist\n");
        return result;
    }
    //if data is in catche.get from catche.
    else if(offset*sizeof(DeltaItem) >= dbio->deltaPosition)
    {
        //get data's offset in catche
        offset = offset % CATCHE_FILE_SIZE;
        //get data
        deltaItem->key = dbio->catcheList[offset].key;
        for (size_t i = 0; i < DATA_FIELD_NUM; i++)
        {
            deltaItem->delta[i] = dbio->catcheList[offset].delta[i];
        }
        result = true;
    }
    //if data is in datafile, read from datafile
    else
    {
        pread(dbio->fdDelta,deltaItem,sizeof(DeltaItem),offset*sizeof(DeltaItem));
        result = true;
    }
    return result;
}//done

void readZero(Dbio *dbio,DeltaItem *deltaItem)
{
    pread(dbio->fdZero,deltaItem,sizeof(DeltaItem),0);
}



int main(int argc, char **argv)
{
    // int result1 = createDir("testdir");
    // int result2 = createDir("testdir");
    // printf("result1:%d,result2:%d\n",result1,result2);
    // int docResult1 = createDoc("testdir","file",sizeof(int)*2);
    // printf("docResult1:%d,\n",docResult1);
    // int content[2] = {1,2};
    // size_t r1 = pwrite(docResult1,content,sizeof(int)*2,0);
    // printf("write result:%ld\n",r1);
    // int readResult[3]={};
    // pread(docResult1,readResult,sizeof(int)*2,0);
    // for (size_t i = 0; i < 3; i++)
    // {
    //     printf("readResult: index:%ld, value:%d\n",i,readResult[i]);
    // }
    // int content1[3] = {11,22,33};
    // size_t r2 = pwrite(docResult1,content1,sizeof(int)*3,0);
    // printf("write result:%ld\n",r2);
    // pread(docResult1,readResult,sizeof(int)*3,0);
    // for (size_t i = 0; i < 3; i++)
    // {
    //     printf("readResult: index:%ld, value:%d\n",i,readResult[i]);
    // }
    // int *map = (int*)createMap(docResult1,sizeof(int)*2);
    // for (size_t i = 0; i < 3; i++)
    // {
    //     printf("mapResult: index:%ld, value:%d\n",i,map[i]);
    // }
    printf("size of indexNode:%ld, size of DeltaItem:%ld\n",sizeof(IndexNode),sizeof(DeltaItem));
    Dbio *dbio = initDbio("dbio");
    DeltaItem d1 = {};
    DeltaItem *dr = (DeltaItem*)malloc(sizeof(DeltaItem));
    for (size_t i = 0; i < 130; i++)
    {
        d1.key = i;
        for (size_t j = 0; j < 64; j++)
        {
            d1.delta[j] = i+1;
        }
        writeIO(dbio,d1,i); 
    }
    printf("index position:%ld, delta position:%ld, catche position:%ld\n",dbio->indexPosition,dbio->deltaPosition,dbio->catchePosition);
    for (size_t i = 0; i < 140; i++)
    {
        bool result = readIO(dbio,i,dr);

        printf("count:%ld,result:%d,key:%ld,delta value:%d\n",i,result,dr->key,dr->delta[1]);
    }
    printf("dbio's last:    catch key:%ld,catch delta:%d,index key:%ld,index version:%ld\n",dbio->catcheList[dbio->catchePosition-1].key,dbio->catcheList[dbio->catchePosition-1].delta[0],dbio->indexList[dbio->indexPosition-1].key,dbio->indexList[dbio->indexPosition-1].version);
    pread(dbio->fdZero,dr,sizeof(DeltaItem),0);
    printf("-------------zero---------\n");
    for (size_t i = 0; i < 64; i++)
    {
        printf("key:%ld,delta index:%ld,delta value:%d\n",dr->key,i,dr->delta[i]);
    }
    
}