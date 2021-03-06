#include "dbio.h"

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
    sprintf(dataPath, "%s/%s", dir, filename);
    int accessresult = access(dataPath, 0);
    int fd = 0;
    if (accessresult == -1)
    {
        printf("createDoc datapath: %s\n", dataPath);
        //create and open file writable & readable
        fd = open(dataPath, O_CREAT | O_RDWR | O_TRUNC, 00777);
        //allocate space
        fallocate(fd, 0, 0, dataSize);
        //trim purplus space
        int fresult = ftruncate(fd, dataSize);
        if (fresult < 0)
        {
            printf("error ftruncate:%d,%s\n", fresult, dataPath);
        }
    }
    else
    {
        printf("createDoc datapath open: %s\n", dataPath);
        fd = open(dataPath, O_RDWR);
    }
    return fd;
}

void *createMap(int fd, size_t len)
{
    void *head = (void *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return head;
}

char *loadZeroNode(const char *dir, const char *filename)
{
    char dataPath[256] = " ";
    //combine dir with filename
    sprintf(dataPath, "%s/%s", dir, filename);
    int accessresult = access(dataPath, 0);
    char *ZeroText = NULL;
    if (accessresult == -1)
    {
        printf("loadZeroNode datapath: %s\n", dataPath);
    }
    else
    {
        printf("loadZeroNode datapath: %s\n", dataPath);

        FILE *fd = fopen(dataPath, "r");
        fseek(fd, 0, SEEK_END);
        long lSize = ftell(fd);
        ZeroText = (char *)malloc(lSize + 1);
        rewind(fd);
        fread(ZeroText, sizeof(char), lSize, fd);
        ZeroText[lSize] = '\0';
        fclose(fd);
    }
    return ZeroText;
}

Dbio *initDbio(const char *dir)
{
    //create dbio's pointer for storing datafile catchefile indexfile
    Dbio *dbio = (Dbio *)malloc(sizeof(Dbio));
    //create dir
    createDir(dir);
    //create & open datafile
    dbio->fdData = createDoc(dir, "dataFile", DATA_FILE_SIZE * sizeof(Data));
    dbio->dataPosition = 0;
    //create & open catcheflie
    dbio->fdCatche = createDoc(dir, "catcheFile", CATCHE_FILE_SIZE * sizeof(Data));
    dbio->catcheList = (Data *)createMap(dbio->fdCatche, CATCHE_FILE_SIZE * sizeof(Data));
    dbio->catchePosition = 0;
    //create & open indexfile
    dbio->fdIndex = createDoc(dir, "indexFile", DATA_FILE_SIZE * sizeof(IndexNode));
    dbio->indexList = (IndexNode *)createMap(dbio->fdIndex, DATA_FILE_SIZE * sizeof(IndexNode));
    dbio->indexPosition = 0;

    dbio->zeroNode = (ZeroNode *)loadZeroNode(dir, "zeroFile");
    return dbio;
}

void deinitDbio(Dbio *dbio)
{
    if (dbio->zeroNode)
    {
        free(dbio->zeroNode);
        dbio->zeroNode = NULL;
    }
    munmap(dbio->catcheList, CATCHE_FILE_SIZE * sizeof(Data));
    munmap(dbio->indexList, DATA_FILE_SIZE * sizeof(IndexNode));
    close(dbio->fdData);
    close(dbio->fdIndex);
    close(dbio->fdCatche);
    free(dbio);
    dbio = NULL;
}

bool writeData(Dbio *dbio, const DeltaItem *deltaItem, uint64_t version)
{
    bool result = false;
    if (!deltaItem)
    {
        printf("delta is empty\n");
        return result;
    }
    //write delta's key & version into indexfile
    dbio->indexList[dbio->indexPosition].key = deltaItem->key;
    dbio->indexList[dbio->indexPosition].version = version;
    dbio->indexPosition++;
    //write data into catche file
    dbio->catcheList[dbio->catchePosition].key = deltaItem->key;
    dbio->catcheList[dbio->catchePosition].version = version;
    for (size_t i = 0; i < DATA_FIELD_NUM; i++)
    {
        dbio->catcheList[dbio->catchePosition].field[i] = deltaItem->delta[i];
    }
    dbio->catchePosition++;
    result = true;
    //if catche file is full ,write catche into datafile and clear catche
    if (dbio->catchePosition >= CATCHE_FILE_SIZE)
    {
        ssize_t presult = pwrite(dbio->fdData, dbio->catcheList, CATCHE_FILE_SIZE * sizeof(Data), dbio->dataPosition);
        if (presult < 0)
        {
            printf("error pwrite:%lu\n", presult);
        }
        dbio->dataPosition += CATCHE_FILE_SIZE;
        dbio->catchePosition = 0;
        printf("catche clear\n");
    }
    return result;
}

bool readData(Dbio *dbio, uint64_t offset, Data *data)
{
    bool result = false;
    // uint64_t offset = 0;
    // //when offset < indexPosition, find target data's index,when offset >= indexposition, target data inexists
    // while(offset < dbio->indexPosition)
    // {
    //     printf("enter while\n");
    //     //when key and version match, index found. when not match, keep looking
    //     if(dbio->indexList[offset].key == key || dbio->indexList[offset].version == version)
    //     {
    //         printf("enter if\n");
    //         break;
    //     }
    //     offset++;
    // }
    // printf("offset:%ld\n",offset);
    //when offset < indexPosition, the data exist. when offset >= dataPosition, the data is in catche.
    if (offset >= dbio->indexPosition)
    {
        printf("data inexist\n");
        return result;
    }
    else
    {
        //if data is in catche.get from catche.
        if (offset >= dbio->dataPosition)
        {
            printf("enter second if\n");
            //get data's offset in catche
            offset = offset % CATCHE_FILE_SIZE;
            printf("offset:%ld\n", offset);
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
            printf("enter second else\n");
            ssize_t presult = pread(dbio->fdData, data, sizeof(Data), sizeof(Data) * offset);
            if (presult < 0)
            {
                printf("error pread:%lu\n", presult);
            }
            result = true;
        }
    }
    return result;
}