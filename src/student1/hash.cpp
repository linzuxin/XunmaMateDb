#include "hash.h"
//获取链表节点插入位置
HashLinkNode *HashLinkFind(HashLinkNode *head, uint64_t version)
{
	HashLinkNode *result = NULL;
	HashLinkNode *node = head;
	//循环查找，找到比当前版本小的最大版本所在节点
	while (node && node->data.version <= version)
	{
		result = node;
		node = node->next;
	}
	return result;
}
HashLinkNode *CreateHashLinkNode(const DeltaItem *deltaItem, uint64_t version)
{
	//声明一个链表节点并申请空间，用于存储Data。
	HashLinkNode *node = NULL;
	if (!(node = (HashLinkNode *)malloc(sizeof(HashLinkNode))))
	{
		return NULL;
	}
	memset(node, 0, sizeof(HashLinkNode));
	node->data.key = deltaItem->key;
	node->data.version = version;

	//给链表节点赋值为Data
	for (size_t i = 0; i < DATA_FIELD_NUM; i++)
	{
		node->data.field[i] += deltaItem->delta[i];
	}
	node->next = NULL;
	return node;
}
void AppendHashLinkNode(HashLinkNode *node, const DeltaItem *deltaItem, uint64_t version)
{
	for (size_t i = 0; i < DATA_FIELD_NUM; i++)
	{
		node->data.field[i] += deltaItem->delta[i];
	}
}
void AppendHashPrevLinkNode(HashLinkNode *node,HashLinkNode *prevNode)
{
	for (size_t i = 0; i < DATA_FIELD_NUM; i++)
	{
		node->data.field[i] += prevNode->data.field[i];
	}
}
void AppendNextHashLinkNode(HashLinkNode *node, const DeltaItem *deltaItem, uint64_t version)
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
//插入链表节点
HashLinkNode *HashLinkInsert(HashTable *hashList, uint64_t index, const DeltaItem *deltaItem, uint64_t version)
{
	//赋值为当前链表节点的前一个节点
	HashLinkNode *bnode = NULL;
	HashLinkNode *enode = NULL;
	HashLinkNode *head = hashList[index].head;
	if (head)
	{
		//获取插入位置
		bnode = HashLinkFind(head, version);

		//若存在比当前节点小的最大节点，则在该节点后插入当前节点
		if (bnode)
		{
			if (bnode->data.version == version)
			{
				AppendHashLinkNode(bnode, deltaItem, version);
				enode = bnode;
			}
			else
			{
				HashLinkNode *node = CreateHashLinkNode(deltaItem, version);
				AppendHashPrevLinkNode(node,bnode);
				node->next = bnode->next;
				bnode->next = node;
				enode = node;
			}
		}
		else
		{
			//若不存在比当前节点小的最大节点（即链表中无节点或者链表中节点都比当前节点大）
			//则将当前节点放在链表的最开头
			HashLinkNode *node = CreateHashLinkNode(deltaItem, version);
			node->next = head;
			head = node;
			enode = node;
		}
	}
	else
	{
		head = CreateHashLinkNode(deltaItem, version);
	}
	if (enode)
	{
		AppendNextHashLinkNode(enode, deltaItem, version);
	}
	hashList[index].head = head;
	return bnode;
}

//获取hash索引位置
uint64_t HashGetPosition(uint64_t key)
{
	uint64_t result;
	result = key % HASH_PRIME_NUM;
	if (result == 0)
	{
		result = 1;
	}
	return result;
}

bool HashInsertIndex(HashTable *hashList, uint64_t index, const DeltaItem *deltaItem, uint64_t version)
{
	bool result = false;
	//若该索引在hash中无元素,则直接插
	if (hashList[index].key == NULLKEY)
	{
		//在hash中标注key
		hashList[index].key = deltaItem->key;
		HashLinkInsert(hashList, index, deltaItem, version);
		result = true;
	}
	//若该索引在hash中的元素等于Data的key,则寻找对应链表并插入节点
	else if (hashList[index].key == deltaItem->key)
	{
		HashLinkInsert(hashList, index, deltaItem, version);
		result = true;
	}
	return result;
}
bool HashInsertIndexAfter(HashTable *hashList, uint64_t index, const DeltaItem *deltaItem, uint64_t version)
{
	bool result = false;
	for (uint64_t i = 1; i < HASH_LEN - index; i++)
	{
		//重新获取索引位置
		index = index + i;
		if (HashInsertIndex(hashList, index, deltaItem, version))
		{
			result = true;
			break;
		}
	}
	return result;
}
//hash中插入数据
bool HashInsert(HashTable *hashList, const DeltaItem *deltaItem, uint64_t version)
{
	bool result = false;
	if (deltaItem->key == 0)
	{
		result = HashLinkInsert(hashList, 0, deltaItem, version);
	}
	else
	{
		//根据key获取Data的hash索引位置
		uint64_t index = HashGetPosition(deltaItem->key);
		result = HashInsertIndex(hashList, index, deltaItem, version);
		if (!result)
		{
			result = HashInsertIndexAfter(hashList, index + 1, deltaItem, version);
		}
		if (!result)
		{
			result = HashInsertIndexAfter(hashList, 0, deltaItem, version);
		}
	}
	return result;
}

bool HashSearchIndex(HashTable *hashList, uint64_t index, uint64_t key, uint64_t version, Data &data)
{
	bool result = false;
	// Data data;
	if (hashList[index].key == key)
	{
		//找到，赋值返回
		HashLinkNode *bnode = HashLinkFind(hashList[index].head, version);
		if (bnode)
		{
			data.key = bnode->data.key;
			data.version = bnode->data.version;
			for (size_t i = 0; i < DATA_FIELD_NUM; i++)
			{
				data.field[i] = bnode->data.field[i];
			}
			result = true;
		}
	}
	return result;
}
bool HashSearchIndexAfter(HashTable *hashList, uint64_t index, uint64_t key, uint64_t version, Data &data)
{
	bool result = false;
	for (uint64_t i = 1; i < HASH_LEN - index; i++)
	{
		//重新获取索引位置
		index = index + i;
		if (HashSearchIndex(hashList, index, key, version, data))
		{
			result = true;
			break;
		}
	}
	return result;
}
//查找对应版本的Data
bool HashSearch(HashTable *hashList, uint64_t key, uint64_t version, Data &data)
{
	bool result = false;
	if (key == 0)
	{
		result = HashSearchIndex(hashList, 0, key, version, data);
	}
	else
	{
		uint64_t index = HashGetPosition(key);
		if (hashList[index].key == NULLKEY)
		{
			//空，直接返回
		}
		else
		{
			result = HashSearchIndex(hashList, index, key, version, data);
			if (!result)
			{
				result = HashSearchIndexAfter(hashList, index + 1, key, version, data);
			}
			if (!result)
			{
				result = HashSearchIndexAfter(hashList, 0, key, version, data);
			}
		}
	}

	return result;
}