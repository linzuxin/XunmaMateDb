#include "store_impl.h"
#include "replay.h"

bool StoreImpl::Init(const char *dir) {
  std::lock_guard<std::mutex> lock(mutex_);
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
    Data data = DeltaToData(tmp,packet.version);
    result = HashInsert(hashList,data);
  }
  return result;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data) {
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = false;
  result = HashSearch(hashList,key,version,data);
  return true;
}
