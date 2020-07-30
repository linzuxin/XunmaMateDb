#include "hash.h"
#include <string.h>

HashLinkNode* HashLinkNodeCreate(uint64_t version)
{
	//声明一个链表节点并申请空间，用于存储Data。
	HashLinkNode *node = NULL;
	if(!(node = (HashLinkNode*)malloc(sizeof(HashLinkNode))))
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

HashLinkNode* HashLinkFind(HashLinkNode* head, uint64_t version) 
{
	HashLinkNode* result = NULL;
	HashLinkNode* node = head;
	//循环查找，找到比当前版本小的最大版本所在节点
	while (node && node->version <= version)
	{
		result = node;
		node = node->next;
	}
	return result;
}

bool CurrHashDataApplyOnly(Dbio *dbio, HashLinkNode *node,DeltaItem *deltaItem)
{
	bool result = writeData(dbio,deltaItem,node->version);
	node->indexOffset = dbio->indexPosition-1;
	return result;
}

bool CurrHashDataApply(Dbio *dbio, HashLinkNode *node, HashLinkNode *bnode, DeltaItem *deltaItem)
{
	printf("enter CurrHashDataApply.\nkey:%ld, bnode version:%ld, node version:%ld\n",deltaItem->key,bnode->version,node->version);
	bool result = false;
	//get bnode's data's field
	Data *tmp = NULL;
	if(!(tmp = (Data*)malloc(sizeof(Data))))
	{
		printf("tmp Data malloc fail!\n");
		return result;
	}
	result = readData(dbio, bnode->indexOffset, tmp);
	for (size_t k = 0; k < 64; k++)
	{
		printf("read bnode info: key:%ld, version:%ld,field index:%ld, value:%ld\n",tmp->key, tmp->version,k,tmp->field[k]);
	}	
	if(result == false)
	{
		return result;
	}
	//bnode's field plus deltaItem's delta
	DeltaItem *tmpWrite = NULL;
	if(!(tmpWrite = (DeltaItem*)malloc(sizeof(DeltaItem))))
	{
		return result;
	}
	tmpWrite->key = deltaItem->key;
	for (size_t i = 0; i < DATA_FIELD_NUM; i++)
	{
		tmpWrite->delta[i] = deltaItem->delta[i] + tmp->field[i];
	}
	//write node's data
	result = writeData(dbio,tmpWrite,node->version);
	node->indexOffset = dbio->indexPosition-1;
	printf("insert node's offset:%ld\n",node->indexOffset);
	return result;
}

bool ForwHashDataApply(Dbio *dbio,HashLinkNode *node, DeltaItem *deltaItem)
{
	bool result = false;
	HashLinkNode *nodeNext = node->next;
	while (nodeNext)
	{
		//get next node's data's field
		Data *tmp = NULL;
		if(!(tmp = (Data*)malloc(sizeof(Data))))
		{
			printf("tmp Data malloc fail!\n");
			return result;
		}
		//get next data's field
		readData(dbio, nodeNext->indexOffset, tmp);
		//next data's field plus deltaItem's delta
		DeltaItem *tmpWrite = NULL;
		if(!(tmpWrite = (DeltaItem*)malloc(sizeof(DeltaItem))))
		{
			printf("tmpWrite DeltaItem malloc fail!\n");
			return result;
		}
		tmpWrite->key = deltaItem->key;
		for (size_t i = 0; i < DATA_FIELD_NUM; i++)
		{
			tmpWrite->delta[i] =  tmp->field[i] + deltaItem->delta[i];
		}
		//write next node's data
		result = writeData(dbio,tmpWrite,tmp->version);
		nodeNext->indexOffset = dbio->indexPosition-1;
		nodeNext = nodeNext->next;
	}
	return result;
}

bool HashLinkInsert(Dbio *dbio,HashItem *hashList, uint64_t index, DeltaItem *deltaItem, uint64_t version)
{
	printf("enter HashLinkInsert.\ninsert key:%ld,version:%ld,\n",deltaItem->key,version);
	bool result = false;
	//赋值为当前链表节点的前一个节点
	HashLinkNode* bnode = NULL;
	bnode = HashLinkFind(hashList[index].head,version);
	//若存在比当前节点小的最大节点，则在该节点后插入当前节点
	if (bnode)
	{
		if (bnode->version < version)
		{
			printf("bnode exisit. bnode version:%ld\n",bnode->version);
			HashLinkNode *node = HashLinkNodeCreate(version);
			printf("create node. node version:%ld\n",node->version);
			node->next = bnode->next;
			bnode->next = node; 
			CurrHashDataApply(dbio,node,bnode,deltaItem);
			ForwHashDataApply(dbio,node,deltaItem);
			result = true;
		}
		else if(bnode->version == version)
		{
			CurrHashDataApplyOnly(dbio,bnode,deltaItem);
			ForwHashDataApply(dbio,bnode,deltaItem);
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
		CurrHashDataApplyOnly(dbio,node,deltaItem);
		ForwHashDataApply(dbio,node,deltaItem);
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

bool HashDirectInsert(Dbio *dbio, HashItem *hashList, DeltaItem *deltaItem, uint64_t index, uint64_t version)
{
	printf("enter HashDirectInsert.\ninsert key:%ld,version:%ld,\n",deltaItem->key,version);
	bool result = false;
	//key不存在的情况
	if (hashList[index].key == NULLKEY)
	{
		hashList[index].key = deltaItem->key;
		HashLinkNode *node = HashLinkNodeCreate(version);
		CurrHashDataApplyOnly(dbio,node, deltaItem);
		node->next = hashList[index].head;
		hashList[index].head = node;
		result = true;
	}
	//key相等的情况
	else if (hashList[index].key == deltaItem->key)
	{
		printf("enter key equal\n");
		result = HashLinkInsert(dbio,hashList, index, deltaItem, version);
	}
	return result;
}

bool HashInsert(Dbio *dbio, HashItem *hashList, DeltaItem *deltaItem, uint64_t version)
{
	printf("enter HashInsert.\ninsert key:%ld,version:%ld,\n",deltaItem->key,version);
	bool result = false;
	if (deltaItem->key == 0)
	{
		hashList[deltaItem->key].key = deltaItem->key;
		result = HashLinkInsert(dbio,hashList,0,deltaItem,version);
		return result;
	}
	uint64_t index =  HashGetPosition(deltaItem->key);
	result = HashDirectInsert(dbio, hashList, deltaItem, index, version);
	if(result == false)
	{
		for (size_t i = 1; i < HASH_LEN; i++)
		{
			uint64_t newIndex = (index + i) % HASH_LEN;
			if(newIndex == 0)
			{
				newIndex++;
			}
			result = HashDirectInsert(dbio,hashList, deltaItem, newIndex,version);
			if(result == true)
			{
				break;
			}
		}
	}
	return result;
}

bool HashSearchNode(Dbio *dbio, HashItem *hashList,uint64_t index, uint64_t version, Data &data)
{
	bool result = false;
	HashLinkNode *node = HashLinkFind(hashList[index].head, version);
	if (node)
	{
		//get node's data's field
		Data *tmp = NULL;
		if(!(tmp = (Data*)malloc(sizeof(Data))))
		{
			printf("tmp Data malloc fail!\n");
			return result;
		}
		result = readData(dbio, node->indexOffset, tmp);
		data.key = tmp->key;
		data.version = tmp->version;
		for (size_t i = 0; i < DATA_FIELD_NUM; i++)
		{
			data.field[i] = tmp->field[i];
		}
	}
	return result;
}

bool HashSearch(Dbio *dbio,HashItem *hashList, uint64_t key, uint64_t version, Data &data)
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
				result = HashSearchNode(dbio, hashList, newIndex, version, data);
				return result;
			}
		}
	}
	return result;
}

