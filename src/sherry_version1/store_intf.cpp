#include "common.h"
#include "store_impl.h"

static std::shared_ptr<StoreImpl> g_store(new StoreImpl);

std::shared_ptr<StoreIntf> GetStoreIntf() { return g_store->GetPtr(); }
