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
#include <string>
#include <switch.h>

typedef enum
{
    ClockProfile_Handheld = 0,
    ClockProfile_HandheldCharging,
    ClockProfile_HandheldChargingUSB,
    ClockProfile_HandheldChargingOfficial,
    ClockProfile_Docked,
    ClockProfile_EnumMax
} ClockProfile;

typedef enum
{
    ClockModule_CPU = 0,
    ClockModule_GPU,
    ClockModule_MEM,
    ClockModule_EnumMax
} ClockModule;

class Clocks
{
  public:
    static void Exit();
    static void Initialize();
    static std::uint32_t ResetToStock();
    static ClockProfile GetCurrentProfile();
    static std::uint32_t GetCurrentHz(ClockModule module);
    static void SetHz(ClockModule module, std::uint32_t hz);
    static const char* GetProfileName(ClockProfile profile, bool pretty);
    static const char* GetModuleName(ClockModule module, bool pretty);
    static std::uint32_t GetNearestHz(ClockModule module, ClockProfile profile, std::uint32_t inHz);

  protected:
    static PcvModule GetPcvModule(ClockModule clockmodule);
    static PcvModuleId GetPcvModuleId(ClockModule clockmodule);
    static std::uint32_t GetNearestHz(ClockModule module, std::uint32_t inHz);
    static void GetList(ClockModule module, std::uint32_t **outClocks, size_t *outClockCount);
    static std::uint32_t GetMaxAllowedHz(ClockModule module, ClockProfile profile);
};
