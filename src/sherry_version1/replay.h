#pragma once

#include "common.h"
#include <vector>

class Replay {
public:
  static void ApplyDeltas(Data &data, const std::vector<DeltaItem> &deltas);
  static void ApplyDelta(Data &data, const DeltaItem &item);
};