#include "../include/common.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    std::shared_ptr<StoreIntf> intf = GetStoreIntf();
    intf->Init("./test_data/");
    uint64_t key = 1024;
    uint64_t version = 1111;
    uint16_t delta_count = 64;
    uint16_t delta_length = 1000;
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
                packet->deltas[j].delta[i] = i;
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
                if (data.field[1] != packet->deltas[j].delta[1])
                {
                    printf("error field:%lu,%d,%d\n", data.field[1], packet->deltas[j].delta[i], i);
                }
            }
        }
    }

    Data data;
    bool result = intf->ReadDataByVersion(key, version, data);
    printf("%d,%lu,%lu,%lu\n", result, data.key, data.version, data.field[0]);
    intf->Deinit();
    return 0;
}
