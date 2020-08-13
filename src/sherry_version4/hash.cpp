#include "hash.h"
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>    
#include <sys/stat.h>  
#include <sys/mman.h>  
#include <fcntl.h>
#include <unistd.h>

HashNode_64* hashInit()
{
    HashNode_64 *hashList = (HashNode_64*)malloc(sizeof(HashNode_64)*HASH_LEN);
    memset(hashList, 0, sizeof(HashNode_64)*HASH_LEN);
    return hashList;
}//done

uint64_t hashRecover(Dbio *dbio)
{
    uint64_t i = 0;
    HashNode_64 *hashList = hashInit();
    while ((dbio->indexList[i].key != 0) || (dbio->indexList[i].version != 0))
    {
        hashInsert(hashList, dbio->indexList[i].key, dbio->indexList[i].version, i);
        i++;
    }
    return i;
}

uint64_t HashGetPosition(uint64_t key)
{
	int index;
	index = key % HASH_PRIME_NUM;
	if (index == 0)
	{
		index = HASH_LEN-1;
	}
	return index;
}//done

uint64_t getHashIndex(HashNode_64 *hashList, uint64_t key)
{
    
    uint64_t index = HashGetPosition(key);
    if ((hashList[index].key == NULLKEY)|| (hashList[index].key == key))
    {
        return index;
    }
    else
    {
        for (size_t i = 1; i < HASH_LEN; i++)
		{
			uint64_t newIndex = (index + i) % HASH_LEN;
			if(newIndex == 0)
			{
				newIndex++;
			}
            if ((hashList[newIndex].key == NULLKEY)|| (hashList[newIndex].key == key))
            {
                return newIndex;
            }
            
		}
    }
    return 0;
}//done

// int getVersionCount(HashNode_64 *hashList, uint64_t key)
// {
//     int result = -1;
//     if(key == 0)
//     {
//         result =  hashList[0].versionCount;
//     }
//     else
//     {
//         uint64_t index = getHashIndex(hashList,key);
//         if (index != 0)
//         {
//             result =  hashList[index].versionCount;
//         }
//     }
//     return result;
// }

int getVersionCount2(HashNode_64 *hashList, uint64_t index)
{
    int result =  hashList[index].versionCount;
    return result;
}//done

bool hashInsertItem(HashNode_64 *hashList, uint64_t key, uint64_t version, uint64_t offset, uint64_t index)
{
    bool result = false;
    if(getVersionCount2(hashList,index) == 64)
    {
        printf("HashData is full!\n");
    }
    else
    {
        hashList[index].key = key;
        hashList[index].versionList[hashList[index].versionCount].version = version;
        hashList[index].versionList[hashList[index].versionCount].indexOffset = offset;
        hashList[index].versionCount++;
        result = true;    
    }
    return result;
}//done

bool hashInsert(HashNode_64 *hashList, uint64_t key, uint64_t version, uint64_t offset)
{
    bool result = false;
    if(key == 0)
    {
        if (version == 0)
        {
            result == true;
        }
        else
        {
            result = hashInsertItem(hashList,key,version,offset,0);
        }
    }
    else
    {
        uint64_t index = getHashIndex(hashList, key);
        if(index!= 0)
        {
            result = hashInsertItem(hashList,key,version,offset,index);
        } 
    }
    return result;   
}//done

// bool hashDirectInsert(HashNode_64 *hashList, uint64_t key, uint64_t version, uint64_t index, uint64_t offset)
// {
//     bool result = false;
//     if (hashList[index].key == NULLKEY)
//     {
//         hashList[index].key = key;
//         hashList[index].versionList[hashList[index].versionCount].version = version;
//         hashList[index].versionList[hashList[index].versionCount].indexOffset = offset;
//         hashList[0].versionCount++;
//         result = true;
//     }
//     else if (hashList[index].key == key)
//     {
//         hashList[index].versionList[hashList[index].versionCount].version = version;
//         hashList[index].versionList[hashList[index].versionCount].indexOffset = offset;
//         hashList[0].versionCount++;
//         result = true;    
//     }
//     return result;
// }

