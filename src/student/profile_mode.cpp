#include <stdlib.h>
#include "profile_model.h"

ProfileModel *initProfileModel()
{
  ProfileModel *profile = (ProfileModel *)malloc(sizeof(ProfileModel));
  profile->bufSize = 1024;
  profile->sysVersion = 1000000;
  profile->sysDateTime = 20200710141100;
  profile->dataVersion = 0;
  return profile;
}
void setDataVersion(ProfileModel *profile,uint64_t version){
  profile->dataVersion = version;
}

uint64_t getDataVersion(ProfileModel *profile){
  return profile->dataVersion;
}