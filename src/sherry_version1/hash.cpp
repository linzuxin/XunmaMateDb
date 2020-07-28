#include "hash.h"
#include <string.h>

HashLinkNode* HashLinkNodeCreate(uint64_t key, uint64_t version)
{
	//声明一个链表节点并申请空间，用于存储Data。
	HashLinkNode *node = NULL;
	if(!(node = (HashLinkNode*)malloc(sizeof(HashLinkNode))))
	{
		return NULL;
	}
	memset(node,0,sizeof(HashLinkNode));
	//给链表节点赋值
	node->data.key = key;
	node->data.version = version;
	node->next = NULL;
	return node;
}

HashLinkNode* HashLinkFind(HashLinkNode* head, uint64_t version) 
{
	HashLinkNode* result = NULL;
	HashLinkNode* node = head;
	//循环查找，找到比当前版本小的最大版本所在节点
	while (node && node->data.version <= version)
	{
		result = node;
		node = node->next;
	}
	return result;
}

void CurrHashDataApplyOnly(HashLinkNode *node,DeltaItem *deltaItem)
{
	for (size_t i = 0; i < DATA_FIELD_NUM; i++)
	{
		node->data.field[i] += deltaItem->delta[i];
	}
}

void CurrHashDataApply(HashLinkNode *node, HashLinkNode *bnode, DeltaItem *deltaItem)
{
	for (size_t i = 0; i < DATA_FIELD_NUM; i++)
	{
		node->data.field[i] = bnode->data.field[i] + deltaItem->delta[i];
	}
}

void ForwHashDataApply(HashLinkNode *node, DeltaItem *deltaItem)
{
	HashLinkNode *nodeNext = node->next;
	while (nodeNext)
	{
		for (size_t i = 0; i < DATA_FIELD_NUM; i++)
		{
			nodeNext->data.field[i] += deltaItem->delta[i];
		}
		nodeNext = nodeNext->next;
	}
}

bool HashLinkInsert(HashItem *hashList, uint64_t index, DeltaItem *deltaItem, uint64_t version)
{
	bool result = false;
	//赋值为当前链表节点的前一个节点
	HashLinkNode* bnode = NULL;
	bnode = HashLinkFind(hashList[index].head,version);
	//若存在比当前节点小的最大节点，则在该节点后插入当前节点
	if (bnode)
	{
		if (bnode->data.version < version)
		{
			HashLinkNode *node = HashLinkNodeCreate(deltaItem->key,version);
			node->next = bnode->next;
			bnode->next = node; 
			CurrHashDataApply(node,bnode,deltaItem);
			ForwHashDataApply(node,deltaItem);
			result = true;
		}
		else if(bnode->data.version = version)
		{
			CurrHashDataApplyOnly(bnode,deltaItem);
			ForwHashDataApply(bnode,deltaItem);
			result = true;
		}
	}
	//若不存在比当前节点小的最大节点（即链表中无节点或者链表中节点都比当前节点大）
	//则将当前节点放在链表的最开头
	else
	{
		HashLinkNode *node = HashLinkNodeCreate(deltaItem->key,version);
		node->next = hashList[index].head;
		hashList[index].head = node;
		CurrHashDataApplyOnly(node,deltaItem);
		ForwHashDataApply(node,deltaItem);
		result = true;
	}
	return result;
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
}

bool HashDirectInsert(HashItem *hashList, DeltaItem *deltaItem, uint64_t index, uint64_t version)
{
	bool result = false;
	//key不存在的情况
	if (hashList[index].key == NULLKEY)
	{
		hashList[index].key = deltaItem->key;
		HashLinkNode *node = HashLinkNodeCreate(deltaItem->key,version);
		CurrHashDataApplyOnly(node, deltaItem);
		node->next = hashList[index].head;
		hashList[index].head = node;
		result = true;
		return result;
	}
	//key相等的情况
	else if (hashList[index].key == deltaItem->key)
	{
		result = HashLinkInsert(hashList, index, deltaItem, version);
	}
	return result;
}

bool HashInsert(HashItem *hashList, DeltaItem *deltaItem, uint64_t version)
{
	bool result = false;
	if (deltaItem->key == 0)
	{
		hashList[deltaItem->key].key = deltaItem->key;
		result = HashLinkInsert(hashList,0,deltaItem,version);
		return result;
	}
	uint64_t index =  HashGetPosition(deltaItem->key);
	result = HashDirectInsert(hashList, deltaItem, index,version);
	if(result == false)
	{
		for (size_t i = 1; i < HASH_LEN; i++)
		{
			uint64_t newIndex = (index + i) % HASH_LEN;
			if(newIndex == 0)
			{
				newIndex++;
			}
			result = HashDirectInsert(hashList, deltaItem, newIndex,version);
			if(result == true)
			{
				break;
			}
		}
	}
	return result;
}

bool HashSearchNode(HashItem *hashList,uint64_t index,uint64_t version, Data &data)
{
	bool result = false;
	HashLinkNode *node = HashLinkFind(hashList[index].head, version);
	if (node)
	{
		data.key = node->data.key;
		data.version = node->data.version;
		for (size_t i = 0; i < DATA_FIELD_NUM; i++)
		{
			data.field[i] = node->data.field[i];
		}
		result = true;
	}
	return result;
}

// bool HashSearchKey(HashItem *hashList, uint64_t index, uint64_t key, uint64_t version, Data &data)
// {
// 	bool result = false;
// 	if (hashList[index].key == NULLKEY)
// 	{
// 		printf("enter nullkey first, index = %ld, hashList[index].key = %ld\n",index,hashList[index].key);
// 		return result;
// 	}
// 	else if (hashList[index].key == key)
// 	{
// 		printf("enter equalkey first, index = %ld\n, hashList[index].key = %ld",index,hashList[index].key);
// 		result = HashSearchNode(hashList, index, version, data);
// 		return result;
// 	}
// 	return result;
// }

bool HashSearch(HashItem *hashList, uint64_t key, uint64_t version, Data &data)
{
	bool result = false;
	if (key == 0)
	{
		result = HashSearchNode(hashList, key, version, data);
		return result;
	}
	uint64_t index = HashGetPosition(key);
	if (hashList[index].key == NULLKEY)
	{
		return result;
	}
	else if (hashList[index].key == key)
	{
		result = HashSearchNode(hashList, index, version, data);
		return result;
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
			if (hashList[newIndex].key == NULLKEY)
			{
				return result;
			}
			else if (hashList[newIndex].key == key)
			{
				result = HashSearchNode(hashList, newIndex, version, data);
				return result;
			}
		}
	}
	return result;
}