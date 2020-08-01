#include "meta_tester.h"

#include "easylogging++.h"
#include <atomic>
#include <chrono>
#include <dlfcn.h>
#include <future>
#include <thread>
#include <vector>
#include <algorithm>
#include <vector>
#include <unordered_map>

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::system_clock;
static std::atomic<uint64_t> global_version{0};
static std::unordered_map<uint64_t, std::vector<DeltaItem>> tree;
uint16_t deltaCount = 10;

bool comdata(Data &data, Data &data1)
{
  bool result = true;
  if (data.key != data1.key)
  {
    result = false;
  }
  if (data.version != data1.version)
  {
    result = false;
  }
  uint8_t i = 1;
  //for (uint8_t i = 0; i < DATA_FIELD_NUM; i++)
  {
    if (data.field[i] != data1.field[i])
    {
      result = false;
      printf("comdata error field:%lu,%lu,%d\n", data.field[i], data1.field[i], i);
    }
  }
  if (!result)
  {
    printf("comdata error key:%lu,%lu version:%lu,%lu\n", data.key, data1.key, data.version, data1.version);
  }
  return result;
}

bool MetaTester::Init(const char *so_path, const char *data_path)
{
  auto entry = loadSo(so_path, entry_name_);
  if (entry == nullptr)
  {
    return false;
  }

  entry_ = reinterpret_cast<std::shared_ptr<StoreIntf> (*)()>(entry);
  if (entry_ != nullptr)
  {
    entry_()->Init(data_path);
  }
  return entry_ != nullptr;
}

void MetaTester::Deinit()
{
  entry_()->Deinit();
  if (so_ != NULL)
  {
    dlclose(so_);
    so_ = NULL;
  }
}

void *MetaTester::loadSo(const char *so_path, const char *entry)
{
  void *dl = dlopen(so_path, RTLD_NOW | RTLD_DEEPBIND | RTLD_LOCAL);
  if (!dl)
  {
    LOG(ERROR) << "load " << so_path << " failed: " << dlerror();
    return nullptr;
  }

  so_ = dl;
  auto f_entry = dlsym(dl, entry);
  if (NULL == f_entry)
  {
    LOG(ERROR) << "find symbol " << entry << " failed: " << dlerror();
    return nullptr;
  }

  return f_entry;
}

int MetaTester::runJobWrite(int thread_index)
{
  if (entry_ == nullptr)
  {
    LOG(ERROR) << "Entry function in user so is nullptr, call Init() first";
    return 1;
  }

  auto stor = entry_();
  if (stor == nullptr)
  {
    LOG(ERROR) << "Get shared_ptr failed, check \"GetKVIntf\" in user "
                  "SO first";
    return 1;
  }

  int write_err = write(stor, thread_index);
  return write_err;
}

int MetaTester::runJobRead(int thread_index)
{
  if (entry_ == nullptr)
  {
    LOG(ERROR) << "Entry function in user so is nullptr, call Init() first";
    return 1;
  }

  auto stor = entry_();
  if (stor == nullptr)
  {
    LOG(ERROR) << "Get store interface shared_ptr failed, check "
                  "\"GetStoreIntf\" in user SO first";
    return 1;
  }
  int read_err = read(stor, thread_index);
  return read_err;
}

