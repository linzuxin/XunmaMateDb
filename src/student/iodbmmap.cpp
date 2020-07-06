
#include "iodbmmap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

void createNmapDb(const char *dataPath)
{
  int fd;
  Data *data = (Data *)malloc(sizeof(Data));
  fd = open(dataPath, O_CREAT | O_RDWR | O_TRUNC, 00666);
  if (fd < 0)
  {
    printf("create open failed\n");
    exit(1);
  }
  lseek(fd, (DATACOUNT - 1) * sizeof(Data), SEEK_CUR);
  write(fd, data, sizeof(Data));
  free(data);
  data = NULL;
  close(fd);
}
int createPathMmapDb(const char *dir, const char *dataPath)
{
  int accessresult = access(dir, 0);
  int mkdirresult = 0;
  if (accessresult == -1)
  {
    mkdirresult = mkdir(dir, 0755);
    createNmapDb(dataPath);
    printf("createNmapDb:%d,%s\n", mkdirresult, dataPath);
  }
  return mkdirresult;
}
Data *initNmapDb(const char *dir)
{
  char dataPath[DIRLEN] = "";
  int fd;
  sprintf(dataPath, "%s/iodb", dir);
  if (createPathMmapDb(dir, dataPath) < 0)
  {
    printf("create open failed\n");
    exit(1);
  }
  fd = open(dataPath, O_RDWR);
  if (fd < 0)
  {
    printf("mmap open failed\n");
    exit(1);
  }
  Data *data = (Data *)mmap(NULL, sizeof(Data) * DATACOUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == (Data *)-1)
  {
    printf("openfailed\n");
    exit(1);
  }
  close(fd);
  return data;
}

void deinitMmapDb(Data *data_)
{
  munmap(data_, sizeof(Data) * DATACOUNT);
  data_ = NULL;
}

bool writeMmapDb(Data *data_, int index, const DeltaPacket &packet)
{
  bool result = true;
  if (data_)
  {
    //Data *dataNode = data_ + index;
    
  }
  return result;
}
bool readMmapDb(Data *data_, uint64_t key, uint64_t version, Data &data)
{
  bool result = true;

  return result;
}