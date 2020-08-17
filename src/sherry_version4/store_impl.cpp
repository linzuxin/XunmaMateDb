#include "store_impl.h"
#include "replay.h"

bool StoreImpl::Init(const char *dir) 
{
  std::lock_guard<std::mutex> lock(mutex_);
  dbio = initDbio("dbio");
  hashList = hashInit();
  uint64_t indexCount = hashRecover(dbio,hashList);
  recoverDbioIndex(dbio,indexCount);
  return true;
}

void StoreImpl::Deinit() {
  std::lock_guard<std::mutex> lock(mutex_);
}

bool StoreImpl::WriteDeltaPacket(const DeltaPacket &packet) {
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = false;
  result = writeData(dbio,hashList,packet);
  return result;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data) {
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = false;
  result = readData(dbio,hashList,key,version,data);
  return true;
}
