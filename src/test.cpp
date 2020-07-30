#include "dbio.h"

int main()
{
    //test createDir & createDoc
    printf("-----------test createDir & createDoc--------------\n");
    const char *dir = "./newFile";
    createDir(dir);
    createDoc(dir,"newfile.txt",20);
    Dbio *dbio = initDbio("dbio");
    Data *data = (Data*)malloc(sizeof(Data));

    //test writeData (write into catche)
    printf("-----------test writeData (write into catche)------------\n");
    data->key = 88;
    data->version = 100;
    for (size_t i = 0; i < 64; i++)
    {
        data->field[i] = i;
    }
    bool result = writeData(dbio, data);
    printf("result :%d\n",result);
    printf("key:%ld, version:%ld, indexPosition:%ld\n",dbio->indexList[0].key, dbio->indexList[0].version,dbio->indexPosition);
    for (size_t j = 0; j < 64; j++)
    {
        printf("key:%ld, version:%ld, catchPosotion:%ld, fieldIndex:%ld, fieldValue:%ld\n",dbio->catcheList->key,dbio->catcheList->version,dbio->catchePosition,j,dbio->catcheList->field[j]);
    }
    //test readData (read from catche)
    printf("-----------test readData (read from catche)------------\n");    
    Data *rData = (Data*)malloc(sizeof(Data));
    bool readResult = readData(dbio,88,100,rData);
    printf("read result: %d, key:%ld, version: %ld\n",readResult,rData->key,rData->version);
    for (size_t l = 0; l < 64; l++)
    {
        printf("field index:%ld, field value:%ld\n",l,rData->field[l]);
    }


    //test pwrite & pread
    printf("----------test pwrite & pread----------\n");
    Data *data1 = (Data*)malloc(sizeof(Data));
    data1->key = 99;
    data1->version = 8;
    for (size_t i = 0; i < 64; i++)
    {
        data1->field[i] = 50;
    }
    Data *data2 = (Data*)malloc(sizeof(Data));
    pwrite(dbio->fdData,data1,sizeof(Data),dbio->dataPosition);
    printf("dataPosition:%ld\n",dbio->dataPosition);
    pread(dbio->fdData,data2,sizeof(Data),0);
    for (size_t k = 0; k < 64; k++)
    {
        printf("key:%ld, version:%ld, fieldIndex:%ld, fieldValue:%ld\n",data2->key,data2->version,k,data2->field[k]);
    }



    
    


}