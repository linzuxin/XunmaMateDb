#include "hash.h"

HashLinkNode *HashLinkNodeCreate(uint64_t version)
{
	//声明一个链表节点并申请空间，用于存储Data。
	HashLinkNode *node = NULL;
	if (!(node = (HashLinkNode *)malloc(sizeof(HashLinkNode))))
	{
		return NULL;
	}
	// memset(node,0,sizeof(HashLinkNode));
	//给链表节点赋值
	node->version = version;
	node->indexOffset = 0;
	node->next = NULL;
	return node;
}

HashLinkNode *HashLinkFind(HashLinkNode *head, uint64_t version)
{
	HashLinkNode *result = NULL;
	HashLinkNode *node = head;
	//循环查找，找到比当前版本小的最大版本所在节点
	while (node && node->version <= version)
	{
		result = node;
		node = node->next;
	}
	return result;
}

bool CurrHashDataApplyOnly(Dbio *dbio, HashLinkNode *node, DeltaItem *deltaItem)
{
	bool result = writeData(dbio, deltaItem, node->version);
	node->indexOffset = dbio->indexPosition - 1;
	return result;
}

bool CurrHashDataApply(Dbio *dbio, HashLinkNode *node, HashLinkNode *bnode, DeltaItem *deltaItem)
{
	bool result = false;
	//get bnode's data's field
	Data *tmp = NULL;
	if (!(tmp = (Data *)malloc(sizeof(Data))))
	{
		printf("tmp Data malloc fail!\n");
		return result;
	}
	result = readData(dbio, bnode->indexOffset, tmp);
	if (result == false)
	{
		return result;
	}
	//bnode's field plus deltaItem's delta
	DeltaItem *tmpWrite = NULL;
	if (!(tmpWrite = (DeltaItem *)malloc(sizeof(DeltaItem))))
	{
		printf("tmpWrite DeltaItem malloc fail!\n");
		return result;
	}
	tmpWrite->key = deltaItem->key;
	for (size_t i = 0; i < DATA_FIELD_NUM; i++)
	{
		tmpWrite->delta[i] = deltaItem->delta[i] + tmp->field[i];
	}
	//write node's data
	writeData(dbio, tmpWrite, node->version);
	node->indexOffset = dbio->indexPosition - 1;
	return result;
}

bool ForwHashDataApply(Dbio *dbio, HashLinkNode *node, DeltaItem *deltaItem)
{
	bool result = false;
	HashLinkNode *nodeNext = node->next;
	while (nodeNext)
	{
		//get next node's data's field
		Data *tmp = NULL;
		if (!(tmp = (Data *)malloc(sizeof(Data))))
		{
			printf("tmp Data malloc fail!\n");
			return result;
		}
		//get next data's field
		readData(dbio, nodeNext->indexOffset, tmp);
		//next data's field plus deltaItem's delta
		DeltaItem *tmpWrite = NULL;
		if (!(tmpWrite = (DeltaItem *)malloc(sizeof(DeltaItem))))
		{
			printf("tmpWrite DeltaItem malloc fail!\n");
			return result;
		}
		tmpWrite->key = deltaItem->key;
		for (size_t i = 0; i < DATA_FIELD_NUM; i++)
		{
			tmpWrite->delta[i] = tmp->field[i] + deltaItem->delta[i];
		}
		//write next node's data
		writeData(dbio, tmpWrite, tmp->version);
		nodeNext->indexOffset = dbio->indexPosition - 1;
		nodeNext = nodeNext->next;
		result = true;
	}
	return result;
}

bool HashLinkInsert(Dbio *dbio, HashItem *hashList, uint64_t index, DeltaItem *deltaItem, uint64_t version)
{
	bool result = false;
	//赋值为当前链表节点的前一个节点
	HashLinkNode *bnode = NULL;
	bnode = HashLinkFind(hashList[index].head, version);
	//若存在比当前节点小的最大节点，则在该节点后插入当前节点
	if (bnode)
	{
		if (bnode->version < version)
		{
			HashLinkNode *node = HashLinkNodeCreate(version);
			node->next = bnode->next;
			bnode->next = node;
			CurrHashDataApply(dbio, node, bnode, deltaItem);
			ForwHashDataApply(dbio, node, deltaItem);
			result = true;
		}
		else if (bnode->version == version)
		{
			CurrHashDataApplyOnly(dbio, bnode, deltaItem);
			ForwHashDataApply(dbio, bnode, deltaItem);
			result = true;
		}
	}
	//若不存在比当前节点小的最大节点（即链表中无节点或者链表中节点都比当前节点大）
	//则将当前节点放在链表的最开头
	else
	{
		HashLinkNode *node = HashLinkNodeCreate(version);
		node->next = hashList[index].head;
		hashList[index].head = node;
		CurrHashDataApplyOnly(dbio, node, deltaItem);
		ForwHashDataApply(dbio, node, deltaItem);
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
		index = HASH_LEN - 1;
	}

	return index;
}

bool HashDirectInsert(Dbio *dbio, HashItem *hashList, DeltaItem *deltaItem, uint64_t index, uint64_t version)
{
	bool result = false;
	//key不存在的情况
	if (hashList[index].key == NULLKEY)
	{
		hashList[index].key = deltaItem->key;
		HashLinkNode *node = HashLinkNodeCreate(version);
		CurrHashDataApplyOnly(dbio, node, deltaItem);
		node->next = hashList[index].head;
		hashList[index].head = node;
		result = true;
		return result;
	}
	//key相等的情况
	else if (hashList[index].key == deltaItem->key)
	{
		result = HashLinkInsert(dbio, hashList, index, deltaItem, version);
	}
	return result;
}

