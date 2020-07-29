#include "common.h"
#include <string.h>

#define HASH_LEN 80000
#define HASH_PRIME_NUM 79999
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

HashLinkNode* HashLinkNodeCreate(uint64_t key, uint64_t version);
HashLinkNode* HashLinkFind(HashLinkNode* head, uint64_t version);
void CurrHashDataApplyOnly(HashLinkNode *node,DeltaItem *deltaItem);
void CurrHashDataApply(HashLinkNode *node, HashLinkNode *bnode, DeltaItem *deltaItem);
void ForwHashDataApply(HashLinkNode *node, DeltaItem *deltaItem);
bool HashLinkInsert(HashItem *hashList, uint64_t index, DeltaItem *deltaItem, uint64_t version);
uint64_t HashGetPosition(uint64_t key);
bool HashDirectInsert(HashItem *hashList, DeltaItem *deltaItem, uint64_t index, uint64_t version);
bool HashInsert(HashItem *hashList, DeltaItem *deltaItem, uint64_t version);
bool HashSearchNode(HashItem *hashList,uint64_t index,uint64_t version, Data &data);
bool HashSearch(HashItem *hashList, uint64_t key, uint64_t version, Data &data);
