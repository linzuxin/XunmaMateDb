#include "../include/common.h"
#include <iostream>
using namespace std;
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string.h>

bool comdata(Data &data, Data &data1)
{
    bool result = true;
    if (data.key != data1.key)
    {
        result = false;
    }
    if (data.version != data1.version)
    {
        result = false;
    }
    uint8_t i = 1;
    //for (uint8_t i = 0; i < DATA_FIELD_NUM; i++)
    {
        if (data.field[i] != data1.field[i])
        {
            result = false;
            printf("comdata error field:%lu,%lu,%d\n", data.field[i], data1.field[i], i);
        }
    }
    if (!result)
    {
        printf("comdata error key:%lu,%lu version:%lu,%lu\n", data.key, data1.key, data.version, data1.version);
    }
    return result;
}

int main(int argc, char *argv[])
{
    std::shared_ptr<StoreIntf> intf = GetStoreIntf();
    intf->Init("./test_data/");
    uint64_t key = 1024;
    uint64_t version = 1111;
    uint16_t delta_count = 64;
    uint16_t delta_length = 1000;

    std::unordered_map<uint64_t, std::vector<DeltaItem>> tree;

    for (uint32_t k = 0; k < delta_length; k++)
    {
        int packet_len = sizeof(DeltaPacket) + delta_count * sizeof(DeltaItem);
        std::unique_ptr<DeltaPacket> packet((DeltaPacket *)new char[packet_len]);
        packet->version = k;
        packet->delta_count = delta_count;
        for (uint8_t j = 0; j < delta_count; j++)
        {
            packet->deltas[j].key = j + k * delta_count;
            //printf("insert %lu,%lu\n", packet->deltas[j].key, packet->version);
            for (uint8_t i = 0; i < DATA_FIELD_NUM; i++)
            {
                packet->deltas[j].delta[i] = 1;
            }

            auto iter = tree.find(packet->deltas[j].key);
            DeltaItem tmp = packet->deltas[j];
            tmp.key = packet->version;
            if (iter != tree.end())
            {
                auto offset = std::lower_bound(iter->second.begin(), iter->second.end(), tmp, [](const DeltaItem &_First, const DeltaItem &_Last) {
                    return _First.key < _Last.key;
                });
                iter->second.insert(offset, tmp);
            }
            else
            {
                tree[packet->deltas[j].key].emplace_back(tmp);
            }
        }

        intf->WriteDeltaPacket(*packet.get());
        for (uint8_t j = 0; j < delta_count; j++)
        {
            Data data;
            bool result = intf->ReadDataByVersion(j + k * delta_count, k, data);
            if (data.key != j + k * delta_count)
            {
                printf("error key:%d,%lu\n", j + k * delta_count, data.key);
            }
            if (data.version != k)
            {
                printf("error version:%d,%lu\n", k, data.version);
            }
            for (uint8_t i = 0; i < DATA_FIELD_NUM; i++)
            {
                if (data.field[i] != packet->deltas[j].delta[i])
                {
                    printf("error field:%lu,%d,%d\n", data.field[1], packet->deltas[j].delta[i], i);
                }
            }
        }
    }

    for (uint32_t k = 0; k < delta_length; k++)
    {
        int packet_len = sizeof(DeltaPacket) + delta_count * sizeof(DeltaItem);
        std::unique_ptr<DeltaPacket> packet((DeltaPacket *)new char[packet_len]);
        packet->version = k % 2;
        packet->delta_count = delta_count;
        for (uint8_t j = 0; j < delta_count; j++)
        {
            packet->deltas[j].key = j;
            //printf("insert2 %lu,%lu\n", packet->deltas[j].key, packet->version);
            for (uint8_t i = 0; i < DATA_FIELD_NUM; i++)
            {
                packet->deltas[j].delta[i] = 1;
            }

            auto iter = tree.find(packet->deltas[j].key);
            DeltaItem tmp = packet->deltas[j];
            tmp.key = packet->version;
            if (iter != tree.end())
            {
                auto offset = std::lower_bound(iter->second.begin(), iter->second.end(), tmp, [](const DeltaItem &_First, const DeltaItem &_Last) {
                    return _First.key < _Last.key;
                });
                iter->second.insert(offset, tmp);
            }
            else
            {
                tree[packet->deltas[j].key].emplace_back(tmp);
            }
        }

        intf->WriteDeltaPacket(*packet.get());
    }
    int error = 0;
    int ok = 0;
    auto iter = tree.begin();
    while (iter != tree.end())
    {
        //printf("tree key %lu\n", iter->first);
        Data data;
        memset(&data, 0, sizeof(Data));
        uint64_t key = iter->first;
        uint64_t version = 0;
        data.key = key;
        bool first = true;

        for (size_t i = 0; i < iter->second.size(); ++i)
        {
            uint64_t keyversion = iter->second[i].key;
            if (first)
            {
                version = keyversion;
                first = false;
            }
            else
            {
                if (version != keyversion)
                {
                    data.version = version;
                    Data data1;
                    //printf("ReadDataByVersion first %lu\n", version);
                    bool result = intf->ReadDataByVersion(key, version, data1);
                    if (!result)
                    {
                        error++;
                        printf("ReadDataByVersion error:%lu,%lu\n", key, version);
                    }
                    else
                    {
                        if (comdata(data, data1))
                        {
                            ok++;
                        }
                        else
                        {
                            error++;
                        }
                    }
                    version = keyversion;
                }
            }

            for (uint8_t j = 0; j < DATA_FIELD_NUM; j++)
            {
                data.field[j] += iter->second[i].delta[j];
            }
        }
        data.version = version;
        Data data1;
        //printf("ReadDataByVersion %lu\n", version);
        bool result = intf->ReadDataByVersion(key, version, data1);
        if (!result)
        {
            error++;
            printf("ReadDataByVersion error:%lu,%lu\n", key, version);
        }
        else
        {
            if (comdata(data, data1))
            {
                ok++;
            }
            else
            {
                error++;
            }
        }
        iter++;
    }
    printf("ok:%d,error:%d,size:%lu\n", ok, error, tree.size());
    intf->Deinit();
    return 0;
}