bool HashInsert(Dbio *dbio, HashItem *hashList, DeltaItem *deltaItem, uint64_t version)
{
	bool result = false;
	if (deltaItem->key == 0)
	{
		hashList[deltaItem->key].key = deltaItem->key;
		result = HashLinkInsert(dbio, hashList, 0, deltaItem, version);
		return result;
	}
	uint64_t index = HashGetPosition(deltaItem->key);
	result = HashDirectInsert(dbio, hashList, deltaItem, index, version);
	if (result == false)
	{
		for (size_t i = 1; i < HASH_LEN; i++)
		{
			uint64_t newIndex = (index + i) % HASH_LEN;
			if (newIndex == 0)
			{
				newIndex++;
			}
			result = HashDirectInsert(dbio, hashList, deltaItem, newIndex, version);
			if (result == true)
			{
				break;
			}
		}
	}
	return result;
}

bool HashSearchNode(Dbio *dbio, HashItem *hashList, uint64_t index, uint64_t version, Data &data)
{
	bool result = false;
	HashLinkNode *node = HashLinkFind(hashList[index].head, version);
	if (node)
	{
		//get node's data's field
		Data *tmp = NULL;
		if (!(tmp = (Data *)malloc(sizeof(Data))))
		{
			printf("tmp Data malloc fail!\n");
			return result;
		}
		readData(dbio, node->indexOffset, tmp);
		data.key = tmp->key;
		data.version = tmp->version;
		for (size_t i = 0; i < DATA_FIELD_NUM; i++)
		{
			data.field[i] = tmp->field[i];
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

bool HashSearch(Dbio *dbio, HashItem *hashList, uint64_t key, uint64_t version, Data &data)
{
	bool result = false;
	if (key == 0)
	{
		result = HashSearchNode(dbio, hashList, 0, version, data);
		return result;
	}
	uint64_t index = HashGetPosition(key);
	if (hashList[index].key == NULLKEY)
	{
		return result;
	}
	else if (hashList[index].key == key)
	{
		result = HashSearchNode(dbio, hashList, index, version, data);
		return result;
	}
	else
	{
		for (size_t i = 1; i < HASH_LEN; i++)
		{
			uint64_t newIndex = (index + i) % HASH_LEN;
			if (newIndex == 0)
			{
				newIndex++;
			}
			if (hashList[newIndex].key == NULLKEY)
			{
				return result;
			}
			else if (hashList[newIndex].key == key)
			{
				result = HashSearchNode(dbio, hashList, newIndex, version, data);
				return result;
			}
		}
	}
	return result;
}

bool HashLinkInit(Dbio *dbio, HashItem *hashList, uint64_t index, uint64_t offset)
{
	bool result = false;
	//赋值为当前链表节点的前一个节点
	HashLinkNode *bnode = NULL;
	bnode = HashLinkFind(hashList[index].head, dbio->indexList[offset].version);
	//若存在比当前节点小的最大节点，则在该节点后插入当前节点
	if (bnode)
	{
		if (bnode->version < dbio->indexList[offset].version)
		{
			HashLinkNode *node = HashLinkNodeCreate(dbio->indexList[offset].version);
			node->next = bnode->next;
			bnode->next = node;
			node->indexOffset = offset;
			result = true;
		}
		else if (bnode->version == dbio->indexList[offset].version)
		{
			bnode->indexOffset = offset;
			result = true;
		}
	}
	//若不存在比当前节点小的最大节点（即链表中无节点或者链表中节点都比当前节点大）
	//则将当前节点放在链表的最开头
	else
	{
		HashLinkNode *node = HashLinkNodeCreate(dbio->indexList[offset].version);
		node->next = hashList[index].head;
		hashList[index].head = node;
		node->indexOffset = offset;
		result = true;
	}
	return result;
}

bool HashDirectInit(Dbio *dbio, HashItem *hashList, uint64_t index, uint64_t offset)
{
	bool result = false;
	//key不存在的情况
	if (hashList[index].key == NULLKEY)
	{
		hashList[index].key = dbio->indexList[offset].key;
		HashLinkNode *node = HashLinkNodeCreate(dbio->indexList[offset].version);
		node->next = hashList[index].head;
		hashList[index].head = node;
		node->indexOffset = offset;
		result = true;
		return result;
	}
	//key相等的情况
	else if (hashList[index].key == dbio->indexList[offset].key)
	{
		result = HashLinkInit(dbio, hashList, index, offset);
	}
	return result;
}

bool HashItemInit(Dbio *dbio, HashItem *hashList, uint64_t offset)
{
	bool result = false;
	if (dbio->indexList[offset].key == 0)
	{
		hashList[0].key = dbio->indexList[offset].key;
		result = HashLinkInit(dbio, hashList, 0, offset);
		return result;
	}
	uint64_t index = HashGetPosition(dbio->indexList[offset].key);
	result = HashDirectInit(dbio, hashList, index, offset);
	if (result == false)
	{
		for (size_t i = 1; i < HASH_LEN; i++)
		{
			uint64_t newIndex = (index + i) % HASH_LEN;
			if (newIndex == 0)
			{
				newIndex++;
			}
			result = HashDirectInit(dbio, hashList, index, offset);
			if (result == true)
			{
				break;
			}
		}
	}
	return result;
}

bool hashInit(Dbio *dbio, HashItem *hashList)
{
	uint64_t offset;
	for (offset = 0; offset < dbio->indexPosition; offset++)
	{
		HashItemInit(dbio, hashList, offset);
	}
	return true;
}
