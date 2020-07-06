
#include "iodbmmap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

void createMmapDb(const char *dataPath, unsigned long datasize)
{
  int fd;
  void *data = malloc(datasize);
  fd = open(dataPath, O_CREAT | O_RDWR | O_TRUNC, 00666);
  if (fd < 0)
  {
    printf("create open failed %s\n", dataPath);
    exit(1);
  }
  lseek(fd, (DATACOUNT - 1) * datasize, SEEK_CUR);
  write(fd, data, datasize);
  free(data);
  data = NULL;
  close(fd);
}
int createPathMmapDb(const char *dir)
{
  int accessresult = access(dir, 0);
  int mkdirresult = 0;
  if (accessresult == -1)
  {
    mkdirresult = mkdir(dir, 0755);
    printf("createPathMmapDb:%d,%s\n", mkdirresult, dir);
  }
  return mkdirresult;
}
void createFileMmapDb(const char *dataPath, unsigned long datasize)
{
  int accessresult = access(dataPath, 0);
  if (accessresult == -1)
  {
    createMmapDb(dataPath, datasize);
    printf("createFileMmapDb:%s\n", dataPath);
  }
}

void *init2SizeMmapDb(const char *dir, const char *filename, unsigned long datasize)
{
  char dataPath[DIRLEN] = "";
  int fd;
  sprintf(dataPath, "%s/%s", dir, filename);
  if (createPathMmapDb(dir) < 0)
  {
    printf("create open failed\n");
    exit(1);
  }
  createFileMmapDb(dataPath, datasize);
  fd = open(dataPath, O_RDWR);
  if (fd < 0)
  {
    printf("mmap open failed\n");
    exit(1);
  }
  void *index = (void *)mmap(NULL, datasize * DATACOUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (index == (void *)-1)
  {
    printf("openfailed\n");
    exit(1);
  }
  close(fd);
  return index;
}

MmapDb *initMmapDb(const char *dir)
{
  MmapDb *mmapDb = (MmapDb *)malloc(sizeof(MmapDb));
  mmapDb->data = (Data *)init2SizeMmapDb(dir, "iodata", sizeof(Data));
  mmapDb->index = (uint64_t *)init2SizeMmapDb(dir, "ioindex", sizeof(uint64_t));
  mmapDb->rbBst = (RedBlackBSTNode **)init2SizeMmapDb(dir, "ioindex", sizeof(RedBlackBSTNode *));
  *(mmapDb->rbBst) = NULL;
  return mmapDb;
}

void deinitMmapDb(MmapDb *mmapDb)
{
  munmap(mmapDb->data, sizeof(Data) * DATACOUNT);
  munmap(mmapDb->index, sizeof(uint64_t) * DATACOUNT);
  free(mmapDb->rbBst);
  free(mmapDb);
  mmapDb = NULL;
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