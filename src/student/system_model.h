#pragma once
#include <cstdint>
#include "profile_model.h"

struct SystemModel {
  ProfileModel* profile;
  uint64_t field[64];
};
