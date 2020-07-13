
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
    //printf("create open failed %s\n", dataPath);
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
    //printf("createPathMmapDb:%d,%s\n", mkdirresult, dir);
  }
  return mkdirresult;
}
void createFileMmapDb(const char *dataPath, unsigned long datasize)
{
  int accessresult = access(dataPath, 0);
  if (accessresult == -1)
  {
    createMmapDb(dataPath, datasize);
    //printf("createFileMmapDb:%s\n", dataPath);
  }
}

void *init2SizeMmapDb(const char *dir, const char *filename, unsigned long datasize)
{
  char dataPath[DIRLEN] = "";
  int fd;
  sprintf(dataPath, "%s/%s", dir, filename);
  if (createPathMmapDb(dir) < 0)
  {
    //printf("create open failed\n");
    exit(1);
  }
  createFileMmapDb(dataPath, datasize);
  fd = open(dataPath, O_RDWR);
  if (fd < 0)
  {
    //printf("mmap open failed\n");
    exit(1);
  }
  void *index = (void *)mmap(NULL, datasize * DATACOUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (index == (void *)-1)
  {
    //printf("openfailed\n");
    exit(1);
  }
  close(fd);
  return index;
}

MmapDb *initMmapDb(const char *dir)
{
  MmapDb *mmapDb = (MmapDb *)malloc(sizeof(MmapDb));
  mmapDb->dataIoDb = (Data *)init2SizeMmapDb(dir, "iodata", sizeof(Data));
  mmapDb->indexIoDb = (uint64_t *)init2SizeMmapDb(dir, "ioindex", sizeof(uint64_t));
  mmapDb->dataInstance = (Data *)malloc(sizeof(Data));
  mmapDb->rbBst = (RedBlackBSTNode **)malloc(sizeof(RedBlackBSTNode **));
  *(mmapDb->rbBst) = NULL;
  mmapDb->length = 0;
  return mmapDb;
}

void deinitMmapDb(MmapDb *mmapDb)
{
  munmap(mmapDb->dataIoDb, sizeof(Data) * DATACOUNT);
  munmap(mmapDb->indexIoDb, sizeof(uint64_t) * DATACOUNT);
  free(mmapDb->dataInstance);
  mmapDb->dataInstance = NULL;
  free(mmapDb->rbBst);
  mmapDb->rbBst = NULL;
  free(mmapDb);
  mmapDb = NULL;
}

void initDataInfo(DataInfo *nInfo)
{
  nInfo->infoItem.version = 1;
  nInfo->infoItem.lenght = 1;
  nInfo->infoItem.count = 0;
  nInfo->infoItem.index = 0;
  nInfo->infoItem.dataIndex = 0;
}
void AppendDataInfo(DataInfo *nInfo, uint32_t dataIndex)
{
  nInfo->infoItem.version = 1;
  nInfo->infoItem.lenght = 1;
  nInfo->infoItem.count++;
  nInfo->infoItem.index++;
  nInfo->infoItem.dataIndex = dataIndex;
}
void updateDataField(Data *data, uint8_t count, uint8_t pos, uint64_t version, uint64_t fieldSum)
{
  for (uint8_t i = count; i > pos; --i)
  {
    data->field[i * 2] = data->field[(i - 1) * 2];
    data->field[i * 2 + 1] = data->field[i * 2 - 1];
  }
  if (pos < 1)
  {
    pos = 1;
  }
  data->field[(pos - 1) * 2] = version;
  data->field[(pos - 1) * 2 + 1] = fieldSum;
}
uint8_t updateLastField(Data *data, BstNodeValue nodeValue, uint8_t count, uint8_t pos, uint64_t version, uint64_t fieldSum)
{
  uint8_t result = 0;
  if (count == 32)
  {
    result = 2;
    nodeValue->version = version;
    nodeValue->field = fieldSum;
  }
  else
  {
    result = 1;
    data->field[pos * 2] = version;
    data->field[pos * 2 + 1] = fieldSum;
  }
  return result;
}
uint8_t setDataField(Data *data, uint8_t length, BstNodeValue nodeValue, uint64_t version, uint64_t fieldSum)
{
  uint8_t result = 0;
  if (data)
  {
    uint8_t pos = 0;
    uint8_t count = length;
    for (uint8_t i = count; i > 0; --i)
    {
      if (version == data->field[(i - 1) * 2])
      {
        data->field[i * 2 - 1] += fieldSum;
        return result;
      }
      else if (version > data->field[(i - 1) * 2])
      {
        if (i == count)
        {
          return updateLastField(data, nodeValue, count, i, version, fieldSum);
        }
        pos = i;
        break;
      }
    }
    result = 1;
    if (count == 32)
    {
      result = 3;
      nodeValue->version = data->field[(count - 1) * 2];
      nodeValue->field = data->field[(count - 1) * 2 + 1];
    }
    updateDataField(data, count, pos, version, fieldSum);
  }
  return result;
}
void newDataField(MmapDb *mmapDb, Data *data, DataInfo *nInfo, BstNodeValue nodeValue)
{
  uint32_t oldIndex = nodeValue->index;
  nodeValue->index = mmapDb->length++;
  memcpy(mmapDb->dataInstance, (mmapDb->dataIoDb + nodeValue->index), sizeof(Data));
  AppendDataInfo(nInfo, oldIndex);
  data->field[0] = nodeValue->version;
  data->field[1] = nodeValue->field;
  data->version = nInfo->info;
  memcpy((mmapDb->dataIoDb + nodeValue->index), data, sizeof(Data));
}
void setTopDataField(MmapDb *mmapDb, Data *data, DataInfo *nInfo, BstNodeValue nodeValue, uint64_t version, uint64_t fieldSum)
{
  uint8_t setResult = setDataField(data, nInfo->infoItem.lenght, nodeValue, version, fieldSum);
  switch (setResult)
  {
  case 1:
    ++(nInfo->infoItem.lenght);
    data->version = nInfo->info;
  case 0:
    memcpy((mmapDb->dataIoDb + nodeValue->index), data, sizeof(Data));
    break;
  case 3:
    memcpy((mmapDb->dataIoDb + nodeValue->index), data, sizeof(Data));
  case 2:
    newDataField(mmapDb, data, nInfo, nodeValue);
    break;
  default:
    break;
  }
}
void setMoreDataField(MmapDb *mmapDb, Data *data, DataInfo *nInfo, BstNodeValue nodeValue, uint64_t version, uint64_t fieldSum)
{
  uint8_t flag = 0;
  uint8_t first = 1;
  uint32_t dataIndex = nodeValue->index;
  uint32_t oldDataIndex = nodeValue->index;
  uint64_t lastVersion = 0;
  uint64_t lastFieldSum = 0;

  Data dataTempInstance, dataPrevTempInstance;
  Data *dataTemp = &dataTempInstance;
  Data *dataPrevTemp = &dataPrevTempInstance;
  memcpy(dataTemp, (mmapDb->dataIoDb + dataIndex), sizeof(Data));
  DataInfo infoTemp = {0};
  do
  {
    infoTemp.info = dataTemp->version;
    if ((version >= dataTemp->field[0] && version <= dataTemp->field[(32 - 1) * 2]) || infoTemp.infoItem.count == 0)
    {
      uint8_t setResult = setDataField(dataTemp, infoTemp.infoItem.lenght, nodeValue, version, fieldSum);
      switch (setResult)
      {
      case 1:
      case 0:
        first = 0;
        break;
      case 2:
      case 3:
        lastVersion = nodeValue->version;
        lastFieldSum = nodeValue->field;
        break;
      default:
        break;
      }
      flag = 0;
      memcpy((mmapDb->dataIoDb + dataIndex), dataTemp, sizeof(Data));
    }
    else
    {
      flag = 1;
      oldDataIndex = dataIndex;
      dataIndex = infoTemp.infoItem.dataIndex;
      memcpy(dataPrevTemp, (mmapDb->dataIoDb + dataIndex), sizeof(Data));
      lastVersion = dataPrevTemp->field[(32 - 1) * 2];
      lastFieldSum = dataPrevTemp->field[(32 - 1) * 2 + 1];
      setDataField(dataTemp, infoTemp.infoItem.lenght, nodeValue, lastVersion, lastFieldSum);
      memcpy((mmapDb->dataIoDb + oldDataIndex), dataTemp, sizeof(Data));
      dataTemp = dataPrevTemp;
      lastVersion = nodeValue->version;
      lastFieldSum = nodeValue->field;
    }

    if (first == 1)
    {
      first = 0;
      setTopDataField(mmapDb, data, nInfo, nodeValue, lastVersion, lastFieldSum);
    }
  } while (flag == 1);
}
void setDataInfo(MmapDb *mmapDb, BstNodeValue nodeValue, uint64_t version, uint64_t fieldSum)
{
  memcpy(mmapDb->dataInstance, (mmapDb->dataIoDb + nodeValue->index), sizeof(Data));
  Data *data = mmapDb->dataInstance;
  if (data)
  {
    DataInfo nInfo = {0};
    nInfo.info = data->version;
    if (nInfo.infoItem.version == 1)
    {
      if (version >= data->field[0] || nInfo.infoItem.count == 0)
      {
        setTopDataField(mmapDb, data, &nInfo, nodeValue, version, fieldSum);
      }
      else
      {
        setMoreDataField(mmapDb, data, &nInfo, nodeValue, version, fieldSum);
      }
    }
    else
    {
      initDataInfo(&nInfo);
      data->field[0] = version;
      data->field[1] = fieldSum;
      data->version = nInfo.info;
      memcpy((mmapDb->dataIoDb + nodeValue->index), data, sizeof(Data));
    }
  }
}

bool writeMmapDb(MmapDb *mmapDb, const DeltaPacket &packet)
{
  bool result = false;
  if (mmapDb &&
      packet.delta_count > 0 &&
      packet.version > 0)
  {
    result = true;
    for (uint16_t i = 0; i < packet.delta_count; i++)
    {
      uint64_t fieldSum = 0;
      for (uint8_t j = 0; j < DATA_FIELD_NUM; j++)
      {
        fieldSum += packet.deltas[i].delta[j];
      }
      BstNodeValue value = getRoot(*(mmapDb->rbBst), packet.deltas[i].key);
      if (value == NULL)
      {

        value = (BstNodeValue)malloc(sizeof(value));
        value->index = mmapDb->length++;
      }
      //setDataInfo(mmapDb, value, packet.version, fieldSum);
      putRoot(mmapDb->rbBst, packet.deltas[i].key, value);
    }
  }
  return result;
}
uint8_t getMoreDataField(MmapDb *mmapDb, BstNodeValue nodeValue, uint64_t version, Data &data)
{
  uint8_t flag = 1;
  uint8_t first = 1;
  uint32_t dataIndex = nodeValue->index;
  Data *dataTemp = (mmapDb->dataIoDb + nodeValue->index);
  DataInfo infoTemp = {0};
  uint8_t count = 0;
  uint8_t lenght = 0;
  do
  {
    infoTemp.info = dataTemp->version;
    if (version >= dataTemp->field[0])
    {
      for (uint8_t i = infoTemp.infoItem.lenght; i > 0; --i)
      {
        if (version >= dataTemp->field[(i - 1) * 2])
        {
          if (first == 1)
          {
            first = 0;
            count = infoTemp.infoItem.count * 32 + i;
            if (count > 64)
            {
              count = 64;
            }
            data.version = dataTemp->field[(i - 1) * 2];
          }
          ++lenght;
          --count;
          data.field[count] = dataTemp->field[(i - 1) * 2 + 1];
          if (lenght == 64 || count == 0)
          {
            flag = 0;
            break;
          }
        }
      }
    }
    if (lenght == 64 || flag == 0 || infoTemp.infoItem.count == 0)
    {
      flag = 0;
      for (uint8_t i = lenght; i < 64; ++i)
      {
        data.field[i] = 0;
      }
    }
    else
    {
      flag = 1;
      dataIndex = infoTemp.infoItem.dataIndex;
      memcpy(dataTemp, (mmapDb->dataIoDb + dataIndex), sizeof(Data));
    }
  } while (flag == 1);
  return lenght;
}
bool readMmapDb(MmapDb *mmapDb, uint64_t key, uint64_t version, Data &data)
{
  bool result = false;
  BstNodeValue value = getRoot(*(mmapDb->rbBst), key);
  if (value == NULL)
  {
    result = false;
  }
  else
  {
    result = true;
    data.key = key;
    data.version = version;
    // if (getMoreDataField(mmapDb, value, version, data) > 0)
    // {
    //   result = true;
    // }
  }
  return result;
}




ERROR

0/0

用例通过

Null

运行时间

-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found PythonInterp: /usr/local/bin/python (found version "3.5.1") 
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Looking for pthread_create
-- Looking for pthread_create - not found
-- Check if compiler accepts -pthread
-- Check if compiler accepts -pthread - yes
-- Found Threads: TRUE  
-- Configuring done
-- Generating done
-- Build files have been written to: /projects/teacher
Scanning dependencies of target hello
[  6%] Building CXX object src/CMakeFiles/hello.dir/student/hash.cpp.o
[ 13%] Building CXX object src/CMakeFiles/hello.dir/student/profile.cpp.o
[ 20%] Building CXX object src/CMakeFiles/hello.dir/student/replay.cpp.o
[ 26%] Building CXX object src/CMakeFiles/hello.dir/student/store_impl.cpp.o
src/CMakeFiles/hello.dir/build.make:101: recipe for target 'src/CMakeFiles/hello.dir/student/store_impl.cpp.o' failed
CMakeFiles/Makefile2:129: recipe for target 'src/CMakeFiles/hello.dir/all' failed
Makefile:140: recipe for target 'all' failed

CMake Warning:
  Manually-specified variables were not used by the project:

    IS_ONLINE



10488
/projects/teacher/src/student/hash.cpp: In function ‘bool HashSearchIndex(HashItem*, uint64_t, uint64_t, uint64_t, Data&)’:
/projects/teacher/src/student/hash.cpp:183:16: warning: unused variable ‘gnode’ [-Wunused-variable]
  HashLinkNode *gnode;
                ^
In file included from /projects/teacher/src/student/store_impl.cpp:1:0:
/projects/teacher/src/student/store_impl.h:24:35: error: could not convert ‘0’ from ‘int’ to ‘HashItem’
   HashItem hashList[HASH_LEN] = {0};
                                   ^
make[2]: *** [src/CMakeFiles/hello.dir/student/store_impl.cpp.o] Error 1
make[1]: *** [src/CMakeFiles/hello.dir/all] Error 2
make: *** [all] Error 2