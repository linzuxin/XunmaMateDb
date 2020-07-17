#include "store_impl.h"
#include "replay.h"

bool StoreImpl::Init(const char *dir)
{
  std::lock_guard<std::mutex> lock(mutex_);
  profile = initProfile();
  memset(&hashList, 0, sizeof(HashTable)*HASH_LEN);
  return true;
}

void StoreImpl::Deinit()
{
  std::lock_guard<std::mutex> lock(mutex_);
}

bool StoreImpl::WriteDeltaPacket(const DeltaPacket &packet)
{
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = false;
  setDataVersion(profile, packet.version);
  for (uint16_t i = 0; i < packet.delta_count; ++i)
  {
    result = HashInsert(hashList, &packet.deltas[i], packet.version);
  }
  return result;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data)
{
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = false;
  if (validateDataVersion(profile,version)){
    result = HashSearch(hashList, key, version, data);
  }
  return result;
}
