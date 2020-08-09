#include "store_impl.h"
#include "easylogging++.h"

bool StoreImpl::Init(const char *dir)
{
  std::lock_guard<std::mutex> lock(mutex_);
  //Init data_mgr
  //Init meta_mgr

  profile = initProfile();
  dbio = initDbio(dir);
  memset(&hashList, 0, sizeof(HashTable) * HASH_LEN);
  return true;
}

void StoreImpl::Deinit()
{
  std::lock_guard<std::mutex> lock(mutex_);
  //Deinit data_mgr
  //Deinit meta_mgr
}

bool StoreImpl::WriteDeltaPacket(const DeltaPacket &packet)
{
  std::lock_guard<std::mutex> lock(mutex_);
  // 1. save data to disk
  // 2. save version to meta manager
  bool result = false;
  setDataVersion(profile, packet.version);
  for (uint16_t i = 0; i < packet.delta_count; ++i)
  {
    writeData(dbio, &packet.deltas[i], packet.version);
    result = HashInsert(hashList, &packet.deltas[i], packet.version);
  }
  return result;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data)
{
  std::lock_guard<std::mutex> lock(mutex_);
  // 1. get deltas with versions are less or equal than 'version' from delta
  // manager

  // 2. read deltas from delta file

  // 3. apply all deltas
  bool result = false;
  if (validateDataVersion(profile, version))
  {
    result = HashSearch(hashList, key, version, data);
  }
  return result;
}
