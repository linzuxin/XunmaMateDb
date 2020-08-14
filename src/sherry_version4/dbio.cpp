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
    dbio->fdZero = createDoc(dir,"zeroFile",sizeof(DeltaItem)*VERSIONCOUNT);
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
        result = pwrite(dbio->fdZero,&deltaItem,sizeof(DeltaItem),dbio->zeroPosition);
        dbio->zeroPosition += sizeof(DeltaItem);
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

void readZero(Dbio *dbio,DeltaItem *zeroList)
{
    pread(dbio->fdZero,zeroList,sizeof(DeltaItem)*VERSIONCOUNT,0);
}