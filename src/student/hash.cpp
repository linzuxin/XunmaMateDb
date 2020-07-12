#include "hash.h"

//将Delta转换成Data
Data DeltaToData(DeltaItem item, uint64_t version)
{
	Data data;
	data.key = item.key;
	data.version = version;
	for (size_t i = 0; i < DATA_FIELD_NUM; i++)
	{
		data.field[i] += item.delta[i];
	}
	return data;
}

//获取链表节点插入位置
HashLinkNode* HashLinkFind(HashLinkNode* head, uint64_t version) 
{
	HashLinkNode* result = NULL;
	//循环查找，找到比当前版本小的最大版本所在节点
	while (head && head->data.version <= version)
	{
		result = head;
		head = head->next;
	}
	return result;
}

//插入链表节点
HashLinkNode* HashLinkInsert(HashLinkNode *head, HashLinkNode *node)
{
	//赋值为当前链表节点的前一个节点
	HashLinkNode* bnode;
	//赋值为当前链表节点的后一个节点
	HashLinkNode* fnode;
	//获取插入位置
	bnode = HashLinkFind(head, node->data.version);
	//若存在比当前节点小的最大节点，则在该节点后插入当前节点
	if (bnode && bnode->data.version < node->data.version)
	{
		node->next = bnode->next;
		bnode->next = node; 
		fnode = node->next;
	}
	//若不存在比当前节点小的最大节点（即链表中无节点或者链表中节点都比当前节点大）
	//则将当前节点放在链表的最开头
	else
	{
		node->next = head;
		head = node;
		fnode = head->next;
	}
	//给node后面的节点的数据累加当前版本的field的值
	while (fnode)
	{
		for (size_t i = 0; i < DATA_FIELD_NUM; i++)
		{
			fnode->data.field[i] += node->data.field[i];
		}
		fnode = fnode->next;
	}
	//将node中的Data的field累加前一版本Data的field的值
	if(bnode)
	{
		for (size_t i = 0; i < DATA_FIELD_NUM; i++)
		{
			node->data.field[i] += bnode->data.field[i];
		}
	}
	return node;
}

//获取hash索引位置
int HashGetPosition(int key)
{
	int result;
	result = key % HASH_PRIME_NUM;
	return result;
}

//hash中插入数据
int HashInsert(HashItem *hashList, Data &data)
{
	//声明一个链表节点并申请空间，用于存储Data。
	HashLinkNode *node;
	if(!(node = (HashLinkNode*)malloc(sizeof(HashLinkNode))))
	{
		printf("HashLinkNode malloc Error!\n");
		return 0;
	}
	//给链表节点赋值为Data
	node->data = data;
	node->next = NULL;
	//根据key获取Data的hash索引位置
	uint64_t index = HashGetPosition(data.key);
	uint64_t indextmp;
	//若该索引在hash中无元素,则直接插
	if (hashList[index].key == NULLKEY)
	{
		//在hash中标注key
		hashList[index].key = data.key;
		//在hash元素指向的链表中放入data
		HashLinkInsert(hashList[index].head, node);
		printf("Hash insert succeed!\n");
	}
	//若该索引在hash中的元素等于Data的key,则寻找对应链表并插入节点
	else if (hashList[index].key == data.key)
	{
		HashLinkInsert(hashList[index].head, node);
		printf("Hash insert succeed!\n");
	}
	//若该索引在hash中的元素不等于Data的key,则通过开放地址法在hash中循环获取索引。
	//当找到相等的key时，或者找到第一个空位时，插入Data。
	else
	{
		uint64_t i;
		for (uint64_t i = 1; i < HASH_LEN; i++)
		{
			//重新获取索引位置
			index = (index + i) % HASH_LEN;
			//为空时直接插入key
			if (hashList[index].key == NULLKEY)
			{
				//在hash中标注key
				hashList[index].key = data.key;
				//在hash元素指向的链表中放入data
				HashLinkInsert(hashList[index].head, node);
				printf("Hash insert succeed!\n");
				break;
			}
			else if (hashList[index].key == data.key)
			{
				HashLinkInsert(hashList[index].head, node);
				printf("Hash insert succeed!\n");
				break;
			}
			printf("no space for inserting");
			return 0;
		}
	}
	return 1;
}

//查找对应版本的Data
int HashSearch(HashItem *hashList, uint64_t key, uint64_t version, Data &data)
{
	uint64_t index = HashGetPosition(key);
	HashLinkNode *gnode;
	// Data data;
	if (hashList[index].key == NULLKEY)
	{
		printf("Hash search: Not Found!\n");
		return 0;
	}
	else if (hashList[index].key == key)
	{
		gnode = HashLinkFind(hashList[index].head,version);
		if (gnode->data.version == version)
		{
			data = gnode->data;
		}
	}
	else
	{
		for (uint64_t i = 1; i < HASH_LEN; ++i)
		{
			index = (index + i) % HASH_LEN;
			if (hashList[index].key == NULLKEY)
			{
				printf("Hash search: Not Found!\n");
				return 0;
			}
			else if (hashList[index].key == key)
			{
				gnode = HashLinkFind(hashList[index].head,version);
				if (gnode->data.version == version)
				{
					data = gnode->data;
				}
				break;
			}
		};
	}
	return 1;
}