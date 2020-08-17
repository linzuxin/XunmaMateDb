#include "hash.h"

HashNode_64* hashInit()
{
    HashNode_64 *hashList = (HashNode_64*)malloc(sizeof(HashNode_64)*HASH_LEN);
    memset(hashList, 0, sizeof(HashNode_64)*HASH_LEN);
    return hashList;
}//done

uint64_t hashRecover(Dbio *dbio, HashNode_64 *hashList)
{
    uint64_t i = 0;
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

int getVersionCount(HashNode_64 *hashList, uint64_t index)
{
    int result =  hashList[index].versionCount;
    return result;
}//done

bool hashInsertItem(HashNode_64 *hashList, uint64_t key, uint64_t version, uint64_t offset, uint64_t index)
{
    bool result = false;
    if(getVersionCount(hashList,index) == VERSIONCOUNT)
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
}//done

bool readData(Dbio *dbio, HashNode_64 *hashList,uint64_t key, uint64_t version, Data &data)
{
    bool result = false;
    size_t i,j;
    DeltaItem *applyDelta = (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *singleDelta = (DeltaItem*)malloc(sizeof(DeltaItem));
    uint64_t *applyList = (uint64_t*)malloc(sizeof(uint64_t)*VERSIONCOUNT);
    memset(applyDelta,0,sizeof(DeltaItem));
    memset(singleDelta,0,sizeof(DeltaItem));
    memset(applyList,0,sizeof(uint64_t)*VERSIONCOUNT);
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
        if((key == 0) && (dbio->zeroPosition != 0))
        {
            DeltaItem *zeroList = (DeltaItem*)malloc(sizeof(DeltaItem)*VERSIONCOUNT);
            memset(zeroList,0,sizeof(DeltaItem)*VERSIONCOUNT);
            readZero(dbio,zeroList);
            for (i = 0; i < VERSIONCOUNT; i++)
            {
                for (j = 0; j < DATA_FIELD_NUM; j++)
                {
                    applyDelta->delta[j] += zeroList[i].delta[j]; 
                }
            }
            free(zeroList);
        }
        data.key = key;
        data.version = version;
        for (i = 0; i < DATA_FIELD_NUM; i++)
        {
            data.field[i] = applyDelta->delta[i];
        }
        result = true;      
    }
    free(applyDelta);
    free(singleDelta);
    free(applyList);
    return result;
}//done



// int main(int argc, char *argv[])
// {
//     Dbio *dbio = initDbio("initIO");
//     HashNode_64 *hash = hashInit();
//     DeltaPacket zeroPacket = {};
//     zeroPacket.version = 0;
//     zeroPacket.delta_count = 10;
//     for (size_t i = 0; i <  zeroPacket.delta_count; i++)
//     {
//         zeroPacket.deltas[i].key = 0;
//         printf("packet index:%ld, key:%ld\n",i,zeroPacket.deltas[i].key);
//         for (size_t j = 0; j < 64; j++)
//         {
//             zeroPacket.deltas[i].delta[j] = i;
//             printf("delta key:%ld, delta index:%ld, delta:%d\n",zeroPacket.deltas[i].key,j,zeroPacket.deltas[i].delta[j]);
//         }  
//     }
//     bool result = writeData(dbio,hash,zeroPacket);
//     printf("result:%d\n",result);
//     printf("----------zerolist------------\n");
//     DeltaItem *zerolist = (DeltaItem*)mmap(NULL, sizeof(DeltaItem)*VERSIONCOUNT,  PROT_READ | PROT_WRITE, MAP_SHARED, dbio->fdZero, 0);
//     for (size_t i = 0; i < VERSIONCOUNT; i++)
//     {
//         printf("i:%ld,key:%ld,delta:%d\n",i,zerolist[i].key,zerolist[i].delta[63]);
//     }
//     printf("----------zerolist2------------\n");
//     DeltaItem *zerolist2 = (DeltaItem*)malloc(sizeof(DeltaItem)*VERSIONCOUNT);
//     pread(dbio->fdZero,zerolist,sizeof(DeltaItem)*VERSIONCOUNT,0);
//     for (size_t i = 0; i < VERSIONCOUNT; i++)
//     {
//         printf("i:%ld,key:%ld,delta:%d\n",i,zerolist[i].key,zerolist[i].delta[63]);
//     }

    

    

// }