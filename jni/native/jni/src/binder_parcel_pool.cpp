#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "binder_parcel_pool.h"
#include "binder_parcel.h"

//#define __ENABLE_LOG_PARCEL_POOL__     1

#ifdef __ENABLE_LOG_PARCEL_POOL__
static int g_pcl_cnt = 0;
static int g_pcl_pool_cnt = 0;
#endif

ParcelItemPtr ParcelPool::sParcelUse = NULL;
ParcelItemPtr ParcelPool::sParcelFree = NULL;
static pthread_mutex_t g_parcel_mutex = PTHREAD_MUTEX_INITIALIZER;
// -----------------------------------------------------------------------------
BinderParcel* ParcelPool::parcelNew(const uint8_t* data, size_t dataSize) {
    ParcelItem* item;
    BinderParcel* pcl = NULL;

    pthread_mutex_lock(&g_parcel_mutex);
    if (sParcelFree) {
        item = ParcelPool::popFirst(sParcelFree);
    } else {
        item = ParcelPool::newItem();
#ifdef __ENABLE_LOG_PARCEL_POOL__
        if (__LIKELY(item)) {
            LOGD("ParcelPool::new cnt: %d", ++g_pcl_cnt)
        }
#endif
    }
    if (__LIKELY(item)) {
        pcl = item->dataPtr;
        ParcelPool::pushFirst(sParcelUse, item);
#ifdef __ENABLE_LOG_PARCEL_POOL__
        LOGD("ParcelPool::pool cnt: %d", ++g_pcl_pool_cnt)
#endif
    }
    pthread_mutex_unlock(&g_parcel_mutex);

    __ASSERT(pcl)
    if (__LIKELY(pcl)) {
        if (__LIKELY(data && 0 < dataSize)) {
            pcl->setDataForReader(data, dataSize);
        } else {
            pcl->setDataForWriter(256);
        }
    }
    return pcl;
}

void ParcelPool::parcelFree(BinderParcel* pcl) {
    if (__UNLIKELY(!pcl)) return ;

    pthread_mutex_lock(&g_parcel_mutex);
    if (__LIKELY(sParcelUse)) {
        ParcelItem* item = ParcelPool::popItem(sParcelUse, pcl);
        if (item) {
            pcl->freeData();
            pcl = NULL;
            ParcelPool::pushFirst(sParcelFree, item);
#ifdef __ENABLE_LOG_PARCEL_POOL__
            LOGD("ParcelPool::pool cnt: %d", --g_pcl_pool_cnt)
#endif
        }
    }
    pthread_mutex_unlock(&g_parcel_mutex);
    MEM_DELETE(pcl)
}

void ParcelPool::pushFirst(ParcelItemPtr& itemList, ParcelItem* item) {
    item->next = itemList;
    itemList = item;
}

void ParcelPool::pushEnd(ParcelItemPtr& itemList, ParcelItem* item) {
    if (itemList) {
        ParcelItem* p = itemList;
        while (p->next) p = p->next;

        item->next = p->next;
        p->next = item;
    } else {
        item->next = itemList;
        itemList = item;
    }
}

ParcelItem* ParcelPool::popItem(ParcelItemPtr& itemList, BinderParcel* pcl) {
    if (pcl == itemList->dataPtr) {
        return ParcelPool::popFirst(itemList);
    }

    ParcelItem* p = itemList;
    ParcelItem* q = p->next;
    while (q && pcl != q->dataPtr) {
        p = q;
        q = p->next;
    }

    if (q) {
        p->next = q->next;
    }
    return q;
}

ParcelItem* ParcelPool::popFirst(ParcelItemPtr& itemList) {
    ParcelItem* tmpPtr = itemList;
    itemList = itemList->next;
    return tmpPtr;
}

ParcelItem* ParcelPool::newItem() {
    ParcelItem* item = (ParcelItem*)::malloc(sizeof(ParcelItem));
    if (__LIKELY(item)) {
        memset(item, 0, sizeof(ParcelItem));
        item->dataPtr = new BinderParcel();
        if (__UNLIKELY(NULL == item->dataPtr)) {
            ParcelPool::freeItem(item);
            item = NULL;
        }
    }
    return item;
}

void ParcelPool::freeItem(ParcelItem* item) {
    if (__LIKELY(item)) {
        MEM_DELETE(item->dataPtr)
        ::free(item);
    }
}

