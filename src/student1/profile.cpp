#include <stdlib.h>
#include "profile.h"

Profile *initProfile()
{
  Profile *profile = (Profile *)malloc(sizeof(Profile));
  profile->bufSize = 1024;
  profile->sysVersion = 1000000;
  profile->sysDateTime = 20200710141100;
  profile->dataVersion = 0;
  profile->count = 0;
  return profile;
}
void setDataVersion(Profile *profile, uint64_t version)
{
  if (version >= profile->dataVersion)
  {
    profile->dataVersion = version;
    profile->count++;
  }
}

uint64_t getDataVersion(Profile *profile)
{
  return profile->dataVersion;
}

bool validateDataVersion(Profile *profile, uint64_t version)
{
  bool result = true;
  if (version > profile->dataVersion)
  {
    result = false;
  }
  else if (profile->count == 0)
  {
    result = false;
  }
  return result;
}