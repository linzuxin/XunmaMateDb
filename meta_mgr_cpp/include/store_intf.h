#pragma once

#include <memory>
#include <vector>

#include "defs.h"

class StoreIntf {
public:
  virtual ~StoreIntf(){};

  virtual bool Init(const char *dir) = 0;

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
   *             there is no change to the data of this key later.
   * @data[out]  data -- data is the final result when you applied all the
   *             versions before or equal the specified 'version'.
   * @return     true -- success.
   *             false -- failed.
   */
  virtual bool ReadDataByVersion(uint64_t key, uint64_t version,
                                 Data &data) = 0;
};

extern "C" std::shared_ptr<StoreIntf> GetStoreIntf();
