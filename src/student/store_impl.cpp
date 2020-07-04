#include "store_impl.h"
#include "iodbmmap.h"
#include "redblackbst.h"
#include <string>
#include <iostream>

std::string toString(RedBlackBSTNode *node)
{
  char a[256] = {0};
  sprintf(a, "%lu,%lu", node->key, node->value);
  std::string s = "(" + std::string(a);
  if (node->left == NULL)
    s += "(";
  else
    s += toString(node->left);
  if (isRed(node))
    s += "*";
  if (node->right == NULL)
    s += ")";
  else
    s += toString(node->right);
  return s + ")";
}

bool StoreImpl::Init(const char *dir)
{
  std::lock_guard<std::mutex> lock(mutex_);
  data_ = initNmapDb(dir);
  RedBlackBSTNode *node = test();
  printf("node-%d-%d-%d\n", size(), height(), heightB());
  std::cout << toString(node);
  return true;
}

void StoreImpl::Deinit()
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (data_)
  {
    deinitMmapDb(data_);
  }
}

bool StoreImpl::WriteDeltaPacket(const DeltaPacket &packet)
{
  std::lock_guard<std::mutex> lock(mutex_);
  for (int i = 0; i < packet.delta_count; i++)
  {
    uint64_t fieldSum = 0;
    for (int j = 0; j < DATA_FIELD_NUM; j++)
    {
      fieldSum += packet.deltas[i].delta[j];
    }
    put(packet.deltas[i].key, fieldSum);
  }
  bool result = true;
  //writeMmapDb(data_, packet);
  return result;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data)
{
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = readMmapDb(data_, key, version, data);
  return result;
}
