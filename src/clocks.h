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

class Clocks
{
  public:
    static void Exit();
    static void Initialize();
    static bool IsConsoleDocked();
    static ChargerType GetConsoleChargerType();
    static std::string GetModeName(bool docked);
    static std::string GetModuleName(PcvModule module);
    static std::uint32_t GetCurrentHz(PcvModule module);
    static void SetHz(PcvModule module, std::uint32_t hz);
    static std::string GetChargerTypeName(ChargerType chargerType);
    static std::uint32_t GetNearestHz(PcvModule module, bool docked, ChargerType chargerType, std::uint32_t inHz);

  protected:
    static std::uint32_t GetNearestHz(PcvModule module, std::uint32_t inHz);
    static void GetList(PcvModule module, std::uint32_t **outClocks, size_t *outClockCount);
    static std::uint32_t GetMaxAllowedHz(PcvModule module, bool docked, ChargerType chargerType);
};
