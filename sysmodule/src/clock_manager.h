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

#include <atomic>
#include <sysclk.h>

#include "config.h"
#include "clocks.h"
#include "nx/lockable_mutex.h"

class ClockManager
{
  public:
    static ClockManager* GetInstance();
    static void Initialize();
    static void Exit();

    void SetRunning(bool running);
    bool Running();
    void Tick();
    SysClkContext GetCurrentContext();
    Config* GetConfig();

  protected:
    ClockManager();
    virtual ~ClockManager();

    bool RefreshContext();

    static ClockManager *instance;
    std::atomic_bool running;
    LockableMutex contextMutex;
    Config *config;
    SysClkContext *context;
};
