  ----------------------------
  声明定义
  
  std::unordered_map<uint64_t,std::vector<DeltaItem>> tree;
  uint64_t global_max_version = 0;
  1. 声明一个unordered_map数据 map就是红黑树
  1.1  map 里面key 是 uint64_t 类型 
  1.2  map 里面key 是 vector 类型 vector就是数组类型
  1.3  vector里面是 DeltaItem 类型
  2. global_max_version这个全局变量
  
  --------------------------------
  实现写
  
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

1. global_max_version = std::max(global_max_version,packet.version);
求最大的版本号
2. for(uint16_t i=0;i<packet.delta_count;++i){
循环最大的包数
3. auto iter = tree.find(packet.deltas[i].key);
红黑树里面根据key读取value数组  
4. DeltaItem tmp = packet.deltas[i];
声明DeltaItem对象，并赋值为包里面对象
5. tmp.key=packet.version;
key赋值
6. if(iter!=this->tree.end()){
判断是否存在key
7. auto offset = std::lower_bound(iter->second.begin(),iter->second.end(),tmp,[](const DeltaItem &_First,const DeltaItem &_Last){
          return _First.key<_Last.key;
        });
 存在key，则先求出插入位置
8. iter->second.insert(offset,tmp);
 根据位置进行插入。

9. else{
        this->tree[packet.deltas[i].key].emplace_back(tmp);
      }
不存在则插入


-----------------------------------
实现读

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

1. if(this->tree.size()==0 || version>global_max_version){ return false; }
判断版本号是否超过最大版本，超过返回失败
2. auto iter = this->tree.find(key);
获取红黑树数据
3. if(iter==this->tree.end() || version<iter->second.front().key){return false; }
判断红黑树是否存在，并且判断是否大于数组里面最小版本，负责返回失败
4.  memset(&data,0,sizeof(Data));
数据重置0
5. int i = 0; Replay replay;
定义变量i，和Replay对象
6. for(;i<iter->second.size();++i){
      if(version<iter->second[i].key){
          break;
      }
      replay.ApplyDelta(data,iter->second[i]);
  }
  遍历数组对象比版本号小的进行累积
7. data.version = iter->second[i-1].key;
版本号赋值
