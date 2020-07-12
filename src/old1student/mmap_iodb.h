#pragma once

#include "common.h"
#include "key_map.h"

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

struct MmapIodb
{
  uint64_t *indexIoDb;
  Data *dataIoDb;
  Data *dataInstance;
  KeyMap **rbBst;
  uint32_t length;
};

MmapIodb *initMmapIodb(const char *dir);
void deinitMmapIodb(MmapIodb *);

bool writeMmapIodb(MmapIodb *, const DeltaPacket &packet);
bool readMmapIodb(MmapIodb *, uint64_t key, uint64_t version, Data &data);