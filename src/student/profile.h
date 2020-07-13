#pragma once
#include <cstdint>

struct Profile
{
  uint64_t bufSize;
  uint64_t sysVersion;
  uint64_t sysDateTime;
  uint64_t dataVersion;
  uint64_t count;
};

Profile *initProfile();
void setDataVersion(Profile *profile, uint64_t version);
uint64_t getDataVersion(Profile *profile);
bool validateDataVersion(Profile *profile, uint64_t version);