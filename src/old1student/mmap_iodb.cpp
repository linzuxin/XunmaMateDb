#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mmap_iodb.h"

void createMmapIodb(const char *dataPath, unsigned long datasize)
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
int createPathMmapIodb(const char *dir)
{
  int accessresult = access(dir, 0);
  int mkdirresult = 0;
  if (accessresult == -1)
  {
    mkdirresult = mkdir(dir, 0755);
    //printf("createPathMmapIodb:%d,%s\n", mkdirresult, dir);
  }
  return mkdirresult;
}
void createFileMmapIodb(const char *dataPath, unsigned long datasize)
{
  int accessresult = access(dataPath, 0);
  if (accessresult == -1)
  {
    createMmapIodb(dataPath, datasize);
    //printf("createFileMmapIodb:%s\n", dataPath);
  }
}

void *init2SizeMmapIodb(const char *dir, const char *filename, unsigned long datasize)
{
  char dataPath[DIRLEN] = "";
  int fd;
  sprintf(dataPath, "%s/%s", dir, filename);
  if (createPathMmapIodb(dir) < 0)
  {
    //printf("create open failed\n");
    exit(1);
  }
  createFileMmapIodb(dataPath, datasize);
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

MmapIodb *initMmapIodb(const char *dir)
{
  MmapIodb *mmapDb = (MmapIodb *)malloc(sizeof(MmapIodb));
  mmapDb->dataIoDb = (Data *)init2SizeMmapIodb(dir, "iodata", sizeof(Data));
  mmapDb->indexIoDb = (uint64_t *)init2SizeMmapIodb(dir, "ioindex", sizeof(uint64_t));
  mmapDb->dataInstance = (Data *)malloc(sizeof(Data));
  mmapDb->rbBst = (KeyMap **)malloc(sizeof(KeyMap **));
  *(mmapDb->rbBst) = NULL;
  mmapDb->length = 0;
  return mmapDb;
}

void deinitMmapIodb(MmapIodb *mmapDb)
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
    }
    updateDataField(data, count, pos, version, fieldSum);
  }
  return result;
}
void newDataField(MmapIodb *mmapDb, Data *data, DataInfo *nInfo, BstNodeValue nodeValue)
{
  uint32_t oldIndex = nodeValue->index;
  nodeValue->index = mmapDb->length++;
  memcpy(mmapDb->dataInstance, (mmapDb->dataIoDb + nodeValue->index), sizeof(Data));
  AppendDataInfo(nInfo, oldIndex);
  data->field[0] = nodeValue->version;
  data->version = nInfo->info;
  memcpy((mmapDb->dataIoDb + nodeValue->index), data, sizeof(Data));
}
void setTopDataField(MmapIodb *mmapDb, Data *data, DataInfo *nInfo, BstNodeValue nodeValue, uint64_t version, uint64_t fieldSum)
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
void setMoreDataField(MmapIodb *mmapDb, Data *data, DataInfo *nInfo, BstNodeValue nodeValue, uint64_t version, uint64_t fieldSum)
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
    }

    if (first == 1)
    {
      first = 0;
      setTopDataField(mmapDb, data, nInfo, nodeValue, lastVersion, lastFieldSum);
    }
  } while (flag == 1);
}
void setDataInfo(MmapIodb *mmapDb, BstNodeValue nodeValue, uint64_t version, uint64_t fieldSum)
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

bool writeMmapIodb(MmapIodb *mmapDb, const DeltaPacket &packet)
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
uint8_t getMoreDataField(MmapIodb *mmapDb, BstNodeValue nodeValue, uint64_t version, Data &data)
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
bool readMmapIodb(MmapIodb *mmapDb, uint64_t key, uint64_t version, Data &data)
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