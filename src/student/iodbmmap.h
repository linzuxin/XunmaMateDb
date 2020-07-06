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
};

MmapDb *initMmapDb(const char *dir);
void deinitMmapDb(MmapDb *);

bool writeMmapDb(Data *, const DeltaPacket &packet);
bool readMmapDb(Data *, uint64_t key, uint64_t version, Data &data);