bool HashLinkInit(Dbio *dbio,HashItem *hashList, uint64_t index, uint64_t offset)
{
	bool result = false;
	//赋值为当前链表节点的前一个节点
	HashLinkNode* bnode = NULL;
	bnode = HashLinkFind(hashList[index].head,dbio->indexList[offset].version);
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
		else if(bnode->version == dbio->indexList[offset].version)
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
		result = HashLinkInit(dbio,hashList, index, offset);
	}
	return result;
}

bool HashItemInit(Dbio *dbio, HashItem *hashList, uint64_t offset)
{
	bool result = false;
	if (dbio->indexList[offset].key == 0)
	{
		hashList[0].key = dbio->indexList[offset].key;
		result =HashLinkInit(dbio,hashList,0,offset);
		return result;
	}
	uint64_t index =  HashGetPosition(dbio->indexList[offset].key);
	result = HashDirectInit(dbio, hashList, index, offset);
	if(result == false)
	{
		for (size_t i = 1; i < HASH_LEN; i++)
		{
			uint64_t newIndex = (index + i) % HASH_LEN;
			if(newIndex == 0)
			{
				newIndex++;
			}
			result = HashDirectInit(dbio, hashList, index, offset);
			if(result == true)
			{
				break;
			}
		}
	}
	return result;
}