// bool hashInsert(HashNode_64 *hashList, uint64_t key, uint64_t version, uint64_t offset)
// {
//     bool result = false;
//     if(key == 0)
//     {
//         hashList[0].key = key;
//         hashList[0].versionList[hashList[0].versionCount].version = version;
//         hashList[0].versionList[hashList[0].versionCount].indexOffset = offset;
//         hashList[0].versionCount++;
//         result = true;
//     }
//     uint64_t index = HashGetPosition(key);
//     result = hashDirectInsert(hashList,key,version,index,offset);
//     if (result = false)    
//     {
//         for (size_t i = 1; i < HASH_LEN; i++)
// 		{
// 			uint64_t newIndex = (index + i) % HASH_LEN;
// 			if(newIndex == 0)
// 			{
// 				newIndex++;
// 			}
//             result = hashDirectInsert(hashList,key,version,newIndex,offset);
//             if(result == true)
// 			{
// 				break;
// 			}
// 		}
//     }
//     return result;
// }

int hashNodeSearch(HashNode_64 *hashList, uint64_t index, uint64_t version, uint64_t *applyList)
{
    int count = 0;
    for (size_t i = 0; i < hashList[index].versionCount; i++)
    {
        if (hashList[index].versionList[i].version <= version)
        {
            applyList[count] = hashList[index].versionList[i].indexOffset;
            count++;
        }
    }
    return count;  
}//done

int hashSearch(HashNode_64 *hashList,uint64_t key, uint64_t version, uint64_t *applyList)
{	
    int applyCount = 0;
    if (key == 0)
	{
        applyCount = hashNodeSearch(hashList,0,version,applyList);
	}
    uint64_t index = HashGetPosition(key);
	if (hashList[index].key == NULLKEY);
	else if (hashList[index].key == key)
	{
        applyCount = hashNodeSearch(hashList,index,version,applyList);
	}
	else
	{
        for (size_t i = 1; i < HASH_LEN; i++)
		{
			uint64_t newIndex = (index + i) % HASH_LEN;
			if(newIndex == 0)
			{
				newIndex++;
			}
			if (hashList[newIndex].key == NULLKEY);
			else if (hashList[newIndex].key == key)
			{
				applyCount = hashNodeSearch(hashList,newIndex,version,applyList);
			}
		}
	} 
    return applyCount;
}//done

