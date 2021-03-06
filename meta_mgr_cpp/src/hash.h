#pragma once
#include <stdio.h>
#include "store_intf.h"
#include <string.h>

#define HASH_LEN 80000
#define HASH_PRIME_NUM 79999
#define NULLKEY 0

//hash表中元素指向的链表节点
struct HashLinkNode
{
	Data data;
	HashLinkNode *next;
} ;

//hash表元素
struct HashTable
{
	uint64_t key;
	HashLinkNode *head;
};

HashLinkNode *CreateHashLinkNode(const DeltaItem *deltaItem, uint64_t version);
HashLinkNode *HashLinkFind(HashLinkNode *head, uint64_t version);
HashLinkNode *HashLinkInsert(HashLinkNode *head, const DeltaItem *deltaItem, uint64_t version);
uint64_t HashGetPosition(uint64_t key);
bool HashInsert(HashTable *hashList, const DeltaItem *deltaItem, uint64_t version);
bool HashSearch(HashTable *hashList, uint64_t key, uint64_t version, Data &data);