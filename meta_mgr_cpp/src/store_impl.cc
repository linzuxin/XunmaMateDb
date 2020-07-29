#include "store_impl.h"
#include "easylogging++.h"
#include "replay.h"

bool StoreImpl::Init(const char *dir) {
  std::lock_guard<std::mutex> lock(mutex_);
  //Init data_mgr
  //Init meta_mgr
  return true;
}

void StoreImpl::Deinit() {
  std::lock_guard<std::mutex> lock(mutex_);
  //Deinit data_mgr
  //Deinit meta_mgr
}

bool StoreImpl::WriteDeltaPacket(const DeltaPacket &packet) {
  std::lock_guard<std::mutex> lock(mutex_);
  // 1. save data to disk
  // 2. save version to meta manager
  return true;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data) {
  std::lock_guard<std::mutex> lock(mutex_);
  // 1. get deltas with versions are less or equal than 'version' from delta
  // manager

  // 2. read deltas from delta file

  // 3. apply all deltas
  memset(&data, 0, sizeof(Data));
  return true;
}
