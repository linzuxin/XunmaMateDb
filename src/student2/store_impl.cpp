#include "store_impl.h"
#include "replay.h"
#include <string.h>

bool StoreImpl::Init(const char *dir) {
  std::lock_guard<std::mutex> lock(mutex_);
  return true;
}

void StoreImpl::Deinit() {
  std::lock_guard<std::mutex> lock(mutex_);
    tree.clear();
    global_max_version=0;
}

bool StoreImpl::WriteDeltaPacket(const DeltaPacket &packet) {
  std::lock_guard<std::mutex> lock(mutex_);
  global_max_version = std::max(global_max_version,packet.version);
  for(uint16_t i=0;i<packet.delta_count;++i){
      auto iter = tree.find(packet.deltas[i].key);
      DeltaItem tmp = packet.deltas[i];
      tmp.key=packet.version;
      if(iter!=this->tree.end()){
        auto offset = std::lower_bound(iter->second.begin(),iter->second.end(),tmp,[](const DeltaItem &_First,const DeltaItem &_Last){
          return _First.key<_Last.key;
        });
        iter->second.insert(offset,tmp);
      }
      else{
        this->tree[packet.deltas[i].key].emplace_back(tmp);
      }
  }
  return true;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data) {
  std::lock_guard<std::mutex> lock(mutex_);
  if(this->tree.size()==0 || version>global_max_version){
    return false;
  }
  
  // 1.read versions
  auto iter = this->tree.find(key);
  if(iter==this->tree.end() || version<iter->second.front().key){
      return false;
  }
  
  // 2.apply deltas to data
  // Replay::ApplyDeltas(data, deltas);
  memset(&data,0,sizeof(Data));
  int i = 0;
  Replay replay;
  for(;i<iter->second.size();++i){
      if(version<iter->second[i].key){
          break;
      }
      replay.ApplyDelta(data,iter->second[i]);
  }
  data.version = iter->second[i-1].key;
  
  return true;
}