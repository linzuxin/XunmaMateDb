#pragma once
#include "common.h"

#include <memory>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>


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
  std::unordered_map<uint64_t,std::vector<DeltaItem>> tree;
  uint64_t global_max_version = 0;
};