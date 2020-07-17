#include "store_impl.h"
#include "replay.h"

bool StoreImpl::Init(const char *dir) {
  std::lock_guard<std::mutex> lock(mutex_);
  memset(&hashList, 0, sizeof(HashItem)*HASH_LEN);
  return true;
}

void StoreImpl::Deinit() {
  std::lock_guard<std::mutex> lock(mutex_);
}

bool StoreImpl::WriteDeltaPacket(const DeltaPacket &packet) {
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = false;
  for(uint16_t i=0;i<packet.delta_count;++i)
  {
    DeltaItem tmp = packet.deltas[i];
    DeltaItem *deltaItem = &tmp;
    result = HashInsert(hashList,deltaItem,packet.version);
  }
  return result;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data) {
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = false;
  result = HashSearch(hashList,key,version,data);
  return true;
}
