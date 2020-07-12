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