double MetaTester::Run(int &err)
{
  if (entry_ == nullptr)
  {
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
  for (int i = 0; i < thread_num; i++)
  {
    std::packaged_task<int()> task(std::bind(&MetaTester::runJobWrite, this, i));
    rets[i] = task.get_future();
    thds[i] = std::thread(std::move(task));
  }
  for (int i = 0; i < thread_num; i++)
  {
    thds[i].join();
  }

  for (int i = 0; i < thread_num; i++)
  {
    err += rets[i].get();
  }

  // for read

  thread_num = 2;
  for (int i = 0; i < thread_num; i++)
  {
    std::packaged_task<int()> task(std::bind(
        &MetaTester::runJobRead, this, i));
    rets[i] = task.get_future();
    thds[i] = std::thread(std::move(task));
  }
  for (int i = 0; i < thread_num; i++)
  {
    thds[i].join();
  }

  for (int i = 0; i < thread_num; i++)
  {
    err += rets[i].get();
  }
  int error = 0;
  int ok = 0;
  auto iter = tree.begin();
  auto stor = entry_();
  if (stor == nullptr)
  {
    LOG(ERROR) << "Get store interface shared_ptr failed, check "
                  "\"GetStoreIntf\" in user SO first";
    return 1;
  }
  while (iter != tree.end())
  {
    //printf("tree key %lu\n", iter->first);
    Data data;
    memset(&data, 0, sizeof(Data));
    uint64_t key = iter->first;
    uint64_t version = 0;
    data.key = key;
    bool first = true;

    for (size_t i = 0; i < iter->second.size(); ++i)
    {
      uint64_t keyversion = iter->second[i].key;
      if (first)
      {
        version = keyversion;
        first = false;
      }
      else
      {
        if (version != keyversion)
        {
          data.version = version;
          Data data1;
          //printf("ReadDataByVersion first %lu\n", version);
          bool result = stor->ReadDataByVersion(key, version, data1);
          if (!result)
          {
            error++;
            printf("ReadDataByVersion error:%lu,%lu\n", key, version);
          }
          else
          {
            if (comdata(data, data1))
            {
              ok++;
            }
            else
            {
              error++;
            }
          }
          version = keyversion;
        }
      }

      for (uint8_t j = 0; j < DATA_FIELD_NUM; j++)
      {
        data.field[j] += iter->second[i].delta[j];
      }
    }
    data.version = version;
    Data data1;
    //printf("ReadDataByVersion %lu\n", version);
    bool result = stor->ReadDataByVersion(key, version, data1);
    if (!result)
    {
      error++;
      printf("ReadDataByVersion error:%lu,%lu\n", key, version);
    }
    else
    {
      if (comdata(data, data1))
      {
        ok++;
      }
      else
      {
        error++;
      }
    }
    iter++;
  }
  printf("ok:%d,error:%d,size:%lu\n", ok, error, tree.size());
  return duration_cast<duration<double>>(system_clock::now() - begin).count();
}

int MetaTester::write(std::shared_ptr<StoreIntf> stor, int thread_index)
{
  if (stor == nullptr)
  {
    return 1;
  }
  /////////////////////////////////////////////////////////////////////////////
  int err = 0;

  DeltaPacket *packet =
      (DeltaPacket *)malloc(sizeof(DeltaPacket) + deltaCount * sizeof(DeltaItem));

  packet->version = ++global_version;

  packet->delta_count = deltaCount;
  for (int j = 0; j < packet->delta_count; j++)
  {
    packet->deltas[j].key = j + thread_index * deltaCount;
    packet->deltas[j].delta[0] = j;
    packet->deltas[j].delta[1] = 1;
    packet->deltas[j].delta[3] = 1;
    packet->deltas[j].delta[7] = j + 2;
    packet->deltas[j].delta[10] = 2 * j;
    auto iter = tree.find(packet->deltas[j].key);
    DeltaItem tmp = packet->deltas[j];
    tmp.key = packet->version;
    if (iter != tree.end())
    {
      auto offset = std::lower_bound(iter->second.begin(), iter->second.end(), tmp, [](const DeltaItem &_First, const DeltaItem &_Last) {
        return _First.key < _Last.key;
      });
      iter->second.insert(offset, tmp);
    }
    else
    {
      tree[packet->deltas[j].key].emplace_back(tmp);
    }
  }
  stor->WriteDeltaPacket(*packet);

  free(packet);
  /////////////////////////////////////////////////////////////////////////////
  return err;
}

int MetaTester::read(std::shared_ptr<StoreIntf> stor, int thread_index)
{
  int err = 0;
  for (int j = 0; j < deltaCount; j++)
  {
    Data data;
    if (stor->ReadDataByVersion(j + thread_index * deltaCount, global_version, data))
    {
      err = 0;
      LOG(INFO) << "get data by key= " << 10086
                << " final result: key=" << data.key
                << ", version=" << data.version
                << ", field[0] = " << data.field[0]
                << ", field[3] = " << data.field[3]
                << ", field[7] = " << data.field[7]
                << ", field[10] = " << data.field[10];
    }
    else
    {
      err = -1;
      // LOG(ERROR) << "get data by key=" << 10086 << ", version=" << i
      //            << " failed";
    }
  }
  for (uint64_t i = 0; i <= global_version; i++)
  {
    
  }
  return err;
}
