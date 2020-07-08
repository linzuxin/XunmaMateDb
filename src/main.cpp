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
            for (uint8_t i = 0; i < DATA_FIELD_NUM; i++)
            {
                packet->deltas[j].delta[i] = i;
            }
        }
        printf("delta_length:%d\n",k);
        intf->WriteDeltaPacket(*packet.get());
    }

    Data data;
    bool result = intf->ReadDataByVersion(key, version, data);
    printf("%d,%lu,%lu,%lu\n", result, data.key, data.version, data.field[0]);
    intf->Deinit();
    return 0;
}
