#pragma once

#include "common.h"
#include "redblackbst.h"

#define DATACOUNT 30000
#define DIRLEN 256

struct MmapDb
{
  uint64_t *index;
  Data *data;
  RedBlackBSTNode **rbBst;
  uint64_t length;
};

MmapDb *initMmapDb(const char *dir);
void deinitMmapDb(MmapDb *);

bool writeMmapDb(MmapDb *, const DeltaPacket &packet);
bool readMmapDb(MmapDb *, uint64_t key, uint64_t version, Data &data);