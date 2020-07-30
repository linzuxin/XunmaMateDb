#include "meta_tester.h"

#include "easylogging++.h"
#include <atomic>
#include <chrono>
#include <dlfcn.h>
#include <future>
#include <thread>
#include <vector>

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::system_clock;
static std::atomic<uint64_t> global_version{0};

bool MetaTester::Init(const char *so_path, const char *data_path) {
  auto entry = loadSo(so_path, entry_name_);
  if (entry == nullptr) {
    return false;
  }

  entry_ = reinterpret_cast<std::shared_ptr<StoreIntf> (*)()>(entry);
  if(entry_ != nullptr) {
    entry_()->Init(data_path);
  }
  return entry_ != nullptr;
}

void MetaTester::Deinit() {
  entry_()->Deinit();
  if (so_ != NULL) {
    dlclose(so_);
    so_ = NULL;
  }
}

void *MetaTester::loadSo(const char *so_path, const char *entry) {
  void *dl = dlopen(so_path, RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL);
  if (!dl) {
    LOG(ERROR) << "load " << so_path << " failed: " << dlerror();
    return nullptr;
  }

  so_ = dl;
  auto f_entry = dlsym(dl, entry);
  if (NULL == f_entry) {
    LOG(ERROR) << "find symbol " << entry << " failed: " << dlerror();
    return nullptr;
  }

  return f_entry;
}

int MetaTester::runJobWrite(int thread_index) {
  if (entry_ == nullptr) {
    LOG(ERROR) << "Entry function in user so is nullptr, call Init() first";
    return 1;
  }

  auto stor = entry_();
  if (stor == nullptr) {
    LOG(ERROR) << "Get shared_ptr failed, check \"GetKVIntf\" in user "
                  "SO first";
    return 1;
  }

  int write_err = write(stor);
  return write_err;
}

int MetaTester::runJobRead(int thread_index) {
  if (entry_ == nullptr) {
    LOG(ERROR) << "Entry function in user so is nullptr, call Init() first";
    return 1;
  }

  auto stor = entry_();
  if (stor == nullptr) {
    LOG(ERROR) << "Get store interface shared_ptr failed, check "
                  "\"GetStoreIntf\" in user SO first";
    return 1;
  }
  int read_err = read(stor);
  return read_err;
}

double MetaTester::Run(int &err) {
  if (entry_ == nullptr) {
    LOG(ERROR) << "Entry function in user so is nullptr, call Init() first";
    return -1.0;
  }

  int thread_num = 2;
  std::vector<std::thread> thds;
  std::vector<std::future<int>> rets;
  thds.resize(thread_num);
  rets.resize(thread_num);
  auto begin = system_clock::now();
  // for write
  thread_num = 1;
  for (int i = 0; i < thread_num; i++) {
    std::packaged_task<int()> task(std::bind(&MetaTester::runJobWrite, this, i));
    rets[i] = task.get_future();
    thds[i] = std::thread(std::move(task));
  }
  for (int i = 0; i < thread_num; i++) {
    thds[i].join();
  }

  for (int i = 0; i < thread_num; i++) {
    err += rets[i].get();
  }

  // for read

  thread_num = 2;
  for (int i = 0; i < thread_num; i++) {
    std::packaged_task<int()> task(std::bind(
        &MetaTester::runJobRead, this, i));
    rets[i] = task.get_future();
    thds[i] = std::thread(std::move(task));
  }
  for (int i = 0; i < thread_num; i++) {
    thds[i].join();
  }

  for (int i = 0; i < thread_num; i++) {
    err += rets[i].get();
  }

  return duration_cast<duration<double>>(system_clock::now() - begin).count();
}

int MetaTester::write(std::shared_ptr<StoreIntf> stor) {
  if (stor == nullptr) {
    return 1;
  }
  /////////////////////////////////////////////////////////////////////////////
  int err = 0;
  DeltaPacket *packet =
      (DeltaPacket *)malloc(sizeof(DeltaPacket) + 10 * sizeof(DeltaItem));

  packet->version = ++global_version;
  packet->delta_count = 10;
  for (int j = 0; j < packet->delta_count; j++) {
    packet->deltas[j].key = 10086;
    packet->deltas[j].delta[0] = j;
    packet->deltas[j].delta[3] = 1;
    packet->deltas[j].delta[7] = j+2;
    packet->deltas[j].delta[10] = 2*j;
  }
  stor->WriteDeltaPacket(*packet);

  free(packet);
  /////////////////////////////////////////////////////////////////////////////
  return err;
}

int MetaTester::read(std::shared_ptr<StoreIntf> stor) {
  int err = 0;
  for (uint64_t i = 0; i <= global_version; i++) {
    Data data;
    if (stor->ReadDataByVersion(10086, i, data)) {
      err = 0;
      // LOG(INFO) << "get data by key= " << 10086
      //           << " final result: key=" << data.key
      //           << ", version=" << data.version
      //           << ", field[0] = " << data.field[0]
      //           << ", field[3] = " << data.field[3]
      //           << ", field[7] = " << data.field[7]
      //           << ", field[10] = " << data.field[10];
    } else {
      err = -1;
      // LOG(ERROR) << "get data by key=" << 10086 << ", version=" << i
      //            << " failed";
    }
  }
  return err;
}
