#include "dbio.h"


int main()
{
    printf("-----------test createDir & createDoc--------------\n");
    const char *dir = "./newFile";
    createDir(dir);
    createDoc(dir,"newfile.txt",20);
    Dbio *dbio = initDbio("dbio");

    // printf("-----------test writeData (write into catche)------------\n");
    // DeltaItem *deltaItem = (DeltaItem*)malloc(sizeof(DeltaItem));
    // deltaItem->key = 88;
    // for (size_t i = 0; i < 64; i++)
    // {
    //     deltaItem->delta[i] = i;
    // }
    // bool result = writeData(dbio, deltaItem,100);
    // printf("result :%d\n",result);
    // printf("key:%ld, version:%ld, indexPosition:%ld\n",dbio->indexList[0].key, dbio->indexList[0].version,dbio->indexPosition);
    // for (size_t j = 0; j < 64; j++)
    // {
    //     printf("key:%ld, version:%ld, catchPosotion:%ld, fieldIndex:%ld, fieldValue:%ld\n",dbio->catcheList->key,dbio->catcheList->version,dbio->catchePosition,j,dbio->catcheList->field[j]);
    // }
    // //test readData (read from catche)
    // printf("-----------test readData (read from catche)------------\n");    
    // Data *rData = (Data*)malloc(sizeof(Data));
    // bool readResult = readData(dbio,0,rData);
    // printf("read result: %d, key:%ld, version: %ld\n",readResult,rData->key,rData->version);
    // for (size_t l = 0; l < 64; l++)
    // {
    //     printf("field index:%ld, field value:%ld\n",l,rData->field[l]);
    // }


    // //test pwrite & pread
    // printf("----------test pwrite & pread----------\n");
    // Data *data1 = (Data*)malloc(sizeof(Data));
    // data1->key = 99;
    // data1->version = 8;
    // for (size_t i = 0; i < 64; i++)
    // {
    //     data1->field[i] = 50;
    // }
    // Data *data2 = (Data*)malloc(sizeof(Data));
    // pwrite(dbio->fdData,data1,sizeof(Data),dbio->dataPosition);
    // printf("dataPosition:%ld\n",dbio->dataPosition);
    // pread(dbio->fdData,data2,sizeof(Data),0);
    // for (size_t k = 0; k < 64; k++)
    // {
    //     printf("key:%ld, version:%ld, fieldIndex:%ld, fieldValue:%ld\n",data2->key,data2->version,k,data2->field[k]);
    // }

    // printf("-----------test writeData (write size over catche)------------\n");
    // DeltaItem *deltaItem1 = (DeltaItem*)malloc(sizeof(DeltaItem));
    // deltaItem1->key = 50;
    // for (size_t i = 0; i < 64; i++)
    // {
    //     deltaItem1->delta[i] = 50;
    // }
    // DeltaItem *deltaItem2 = (DeltaItem*)malloc(sizeof(DeltaItem));
    // deltaItem2->key = 150;
    // for (size_t i = 0; i < 64; i++)
    // {
    //     deltaItem2->delta[i] = 150;
    // }
    // DeltaItem *deltaItem3 = (DeltaItem*)malloc(sizeof(DeltaItem));
    // deltaItem3->key = 100;
    // for (size_t i = 0; i < 64; i++)
    // {
    //     deltaItem3->delta[i] = 100;
    // }
    // printf("write first\n");
    // writeData(dbio,deltaItem1,51);
    // printf("write second\n");
    // writeData(dbio,deltaItem2,52);
    // printf("write thrid\n");
    // writeData(dbio,deltaItem3,53);

    // Data *rData1 = (Data*)malloc(sizeof(Data));
    // readResult = readData(dbio,1,rData1);
    // printf("data1 read result: %d, key:%ld, version: %ld\n",readResult,rData1->key,rData1->version);
    // for (size_t l = 0; l < 64; l++)
    // {
    //     printf("field index:%ld, field value:%ld\n",l,rData1->field[l]);
    // }
    // Data *rData2 = (Data*)malloc(sizeof(Data));
    // readResult = readData(dbio,2,rData2);
    // printf("data2 read result: %d, key:%ld, version: %ld\n",readResult,rData2->key,rData2->version);
    // for (size_t l = 0; l < 64; l++)
    // {
    //     printf("field index:%ld, field value:%ld\n",l,rData2->field[l]);
    // }
    // Data *rData3 = (Data*)malloc(sizeof(Data));
    // readResult = readData(dbio,3,rData3);
    // printf("data3 read result: %d, key:%ld, version: %ld\n",readResult,rData3->key,rData3->version);
    // for (size_t l = 0; l < 64; l++)
    // {
    //     printf("field index:%ld, field value:%ld\n",l,rData3->field[l]);
    // }

    // printf("-----------test hash------------\n");
    // dbio = initDbio("dbio");
    // printf("dataPosition:%ld, indexPosition:%ld, catchePosition:%ld\n",dbio->dataPosition,dbio->indexPosition,dbio->catchePosition);
    // Data *rData0 = (Data*)malloc(sizeof(Data));
    // Data *rData0a = (Data*)malloc(sizeof(Data));
    // Data *rData0b = (Data*)malloc(sizeof(Data));
    // Data *rData0c = (Data*)malloc(sizeof(Data));
    // readResult = readData(dbio,0,rData0);
    // printf("data0 read result: %d, key:%ld, version: %ld\n",readResult,rData0->key,rData0->version);
    // for (size_t l = 0; l < 64; l++)
    // {
    //     printf("field index:%ld, field value:%ld\n",l,rData0->field[l]);
    // }
    // readResult = readData(dbio,1,rData0a);
    // printf("data0a read result: %d, key:%ld, version: %ld\n",readResult,rData0a->key,rData0a->version);
    // for (size_t l = 0; l < 64; l++)
    // {
    //     printf("field index:%ld, field value:%ld\n",l,rData0a->field[l]);
    // }   
    // readResult = readData(dbio,2,rData0b);
    // printf("data0b read result: %d, key:%ld, version: %ld\n",readResult,rData0b->key,rData0b->version);
    // for (size_t l = 0; l < 64; l++)
    // {
    //     printf("field index:%ld, field value:%ld\n",l,rData0b->field[l]);
    // } 
    // readResult = readData(dbio,3,rData0c);
    // printf("data0c read result: %d, key:%ld, version: %ld\n",readResult,rData0c->key,rData0c->version);
    // for (size_t l = 0; l < 64; l++)
    // {
    //     printf("field index:%ld, field value:%ld\n",l,rData0c->field[l]);
    // }
    uint64_t i = 0;
    Data *dd1 = (Data*)malloc(sizeof(Data));
    dd1->key = 101;
    dd1->version = 1001;
    for (i = 0; i < 64; i++)
    {
        dd1->field[i] = 10001;
    }
    Data *dd2 = (Data*)malloc(sizeof(Data));
    dd2->key = 202;
    dd2->version = 2002;
    for (i = 0; i < 64; i++)
    {
        dd2->field[i] = 20002;
    }
    Data *dd3 = (Data*)malloc(sizeof(Data));
    dd3->key = 303;
    dd3->version = 3003;
    for (i = 0; i < 64; i++)
    {
        dd3->field[i] = 30003;
    }

    dbio->catcheList[0].key = dd1->key;
    dbio->catcheList[0].version = dd1->version;
    for (i = 0; i < 64; i++)
    {
        dbio->catcheList[0].field[i] = dd1->field[i];
    }
    dbio->catcheList[1].key = dd2->key;
    dbio->catcheList[1].version = dd2->version;
    for (i = 0; i < 64; i++)
    {
        dbio->catcheList[1].field[i] = dd2->field[i];
    }
    dbio->catcheList[2].key = dd3->key;
    dbio->catcheList[2].version = dd3->version;
    for (i = 0; i < 64; i++)
    {
        dbio->catcheList[2].field[i] = dd3->field[i];
    }    
    
    for ( i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 64; j++)
        {
            printf("catch index:%ld, key:%ld, version:%ld, field index:%ld,value:%ld\n",i,dbio->catcheList[i].key,dbio->catcheList[i].version,j,dbio->catcheList[i].field[j]);
        }
    }
    pwrite(dbio->fdData,dbio->catcheList,3*sizeof(Data),dbio->dataPosition);
    Data *rd1 = (Data*)malloc(sizeof(Data));
    Data *rd2 = (Data*)malloc(sizeof(Data));  
    Data *rd3 = (Data*)malloc(sizeof(Data));      
    pread(dbio->fdData,rd1,sizeof(Data),sizeof(Data)*0);
    pread(dbio->fdData,rd2,sizeof(Data),sizeof(Data)*1);
    pread(dbio->fdData,rd3,sizeof(Data),sizeof(Data)*2);
    for (i = 0; i < 64; i++)
    {
        printf("rd1 key:%ld, version:%ld, field index:%ld, value:%ld\n",rd1->key,rd1->version,i,rd1->field[i]);
    }
    for (i = 0; i < 64; i++)
    {
        printf("rd2 key:%ld, version:%ld, field index:%ld, value:%ld\n",rd2->key,rd2->version,i,rd2->field[i]);
    }
    for (i = 0; i < 64; i++)
    {
        printf("rd3 key:%ld, version:%ld, field index:%ld, value:%ld\n",rd3->key,rd3->version,i,rd3->field[i]);
    }
    
    


}