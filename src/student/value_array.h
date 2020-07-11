#pragma once
#include <cstdint>

enum ValueType
{
  ValueByte1 = 1, //1个字节
  ValueByte2 = 2, //2个字节
  ValueByte3 = 4, //4个字节
  ValueByte4 = 8, //8个字节
};

union ValueInfo
{
  struct
  {
    uint8_t count;      //版本号个数
    uint8_t type;       //类型ValueType
    uint8_t location;   //类型所在的位置
    uint8_t fileNo;     //文件块编号
    uint32_t dataIndex; //上个版本位置
  } infoItem;
  uint64_t info;
};

struct ValueArray
{
  uint64_t version;
  uint64_t field[64];
};


static uint32_t fmix(uint32_t h)
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}

static uint32_t Hash32Len5to12(const char *s, size_t len) {
  uint32_t a = len, b = len * 5, c = 9, d = b;
  a += Fetch32(s);
  b += Fetch32(s + len - 4);
  c += Fetch32(s + ((len >> 1) & 4));
  return fmix(Mur(c, Mur(b, Mur(a, d))));
}