void hashInit(Dbio *dbio, HashItem *hashList)
{
	uint64_t offset;
	for (offset = 0; offset < dbio->indexPosition; offset++)
	{
		HashItemInit(dbio, hashList, offset);
	}
}


int main()
{
    Dbio *hashIo = initDbio("hashIo");
	HashItem hashlist[80000];
	memset(hashlist,0,sizeof(HashItem)*80000);
	printf("-----------test hash------------\n");
	uint64_t i,j,k;
    DeltaItem *d1 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d2 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d3 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d4 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d5 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d6 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d7 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d8 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d9 =  (DeltaItem*)malloc(sizeof(DeltaItem));
    DeltaItem *d10 =  (DeltaItem*)malloc(sizeof(DeltaItem));

    d1->key = 20; 
    uint64_t version1 = 20;
    d2->key = 20;
    uint64_t version2 = 5;
    d3->key = 20;
    uint64_t version3 = 15;
    d4->key = 20;
    uint64_t version4 = 25;
    d5->key = 80000;
    uint64_t version5 = 80;
    d6->key = 1;
    uint64_t version6 = 11;
    d7->key = 0;
    uint64_t version7 = 15;
    d8->key = 79999;
    uint64_t version8 = 99;
    d9->key = 159998;
    uint64_t version9 = 98;
    d10->key = 0;
    uint64_t version10 = 15;

    for (i = 0; i < 64; i++)
    {
        d1->delta[i] = 20;
        d2->delta[i] = 5;
        d3->delta[i] = 15;
        d4->delta[i] = 25;
        d5->delta[i] = 80;
        d6->delta[i] = 11;
        d7->delta[i] = 15;
        d8->delta[i] = 99;
        d9->delta[i] = 98;
        d10->delta[i] = 15;
    }
	printf("dbio's indexPosition: %ld, catchPosition: %ld, dataPosition: %ld\n",hashIo->indexPosition,hashIo->catchePosition, hashIo->dataPosition);
	printf("------------insert d1----------------\n");
	HashInsert(hashIo, hashlist,d1,version1);
	Data da1 = {};
	HashSearch(hashIo, hashlist,d1->key,version1,da1);
	for (size_t a = 0; a < 64; a++)
	{
		printf("DA1  key:%ld, version:%ld, index:%ld, field:%ld\n",da1.key,da1.version,a,da1.field[a]);
	}
	printf("dbio's indexPosition: %ld, catchPosition: %ld, dataPosition: %ld\n",hashIo->indexPosition,hashIo->catchePosition, hashIo->dataPosition);
	printf("------------insert d2----------------\n");
	HashInsert(hashIo, hashlist,d2,version2);
	Data da2 = {};
	HashSearch(hashIo, hashlist,d2->key,version2,da2);
	for (size_t b = 0; b < 64; b++)
	{
		printf("DA2  key:%ld, version:%ld, index:%ld, field:%ld\n",da2.key,da2.version,b,da2.field[b]);
	}
	printf("dbio's indexPosition: %ld, catchPosition: %ld, dataPosition: %ld\n",hashIo->indexPosition,hashIo->catchePosition, hashIo->dataPosition);
	printf("------------insert d3----------------\n");
	HashInsert(hashIo, hashlist,d3,version3);
	Data da3 = {};
	HashSearch(hashIo, hashlist,d3->key,version3,da3);
	for (size_t c = 0; c < 64; c++)
	{
		printf("DA3  key:%ld, version:%ld, index:%ld, field:%ld\n",da3.key,da3.version,c,da3.field[c]);
	}
	printf("dbio's indexPosition: %ld, catchPosition: %ld, dataPosition: %ld\n",hashIo->indexPosition,hashIo->catchePosition, hashIo->dataPosition);
	printf("------------insert d4----------------\n");
	bool d4Result = HashInsert(hashIo, hashlist,d4,version4);
	printf("d4Result:%d\n",d4Result);
	Data da4 = {};
	// HashSearch(hashIo, hashlist,d4->key,version4,da4);
	// for (size_t d = 0; d < 64; d++)
	// {
	// 	printf("DA4  key:%ld, version:%ld, index:%ld, field:%ld\n",da4.key,da4.version,d,da4.field[d]);
	// }	
	printf("dbio's indexPosition: %ld, catchPosition: %ld, dataPosition: %ld\n",hashIo->indexPosition,hashIo->catchePosition, hashIo->dataPosition);
	Data *rdata0 = (Data*)malloc(sizeof(Data));
	Data *rdata1 = (Data*)malloc(sizeof(Data));
	Data *rdata2 = (Data*)malloc(sizeof(Data));
	Data *rdata3 = (Data*)malloc(sizeof(Data));
	Data *rdata4 = (Data*)malloc(sizeof(Data));
	Data *rdata5 = (Data*)malloc(sizeof(Data));
	Data *rdata6 = (Data*)malloc(sizeof(Data));
	pread(hashIo->fdData,rdata0,sizeof(Data),0);
	pread(hashIo->fdData,rdata1,sizeof(Data),1);
	pread(hashIo->fdData,rdata2,sizeof(Data),2);
	pread(hashIo->fdData,rdata3,sizeof(Data),3);
	pread(hashIo->fdData,rdata4,sizeof(Data),4);
	pread(hashIo->fdData,rdata5,sizeof(Data),5);
	pread(hashIo->fdData,rdata6,sizeof(Data),6);


	// readData(hashIo,0,rdata0);
	// readData(hashIo,1,rdata1);
	// readData(hashIo,2,rdata2);
	// readData(hashIo,3,rdata3);
	// readData(hashIo,4,rdata4);
	// readData(hashIo,5,rdata5);
	// readData(hashIo,6,rdata6);
	for (size_t aa = 0; aa < 64; aa++)
	{
		printf("rdata0  key:%ld, version:%ld, index:%ld, field:%ld\n",rdata0->key,rdata0->version,aa,rdata0->field[aa]);
	}
	for (size_t bb = 0; bb < 64; bb++)
	{
		printf("rdata1  key:%ld, version:%ld, index:%ld, field:%ld\n",rdata1->key,rdata1->version,bb,rdata1->field[bb]);
	}
	for (size_t cc = 0; cc < 64; cc++)
	{
		printf("rdata2  key:%ld, version:%ld, index:%ld, field:%ld\n",rdata2->key,rdata2->version,cc,rdata2->field[cc]);
	}
	for (size_t dd = 0; dd < 64; dd++)
	{
		printf("rdata3  key:%ld, version:%ld, index:%ld, field:%ld\n",rdata3->key,rdata3->version,dd,rdata3->field[dd]);
	}
	for (size_t ee = 0; ee < 64; ee++)
	{
		printf("rdata4  key:%ld, version:%ld, index:%ld, field:%ld\n",rdata4->key,rdata4->version,ee,rdata4->field[ee]);
	}	
	for (size_t ff = 0; ff < 64; ff++)
	{
		printf("rdata5  key:%ld, version:%ld, index:%ld, field:%ld\n",rdata5->key,rdata5->version,ff,rdata5->field[ff]);
	}
	for (size_t gg = 0; gg < 64; gg++)
	{
		printf("rdata6  key:%ld, version:%ld, index:%ld, field:%ld\n",rdata6->key,rdata6->version,gg,rdata6->field[gg]);
	}
}