bool writeData(Dbio *dbio, HashNode_64 *hashList, const DeltaPacket &deltaPacket)
{
    bool ioResult, hashResult, result = false;
    for (size_t i = 0; i < deltaPacket.delta_count; i++)
    {
        ioResult = writeIO(dbio,deltaPacket.deltas[i],deltaPacket.version);
        hashResult = hashInsert(hashList,deltaPacket.deltas[i].key,deltaPacket.version,dbio->indexPosition-1);
        if(hashResult == false)
        {
        }
    }
    if((ioResult == true)&&(hashResult == true))
    {
        result = true;
    }
    return result;
}
bool readData(Dbio *dbio, HashNode_64 *hashList,uint64_t key, uint64_t version, Data &data)
{
    bool result = false;
    size_t i,j;
    DeltaItem *applyDelta = (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *singleDelta = (DeltaItem*)malloc(sizeof(DeltaItem));
    uint64_t *applyList = (uint64_t*)malloc(sizeof(uint64_t)*VERSIONCOUNT);
    int versionCount = hashSearch(hashList,key,version,applyList);
    if((versionCount == 0) && (key != 0 || version !=0));
    else
    {
        for (i = 0; i < versionCount; i++)
        {
            readIO(dbio,applyList[i],singleDelta);
            for (j = 0; j < DATA_FIELD_NUM; j++)
            {
                applyDelta->delta[j] += singleDelta->delta[j];
            }     
        }
        if(key == 0)
        {
            readZero(dbio,singleDelta);
            for (i = 0; i < DATA_FIELD_NUM; i++)
            {
                applyDelta->delta[i] += singleDelta->delta[i]; 
            } 
        }
        data.key = key;
        data.version = version;
        for (i = 0; i < DATA_FIELD_NUM; i++)
        {
            data.field[i] = applyDelta->delta[i];
        }
        result = true;      
    }
    return result;
}

// int main(int argc, char **argv)
// {
//     Dbio *hashio = initDbio("hashio");
//     HashNode_64 *hashlist = hashInit();
//     DeltaPacket packet1 = {};
//     DeltaPacket packet2 = {};
//     DeltaPacket packet3 = {};
//     DeltaPacket packet4 = {};
//     size_t i,j;
//     printf("--------------------packet1----------version 0,key 0-44\n");
//     packet1.version = 0;
//     packet1.delta_count = 45;
//     printf("p1 deltacount: %d, version: %ld\n",packet1.delta_count,packet1.version);
//     for (i = 0; i < packet1.delta_count; i++)
//     {
//         printf("i:%ld\n",i);
//         packet1.deltas[i].key = i;
//         printf("    packet1.deltas[%ld].key:%ld\n",i,packet1.deltas[i].key);
//         for (j = 0; j < 64; j++)
//         {
//             packet1.deltas[i].delta[j] = i*100+1;
//         }
//         printf("    packet1.deltas[%ld].delta:%d\n",i,packet1.deltas[i].delta[63]);
//     }
//     printf("--------------------packet2----------version 50,key 0-44\n");
//     packet2.version = 50;
//     packet2.delta_count = 45;
//     printf("p2 deltacount: %d, version: %ld\n",packet2.delta_count,packet2.version);
//     for (i = 0; i < packet2.delta_count; i++)
//     {
//         printf("i:%ld\n",i);
//         packet2.deltas[i].key = i;
//         printf("    packet2.deltas[%ld].key:%ld\n",i,packet2.deltas[i].key);
//         for (j = 0; j < 64; j++)
//         {
//             packet2.deltas[i].delta[j] = i*100+1;
//         }
//         printf("    packet2.deltas[%ld].delta:%d\n",i,packet2.deltas[i].delta[63]);
        
//     }
//     printf("--------------------packet3----------version 15,key 25-69\n");
//     packet3.version = 15;
//     packet3.delta_count = 45;
//     int count = 25;
//     printf("p3 deltacount: %d, version: %ld\n",packet3.delta_count,packet3.version);
//     for (size_t i = 0; i < packet3.delta_count; i++)
//     {
//         printf("i:%ld\n",i);
//         packet3.deltas[i].key = count;
//         printf("    packet3.deltas[%ld].key:%ld\n",i,packet3.deltas[i].key);
//         for (size_t j = 0; j < 64; j++)
//         {
//             packet3.deltas[i].delta[j] = count*100+1;
//         }
//         count++;
//         printf("    packet3.deltas[%ld].delta:%d\n",i,packet3.deltas[i].delta[63]);
//     }
//     printf("--------------------packet4----------version 95,key 0-44\n");
//     packet4.version = 95;
//     packet4.delta_count = 45;
//     printf("p4 deltacount: %d, version: %ld\n",packet4.delta_count,packet4.version);
//     for (size_t i = 0; i < packet4.delta_count; i++)
//     {
//         printf("i:%ld\n",i);
//         packet4.deltas[i].key = i;
//         printf("    packet4.deltas[%ld].key:%ld\n",i,packet4.deltas[i].key);
//         for (size_t j = 0; j < 64; j++)
//         {
//             packet4.deltas[i].delta[j] = i*100+1;
//         }
//         printf("    packet4.deltas[%ld].delta:%d\n",i,packet4.deltas[i].delta[63]);
//     }
    
//     bool wr1 = writeData(hashio,hashlist,packet1);
//     bool wr2 = writeData(hashio,hashlist,packet2);
//     bool wr3 = writeData(hashio,hashlist,packet3);
//     bool wr4 = writeData(hashio,hashlist,packet4);
//     printf("write result  1:%d  2:%d  3:%d  4:%d\n",wr1,wr2,wr3,wr4);
//     printf("-----indexList-----\n"); 
//     for (i = 0; i < hashio->indexPosition; i++)
//     {
//         printf("key:%ld,version:%ld\n",hashio->indexList[i].key,hashio->indexList[i].version);
//     }
//     printf("-----deltaFile-----\n"); 
//     DeltaItem *fileList = (DeltaItem*)malloc(sizeof(DeltaItem)*150);
//     pread(hashio->fdDelta,fileList,sizeof(DeltaItem)*hashio->deltaPosition,0);
//     for (i = 0; i < 150; i++)
//     {
//         printf("i:%ld, key:%ld, delta:%d\n",i,fileList[i].key,fileList[i].delta[63]);
//     }
//     printf("-----catchFile-----\n"); 
//     for (i = 0; i < hashio->catchePosition; i++)
//     {
//         printf("i:%ld, key:%ld, delta:%d\n",i,hashio->catcheList[i].key,hashio->catcheList[i].delta[63]);
//     }
//     DeltaItem *zero = (DeltaItem*)malloc(sizeof(DeltaItem));
//     pread(hashio->fdZero,zero,sizeof(DeltaItem),0);
//     for (i = 0; i < 64; i++)
//     {
//         printf("key:%ld, i:%ld, delta:%d\n",zero->key,i,zero->delta[i]);
//     }
//     printf("----------file print end----------------\n");
    // Data data1 = {};
    // printf("-----------read     key:0  version:0\n");
    // readData(hashio,hashlist,0,0,data1);
    // printf("read data1 done\n");
    // for (i = 0; i < 64; i++)
    // {
    //     printf("key:%ld, version:%ld, i:%ld, field:%ld\n",data1.key,data1.version,i,data1.field[i]);
    // }
    // printf("-----------read     key:0  version:0\n");
    // Data data2 = {};
    // readData(hashio,hashlist,0,100,data2);
    // for (i = 0; i < 64; i++)
    // {
    //     printf("key:%ld, version:%ld, i:%ld, field:%ld\n",data2.key,data2.version,i,data2.field[i]);
    // }
    // printf("-----------read     key:60  version:10\n");
    // Data data3 = {};
    // readData(hashio,hashlist,60,10,data3);
    // for (i = 0; i < 64; i++)
    // {
    //     printf("key:%ld, version:%ld, i:%ld, field:%ld\n",data3.key,data3.version,i,data3.field[i]);
    // }
    // printf("-----------read     key:15  version:100\n");
    // Data data4 = {};
    // readData(hashio,hashlist,25,100,data4);
    // for (i = 0; i < 64; i++)
    // {
    //     printf("key:%ld, version:%ld, i:%ld, field:%ld\n",data4.key,data4.version,i,data4.field[i]);
    // }
      
    
    // uint64_t p1 = HashGetPosition(0);
    // uint64_t p2 = HashGetPosition(26);
    // uint64_t p3 = HashGetPosition(79999);
    // uint64_t p4 = HashGetPosition(159998);
    // printf("p1:%ld,p2:%ld,p3:%ld,p4:%ld\n",p1,p2,p3,p4);
    // HashNode_64 *list = hashInit();
    // for (size_t i = 0; i < 64; i++)
    // {
    //     hashInsertItem(list,5,i,i,50);
    //     hashInsertItem(list,5,i,i*2,50);
    // }
    // for (size_t i = 0; i < 128; i++)
    // {
    //     printf("index:%ld,versioncount:%d,version:%ld,offset:%ld\n",i,list[50].versionCount,list[50].versionList[i].version,list[50].versionList[i].indexOffset);
    // }
    // printf("--------------insert key == 0---------------\n");
    // hashInsert(list,0,0,0);
    // hashInsert(list,0,10,10);
    // hashInsert(list,0,5,5);
    // hashInsert(list,0,8,8);
    // hashInsert(list,0,15,15);
    // printf("--------------insert key == 79999---------------\n");
    // hashInsert(list,79999,15,15);
    // hashInsert(list,79999,5,5);
    // hashInsert(list,79999,10,10);
    // hashInsert(list,79999,20,20);
    // printf("--------------insert key == 159998---------------\n");
    // hashInsert(list,159998,15,15);
    // hashInsert(list,159998,0,1);
    // hashInsert(list,159998,10,10);
    // hashInsert(list,159998,20,20);
    // printf("--------------insert key == 80000---------------\n");
    // hashInsert(list,80000,15,15);
    // printf("--------------insert key == 1---------------\n");
    // hashInsert(list,1,15,15);
    // for (size_t i = 0; i < 64; i++)
    // {
    //     printf("index:%ld,key:%ld,versioncount:%d,version:%ld,offset:%ld\n",i,list[0].key,list[0].versionCount,list[0].versionList[i].version,list[0].versionList[i].indexOffset);
    // }
    // for (size_t i = 0; i < 64; i++)
    // {
    //     printf("index:%ld,key:%ld,versioncount:%d,version:%ld,offset:%ld\n",i,list[79999].key,list[79999].versionCount,list[79999].versionList[i].version,list[79999].versionList[i].indexOffset);
    // }
    // for (size_t i = 0; i < 64; i++)
    // {
    //     printf("index:%ld,key:%ld,versioncount:%d,version:%ld,offset:%ld\n",i,list[1].key,list[1].versionCount,list[1].versionList[i].version,list[1].versionList[i].indexOffset);
    // }
    // for (size_t i = 0; i < 64; i++)
    // {
    //     printf("index:%ld,key:%ld,versioncount:%d,version:%ld,offset:%ld\n",i,list[2].key,list[2].versionCount,list[2].versionList[i].version,list[2].versionList[i].indexOffset);
    // }
    // for (size_t i = 0; i < 64; i++)
    // {
    //     printf("index:%ld,key:%ld,versioncount:%d,version:%ld,offset:%ld\n",i,list[3].key,list[3].versionCount,list[3].versionList[i].version,list[3].versionList[i].indexOffset);
    // }
    // int a = getVersionCount2(list,0);
    // int b = getVersionCount2(list,5);
    // int c = getVersionCount2(list,79999);
    // int d = getVersionCount2(list,1);
    // int e = getVersionCount2(list,2);
    // int f = getVersionCount2(list,3);
    // printf("a:%d,b:%d,c:%d,d:%d,e:%d,f:%d\n",a,b,c,d,e,f);

    // uint64_t *applyList = (uint64_t*)malloc(sizeof(uint64_t)*64);
    // printf("-------------search key==0,version==0--------------\n");
    // int count1 = hashSearch(list,0,0,applyList);
    // printf("count1:%d\n",count1);
    // printf("-------------search key==0,version==10--------------\n");  
    // memset(applyList,0,sizeof(uint64_t));  
    // int count2 = hashSearch(list,0,10,applyList);
    // printf("count2:%d\n",count2);
    // for (size_t i = 0; i < count2; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // }
    // printf("-------------search key==0,version==20--------------\n");    
    // memset(applyList,0,sizeof(uint64_t));
    // int count3 = hashSearch(list,0,20,applyList);
    // printf("count3:%d\n",count3);   
    // for (size_t i = 0; i < count3; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // }     
    // printf("-------------search key==79999,version==1--------------\n");    
    // memset(applyList,0,sizeof(uint64_t));
    // int count4 = hashSearch(list,79999,1,applyList);
    // printf("coun4:%d\n",count4);   
    // for (size_t i = 0; i < count4; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // } 
    // printf("-------------search key==79999,version==18--------------\n");    
    // memset(applyList,0,sizeof(uint64_t));
    // int count5 = hashSearch(list,79999,18,applyList);
    // printf("count5:%d\n",count5);   
    // for (size_t i = 0; i < count5; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // } 
    // printf("-------------search key==79999,version==25--------------\n");    
    // memset(applyList,0,sizeof(uint64_t));
    // int count6 = hashSearch(list,79999,25,applyList);
    // printf("count6:%d\n",count6);   
    // for (size_t i = 0; i < count6; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // }     
    // printf("-------------search key==159998,version==0--------------\n");    
    // memset(applyList,0,sizeof(uint64_t));
    // int count7 = hashSearch(list,159998,0,applyList);
    // printf("count7:%d\n",count7);   
    // for (size_t i = 0; i < count7; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // } 
    // printf("-------------search key==159998,version==18--------------\n");    
    // memset(applyList,0,sizeof(uint64_t));
    // int count8 = hashSearch(list,159998,18,applyList);
    // printf("count8:%d\n",count8);   
    // for (size_t i = 0; i < count8; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // } 
    // printf("-------------search key==159998,version==20--------------\n");    
    // memset(applyList,0,sizeof(uint64_t));
    // int count9 = hashSearch(list,159998,20,applyList);
    // printf("count9:%d\n",count9);   
    // for (size_t i = 0; i < count9; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // }     
    // printf("-------------search inexist key--------------\n");    
    // memset(applyList,0,sizeof(uint64_t));
    // int count10 = hashSearch(list,55,20,applyList);
    // printf("count10:%d\n",count10);   
    // for (size_t i = 0; i < count10; i++)
    // {
    //     printf("offset:%ld\n",applyList[i]);
    // }  

    // printf("size of hashnode64:%ld\n",sizeof(HashNode_64));       
// }
