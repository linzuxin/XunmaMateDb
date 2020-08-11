#pragma once
#include <memory>
#include <mutex>
#include "common.h"
#include "hash.h"


class StoreImpl : public StoreIntf,
                  public std::enable_shared_from_this<StoreImpl> {
public:
  bool Init(const char *dir);

  void Deinit();

  bool WriteDeltaPacket(const DeltaPacket &packet);

  bool ReadDataByVersion(uint64_t key, uint64_t version, Data &data);

  std::shared_ptr<StoreImpl> GetPtr() { return shared_from_this(); }

private:
  std::mutex mutex_;
  HashNode_64* hashList;
  Dbio *dbio;

};