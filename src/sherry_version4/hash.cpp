#include "hash.h"

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

int getVersionCount(HashNode_64 *hashList, uint64_t index)
{
    int result =  hashList[index].versionCount;
    return result;
}//done

bool hashInsertItem(HashNode_64 *hashList, uint64_t key, uint64_t version, uint64_t offset, uint64_t index)
{
    bool result = false;
    if(getVersionCount(hashList,index) == 64)
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
}//done