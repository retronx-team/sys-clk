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
#include <nxExt.h>
#include <sysclk.h>
#include "clock_manager.h"

class IpcService
{
  public:
    IpcService(ClockManager* clockMgr);
    virtual ~IpcService();
    void SetRunning(bool running);

  protected:
    static void ProcessThreadFunc(void* arg);
    static Result ServiceHandlerFunc(void* arg, const IpcServerRequest* r, std::uint8_t* out_data, size_t* out_dataSize);

    Result GetApiVersion(u32* out_version);
    Result GetVersionString(char* out_buf, size_t bufSize);
    Result GetCurrentContext(SysClkContext* out_ctx);
    Result Exit();
    Result GetProfileCount(std::uint64_t* tid, std::uint8_t* out_count);
    Result GetProfiles(std::uint64_t* tid, SysClkTitleProfileList* out_profiles);
    Result SetProfiles(SysClkIpc_SetProfiles_Args* args);
    Result SetEnabled(std::uint8_t* enabled);
    Result SetOverride(SysClkIpc_SetOverride_Args* args);
    Result GetConfigValues(SysClkConfigValueList* out_configValues);
    Result SetConfigValues(SysClkConfigValueList* configValues);
    Result GetFreqList(SysClkIpc_GetFreqList_Args* args, std::uint32_t* out_list, std::size_t size, std::uint32_t* out_count);

    bool running;
    Thread thread;
    LockableMutex threadMutex;
    IpcServer server;
    ClockManager* clockMgr;
};
