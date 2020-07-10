#pragma once
#include <cstdint>

struct ProfileModel {
  uint64_t bufSize;
  uint64_t sysVersion;
  uint64_t sysDateTime;
  uint64_t dataVersion;
};

ProfileModel* initProfileModel();
void setDataVersion(ProfileModel *profile, uint64_t version);
uint64_t getDataVersion(ProfileModel *profile);