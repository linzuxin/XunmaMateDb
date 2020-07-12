#pragma once
#include <cstdint>
#include "profile_model.h"
#include "key_map.h"

struct SystemModel {
  ProfileModel* profile;
  
  uint64_t field[64];
};
