#pragma once
#include <cstdint>
#include <memory>
#include <vector>

constexpr uint8_t DATA_FIELD_NUM = 64;

struct Data {
  uint64_t key;
  // If we applied the verion of the delta, we say the data has this version
  uint64_t version;
  uint64_t field[DATA_FIELD_NUM];
};

struct DeltaItem {
  uint64_t key;
  // the change of the data compare to previous version.
  uint32_t delta[DATA_FIELD_NUM];
};

// all the deltas in one DeltaPacketket have the same version
// DeltaItem can have same or different keys.
struct DeltaPacket {
  // the global sequence number of the packet
  uint64_t version;
  uint16_t delta_count;
  DeltaItem deltas[];
};

class StoreIntf {
public:
  virtual ~StoreIntf(){};

  /** init the store
   * @param[in]  dir  -- the data will be saved to this dir
   * @return     true -- success.
   *             false -- failed.
   */
  virtual bool Init(const char *dir) = 0;

  /** deinit resources */
  virtual void Deinit() = 0;

  /** write delta packet. delta is the changed parts compare to the previous
   * version.
   * @param[in]  packet  -- the delta packet
   * @return     true -- success.
   *             false -- failed.
   */
  virtual bool WriteDeltaPacket(const DeltaPacket &packet) = 0;

  /* read the replayed data  by version. data is the final result when
   * applied all the deltas before or equal the specified version.
   * @param[in]  key  -- the key to read
   * @param[in]  version -- version is the sequence of the delta packet. If we
   *             have applied the delta, this is also the data's version. e.g. ： The delta
   *             has version 3, 4 ,5 ,6.  If we applied delta version 3, 4, 5 to data, the
   *             data's verion is 5. If we want to read data with version 2, there is no
   *             this version, we should return false to caller. If we want to read data
   *             with verion 10, we should apply delta version 3, 4, 5, 6. And return data
   *             with version 6 to user. Data with version 6 is the proper version, because
   *             there is no change to the data of this key later. If the read verson is
   *             bigger than the global verison we have every written, we should return
   *             false to the caller.
   * @data[out]  data -- data is the final result when you applied all the
   *             versions before or equal the specified 'version'.
   * @return     true -- success.
   *             false -- failed.
   */
  virtual bool ReadDataByVersion(uint64_t key, uint64_t version,
                                 Data &data) = 0;
};

// get the singleton StoreIntf object ptr
extern "C" std::shared_ptr<StoreIntf> GetStoreIntf();