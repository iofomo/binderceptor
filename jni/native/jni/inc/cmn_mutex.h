#ifndef __CMN_MUTEX_H__
#define __CMN_MUTEX_H__

#include <pthread.h>
#include "cmn_macro.h"

class MutexAutoRelease {
public:
    MutexAutoRelease(pthread_mutex_t* mutex):_arMutex(mutex){
        if (__LIKELY(_arMutex)) pthread_mutex_lock(_arMutex);
    }
    ~MutexAutoRelease() {
        if (__LIKELY(_arMutex)) pthread_mutex_unlock(_arMutex);
    }

private:
    pthread_mutex_t* _arMutex;
};

#define __DEF_AUTO_RELEASE_MUTEX(name) static pthread_mutex_t g_mutex_##name = PTHREAD_MUTEX_INITIALIZER
#define __AUTO_RELEASE_MUTEX(name) MutexAutoRelease __auto_release_mutex(&g_mutex_##name)

#endif //__CMN_MUTEX_H__
