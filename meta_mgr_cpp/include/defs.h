#pragma once
#include <cstdint>
constexpr uint8_t DATA_FIELD_NUM = 64;
struct Data {
  uint64_t key;
  // If we applied the verion of the delta, we say the data has this version
  uint64_t version;
  uint64_t field[DATA_FIELD_NUM];
};

struct DeltaItem {
  uint64_t key;
  // the change of the data compare to previous version.
  uint32_t delta[DATA_FIELD_NUM];
};

// all the deltas in one DeltaPacketket have the same version
// DeltaItem can have same or different keys.
struct DeltaPacket {
  // the global sequence number of the packet
  uint64_t version;
  uint16_t delta_count;
  DeltaItem deltas[0];
};
