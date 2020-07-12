#include <stdio.h>
#include "common.h"

#define HASH_LEN 30000
#define HASH_PRIME_NUM 29989
#define NULLKEY 0

//hash表中元素指向的链表节点
typedef struct HashLinkNode
{
	Data data;
	HashLinkNode *next;

}HashLinkNode;

//hash表元素
typedef struct HashItem
{
	uint64_t key;
	HashLinkNode *head;
}HashItem;

Data DeltaToData(DeltaItem item, uint64_t version);
HashLinkNode* HashLinkFind(HashLinkNode* head, uint64_t version);
HashLinkNode* HashLinkInsert(HashLinkNode *head, HashLinkNode *node);
int HashGetPosition(int key);
int HashInsert(HashItem *hashList, Data &data);
int HashSearch(HashItem *hashList, uint64_t key, uint64_t version, Data &data);