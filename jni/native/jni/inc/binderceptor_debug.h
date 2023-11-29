#ifndef __BINDERCEPTOR_DEBUG_H__
#define __BINDERCEPTOR_DEBUG_H__

#include "cmn_macro.h"
#include "jni.h"
#include "binder_info.h"
#include "binder_linux.h"
#include "binder_parcel.h"

class BinderceptorDebug {
public:
    static void onFrameworkBinderBefore(TBinderInfo* pInfo);
    static bool onFrameworkBinderTarget(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token);
    static void onFrameworkBinderAfterFaker(BinderParcel& reply, TBinderInfo& pInfo);
    static void onFrameworkBinderAfterFixer(binder_txn_st* txn, TBinderInfo& pInfo);
    static bool onFrameworkBinderOneway(binder_txn_st* txn, TBinderInfo& pInfo, TBinderTokenItem& token);
};

#endif //__BINDERCEPTOR_DEBUG_H__
