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
#include <cstdint>
#include <switch.h>
#include <sysclk.h>

class Clocks
{
  public:
    static void Exit();
    static void Initialize();
    static std::uint32_t ResetToStock();
    static SysClkProfile GetCurrentProfile();
    static std::uint32_t GetCurrentHz(SysClkModule module);
    static void SetHz(SysClkModule module, std::uint32_t hz);
    static const char* GetProfileName(SysClkProfile profile, bool pretty);
    static const char* GetModuleName(SysClkModule module, bool pretty);
    static std::uint32_t GetNearestHz(SysClkModule module, SysClkProfile profile, std::uint32_t inHz);

  protected:
    static PcvModule GetPcvModule(SysClkModule SysClkModule);
    static PcvModuleId GetPcvModuleId(SysClkModule SysClkModule);
    static std::uint32_t GetNearestHz(SysClkModule module, std::uint32_t inHz);
    static void GetList(SysClkModule module, std::uint32_t **outClocks, size_t *outClockCount);
    static std::uint32_t GetMaxAllowedHz(SysClkModule module, SysClkProfile profile);
};
