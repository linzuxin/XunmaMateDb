#pragma once

//#include "buffer.h"
#include "store_intf.h"

class MetaTester {
public:
  MetaTester(){};

  virtual ~MetaTester(){};

  bool Init(const char *so_path, const char *data_path);

  void Deinit();

  double Run(int &err);

protected:
  void *loadSo(const char *path, const char *entry);

  int write(std::shared_ptr<StoreIntf> stor);

  int read(std::shared_ptr<StoreIntf> stor);

  int runJobWrite(int thread_num);
  int runJobRead(int thread_num);

private:
  void *so_ = nullptr;

  std::shared_ptr<StoreIntf> (*entry_)() = nullptr;

  const char *entry_name_ = "GetStoreIntf";
};
