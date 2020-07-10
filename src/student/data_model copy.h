#pragma once
#include <cstdint>

struct DataModel {
  uint64_t version;
  uint64_t field[64];
};
