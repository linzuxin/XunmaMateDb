#include "common.h"

#define HASH_LEN 80000
#define HASH_PRIME_NUM 79999
#define NULLKEY 0

//hash表中元素指向的链表节点
typedef struct HashLinkNode
{
	uint64_t version;
	uint64_t indexOffset;
	HashLinkNode *next;

}HashLinkNode;

//hash表元素
typedef struct HashItem
{
	uint64_t key;
	HashLinkNode *head;
}HashItem;

HashLinkNode* HashLinkNodeCreate(uint64_t version);
HashLinkNode* HashLinkFind(HashLinkNode* head, uint64_t version);
bool CurrHashDataApplyOnly(Dbio *dbio, HashLinkNode *node,DeltaItem *deltaItem);
bool CurrHashDataApply(Dbio *dbio, HashLinkNode *node, HashLinkNode *bnode, DeltaItem *deltaItem);
bool ForwHashDataApply(Dbio *dbio,HashLinkNode *node, DeltaItem *deltaItem);
bool HashLinkInsert(Dbio *dbio,HashItem *hashList, uint64_t index, DeltaItem *deltaItem, uint64_t version);
uint64_t HashGetPosition(uint64_t key);
bool HashDirectInsert(Dbio *dbio, HashItem *hashList, DeltaItem *deltaItem, uint64_t index, uint64_t version);
bool HashInsert(Dbio *dbio, HashItem *hashList, DeltaItem *deltaItem, uint64_t version);
bool HashSearchNode(Dbio *dbio, HashItem *hashList,uint64_t index, uint64_t version, Data &data);
bool HashSearch(Dbio *dbio,HashItem *hashList, uint64_t key, uint64_t version, Data &data);
bool HashLinkInit(Dbio *dbio,HashItem *hashList, uint64_t index, uint64_t offset);
bool HashDirectInit(Dbio *dbio, HashItem *hashList, uint64_t index, uint64_t offset);
bool HashItemInit(Dbio *dbio, HashItem *hashList, uint64_t offset);
bool hashInit(Dbio *dbio, HashItem *hashList);


