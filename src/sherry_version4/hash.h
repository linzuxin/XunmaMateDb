#pragma once
#include "common.h"
#include "dbio.h"
#include "allocate.h"
#include <string.h>
#include <stdlib.h>

//hash表中元素指向的链表节点
typedef struct HashData
{
	uint64_t version;
	uint64_t indexOffset;
}HashData;

//hash表元素
typedef struct HashNode_64
{
	uint64_t key;
	HashData versionList[VERSIONCOUNT];
	int versionCount;
}HashNode_64;

HashNode_64* hashInit();
uint64_t hashRecover(Dbio *dbio,HashNode_64 *hashList);
uint64_t HashGetPosition(uint64_t key);
uint64_t getHashIndex(HashNode_64 *hashList, uint64_t key);
int getVersionCount(HashNode_64 *hashList, uint64_t index);
bool hashInsertItem(HashNode_64 *hashList, uint64_t key, uint64_t version, uint64_t offset, uint64_t index);
bool hashInsert(HashNode_64 *hashList, uint64_t key, uint64_t version, uint64_t offset);
int hashNodeSearch(HashNode_64 *hashList, uint64_t index, uint64_t version, uint64_t *applyList);
int hashSearch(HashNode_64 *hashList,uint64_t key, uint64_t version, uint64_t *applyList);
bool writeData(Dbio *dbio, HashNode_64 *hashList, const DeltaPacket &deltaPacket);
bool readData(Dbio *dbio, HashNode_64 *hashList,uint64_t key, uint64_t version, Data &data);