#pragma once

#include "common.h"
#include "redblackbst.h"

#define DATACOUNT 30000
#define DIRLEN 256
union DataInfo {
  struct
  {
    uint8_t lenght;
    uint8_t version;
    uint8_t index;
    uint8_t count;
    uint32_t dataIndex;
  } infoItem;
  uint64_t info;
};

struct MmapDb
{
  uint64_t *indexIoDb;
  Data *dataIoDb;
  Data *dataInstance;
  RedBlackBSTNode **rbBst;
  uint32_t length;
};

MmapDb *initMmapDb(const char *dir);
void deinitMmapDb(MmapDb *);

bool writeMmapDb(MmapDb *, const DeltaPacket &packet);
bool readMmapDb(MmapDb *, uint64_t key, uint64_t version, Data &data);