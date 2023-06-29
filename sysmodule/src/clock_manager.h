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
#include "board.h"
#include <nxExt/cpp/lockable_mutex.h>

class ClockManager
{
  public:
    ClockManager();
    virtual ~ClockManager();

    SysClkContext GetCurrentContext();
    Config* GetConfig();
    void SetRunning(bool running);
    bool Running();
    void GetFreqList(SysClkModule module, std::uint32_t* list, std::uint32_t maxCount, std::uint32_t* outCount);
    void Tick();
    void WaitForNextTick();

  protected:
    bool IsAssignableHz(SysClkModule module, std::uint32_t hz);
    std::uint32_t GetMaxAllowedHz(SysClkModule module, SysClkProfile profile);
    std::uint32_t GetNearestHz(SysClkModule module, std::uint32_t inHz, std::uint32_t maxHz);
    bool ConfigIntervalTimeout(SysClkConfigValue intervalMsConfigValue, std::uint64_t ns, std::uint64_t* lastLogNs);
    void RefreshFreqTableRow(SysClkModule module);
    bool RefreshContext();

    std::atomic_bool running;
    LockableMutex contextMutex;
    struct {
      std::uint32_t count;
      std::uint32_t list[SYSCLK_FREQ_LIST_MAX];
    } freqTable[SysClkModule_EnumMax];
    Config* config;
    SysClkContext* context;
    std::uint64_t lastTempLogNs;
    std::uint64_t lastFreqLogNs;
    std::uint64_t lastPowerLogNs;
    std::uint64_t lastCsvWriteNs;
};
