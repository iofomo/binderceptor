#ifndef __BINDER_PARCEL_POOL_H__
#define __BINDER_PARCEL_POOL_H__

#include <stdint.h>
#include "cmn_macro.h"

class BinderParcel;
struct ParcelItem {
    BinderParcel* dataPtr;
    struct ParcelItem* next;
};
typedef struct ParcelItem  ParcelItem;
typedef struct ParcelItem* ParcelItemPtr;

class ParcelPool {
public:
    static BinderParcel* parcelNew(const uint8_t* data, size_t dataSize);
    static void parcelFree(BinderParcel* pcl);

#ifdef __ENABLE_UNIT_TEST__
    static void unit_test();
    static int countOfFree();
    static int countOfUse();
#endif

protected:
    static void pushFirst(ParcelItemPtr& itemList, ParcelItem* item);
    static void pushEnd(ParcelItemPtr& itemList, ParcelItem* item);
    static ParcelItem* popItem(ParcelItemPtr& itemList, BinderParcel* addr);
    static ParcelItem* popFirst(ParcelItemPtr& itemList);
    static ParcelItem* newItem();
    static void freeItem(ParcelItem* item);

private:
    static ParcelItemPtr sParcelUse, sParcelFree;
};

class ParcelPoolAutoFree {
public:
    ParcelPoolAutoFree(BinderParcel* pcl) : _pcl(pcl) {}
    ~ParcelPoolAutoFree() { ParcelPool::parcelFree(_pcl); }
private:
    BinderParcel* _pcl;
};

#define __PARCEL_NEW() ParcelPool::parcelNew(NULL, 0)
#define __PARCEL_NEW_DATA(p, q) ParcelPool::parcelNew(p, q)
#define __PARCEL_FREE(pcl) ParcelPool::parcelFree(pcl)
#define __PARCEL_AUTO_FREE(pcl) ParcelPoolAutoFree __uu_mp_af_##pcl(pcl)

#endif //__BINDER_PARCEL_POOL_H__
