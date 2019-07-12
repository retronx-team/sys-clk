/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#ifdef __cplusplus

#include <mutex>
#include <switch.h>

class LockableMutex
{
public:
    LockableMutex()
    {
        mutexInit(&this->m);
    }

    virtual ~LockableMutex() {}

    void Lock()
    {
        mutexLock(&this->m);
    }

    bool TryLock()
    {
        return mutexTryLock(&this->m);
    }
    
    void Unlock()
    {
        mutexUnlock(&this->m);
    }

    // snake_case aliases in order to implement Lockable

    void lock()
    {
        this->Lock();
    }

    bool try_lock()
    {
        return this->TryLock();
    }

    void unlock()
    {
        this->Unlock();
    }

private:
    Mutex m;
};

#endif
