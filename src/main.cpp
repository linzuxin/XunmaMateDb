#include "../include/common.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    std::shared_ptr<StoreIntf> intf = GetStoreIntf();
    intf->Init("./test_data/");
    uint64_t key = 1024;
    uint64_t version = 1111;
    uint16_t delta_count = 2;

    int packet_len = sizeof(DeltaPacket) + delta_count * sizeof(DeltaItem);
    std::unique_ptr<DeltaPacket> packet((DeltaPacket *)new char[packet_len]);
    packet->version = version;
    packet->delta_count = delta_count;
    packet->deltas[0].key = key;
    for (uint8_t i = 0; i < DATA_FIELD_NUM; i++)
    {
        packet->deltas[0].delta[i] = i;
    }
    packet->deltas[1].key = key;
    for (uint8_t i = 0; i < DATA_FIELD_NUM; i++)
    {
        packet->deltas[1].delta[i] = i;
    }
    intf->WriteDeltaPacket(*packet.get());

    Data data;
    bool result = intf->ReadDataByVersion(key, version, data);
    printf("%d,%lu,%lu,%lu\n", result,data.key, data.version, data.field[0]);
    intf->Deinit();
    return 0;
}
