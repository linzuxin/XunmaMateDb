#pragma once

#include "common.h"

#define DATACOUNT 30000
#define DIRLEN 256

Data *initNmapDb(const char *dir);
void deinitMmapDb(Data *);
bool writeMmapDb(Data *,const DeltaPacket &packet);
bool readMmapDb(Data *,uint64_t key, uint64_t version, Data &data);