#ifdef __ENABLE_UNIT_TEST__
int ParcelPool::countOfFree() {
    int cnt = 0;
    if (sParcelFree) {
        ParcelItem* p = sParcelFree;
        while (p) {
            ++cnt;
            p = p->next;
        }
    }
    return cnt;
}

int ParcelPool::countOfUse() {
    int cnt = 0;
    if (sParcelUse) {
        ParcelItem* p = sParcelUse;
        while (p) {
            ++cnt;
            p = p->next;
        }
    }
    return cnt;
}

void ParcelPool::unit_test() {
    LOGD("ParcelPool::unit_test >>>")
    uint8_t data8[10];
    size_t dataSize = sizeof(data8);

    BinderParcel* addr = __PARCEL_NEW();
    __ASSERT(addr)
    __ASSERT(1 == ParcelPool::countOfUse())
    __ASSERT(0 == ParcelPool::countOfFree())
    __PARCEL_FREE(addr);
    __ASSERT(0 == ParcelPool::countOfUse())
    __ASSERT(1 == ParcelPool::countOfFree())

    BinderParcel* addr1 = __PARCEL_NEW_DATA(data8, dataSize);
    __ASSERT(addr1)
    __ASSERT(addr == addr1)
    __ASSERT(dataSize == addr1->dataSize())
    __ASSERT(1 == ParcelPool::countOfUse())
    __ASSERT(0 == ParcelPool::countOfFree())

    BinderParcel* addr2 = __PARCEL_NEW();
    __ASSERT(2 == ParcelPool::countOfUse())
    __ASSERT(0 == ParcelPool::countOfFree())

    BinderParcel* addr3 = __PARCEL_NEW_DATA(data8, dataSize);
    __ASSERT(3 == ParcelPool::countOfUse())
    __ASSERT(0 == ParcelPool::countOfFree())
    __ASSERT(dataSize == addr3->dataSize())
    __PARCEL_FREE(addr1);
    __ASSERT(2 == ParcelPool::countOfUse())
    __ASSERT(1 == ParcelPool::countOfFree())

    BinderParcel* addr4 = __PARCEL_NEW();
    __ASSERT(3 == ParcelPool::countOfUse())
    __ASSERT(0 == ParcelPool::countOfFree())

    __PARCEL_FREE(addr2);
    __ASSERT(2 == ParcelPool::countOfUse())
    __ASSERT(1 == ParcelPool::countOfFree())

    __PARCEL_FREE(addr3);
    __ASSERT(1 == ParcelPool::countOfUse())
    __ASSERT(2 == ParcelPool::countOfFree())

    __PARCEL_FREE(addr4);
    __ASSERT(0 == ParcelPool::countOfUse())
    __ASSERT(3 == ParcelPool::countOfFree())
    {
        BinderParcel* addr10 = __PARCEL_NEW();
        __PARCEL_AUTO_FREE(addr10);
        __ASSERT(addr10 == addr2 || addr10 == addr3 || addr10 == addr4)
        __ASSERT(1 == ParcelPool::countOfUse())
        __ASSERT(2 == ParcelPool::countOfFree())
        __ASSERT(0 == addr10->dataSize())

        BinderParcel* addr11 = __PARCEL_NEW_DATA(data8, dataSize);
        __PARCEL_AUTO_FREE(addr11);
        __ASSERT(addr11 == addr2 || addr11 == addr3 || addr11 == addr4)
        __ASSERT(2 == ParcelPool::countOfUse())
        __ASSERT(1 == ParcelPool::countOfFree())
        __ASSERT(dataSize == addr11->dataSize())

        BinderParcel* addr12 = __PARCEL_NEW();
        __PARCEL_AUTO_FREE(addr12);
        __ASSERT(addr12 == addr2 || addr12 == addr3 || addr12 == addr4)
        __ASSERT(3 == ParcelPool::countOfUse())
        __ASSERT(0 == ParcelPool::countOfFree())
        __ASSERT(0 == addr12->dataSize())

        BinderParcel* addr13 = __PARCEL_NEW();
        __PARCEL_AUTO_FREE(addr13);
        __ASSERT(addr13 != addr10 && addr13 != addr11 && addr13 != addr12)
        __ASSERT(4 == ParcelPool::countOfUse())
        __ASSERT(0 == ParcelPool::countOfFree())
        __ASSERT(0 == addr13->dataSize())
    }
    __ASSERT(0 == ParcelPool::countOfUse())
    __ASSERT(4 == ParcelPool::countOfFree())

    LOGD("ParcelPool::unit_test <<<")
}
#endif