
#include "replay.h"

void Replay::ApplyDeltas(Data &data, const std::vector<DeltaItem> &deltas) {
  for (auto &&item : deltas) {
    ApplyDelta(data, item);
  }
}

void Replay::ApplyDelta(Data &data, const DeltaItem &item) {
  for (uint8_t i = 0; i < DATA_FIELD_NUM; i++) {
    data.field[i] += item.delta[i];
  }
}