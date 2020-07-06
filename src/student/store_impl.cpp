#include "store_impl.h"
#include <string>
#include <iostream>

std::string toString(RedBlackBSTNode *node)
{
  char a[256] = {0};
  sprintf(a, "%lu", node->key);
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
  mmapDb_ = initMmapDb(dir);
  RedBlackBSTNode *root = test(mmapDb_->rbBst);
  printf("node-%d-%d\n", sizeRoot(root), heightRoot(root));
  //std::cout << toString(root);
  return true;
}

void StoreImpl::Deinit()
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (mmapDb_)
  {
    deinitMmapDb(mmapDb_);
  }
}

bool StoreImpl::WriteDeltaPacket(const DeltaPacket &packet)
{
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = writeMmapDb(mmapDb_, packet);
  return result;
}

bool StoreImpl::ReadDataByVersion(uint64_t key, uint64_t version, Data &data)
{
  std::lock_guard<std::mutex> lock(mutex_);
  bool result = readMmapDb(mmapDb_, key, version, data);
  return result;
}
