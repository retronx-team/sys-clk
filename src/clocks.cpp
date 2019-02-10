/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "clocks.h"
#include "clock_table.h"
#include "errors.h"
#include "ipc/apm_ext.h"

void Clocks::GetList(PcvModule module, std::uint32_t **outClocks, size_t *outClockCount)
{
    switch(module)
    {
        case PcvModule_Cpu:
            *outClocks = &g_cpu_clocks[0];
            *outClockCount = g_cpu_clock_count;
            break;
        case PcvModule_Gpu:
            *outClocks = &g_gpu_clocks[0];
            *outClockCount = g_gpu_clock_count;
            break;
        case PcvModule_Emc:
            *outClocks = &g_mem_clocks[0];
            *outClockCount = g_mem_clock_count;
            break;
        default:
            ERROR_THROW("No such PcvModule: %u", module);
    }
}

void Clocks::Initialize()
{
    Result rc = 0;

    rc = pcvInitialize();
    ASSERT_RESULT_OK(rc, "pcvInitialize");

    rc = apmExtInitialize();
    ASSERT_RESULT_OK(rc, "apmExtInitialize");

    rc = psmInitialize();
    ASSERT_RESULT_OK(rc, "psmInitialize");
}

void Clocks::Exit()
{
    pcvExit();
    apmExit();
    psmExit();
}

std::string Clocks::GetModeName(bool docked)
{
    return docked ? "docked" : "handheld";
}

std::string Clocks::GetModuleName(PcvModule module)
{
    switch(module)
    {
        case PcvModule_Cpu:
            return "cpu";
        case PcvModule_Gpu:
            return "gpu";
        case PcvModule_Emc:
            return "mem";
        default:
            ERROR_THROW("No such PcvModule: %u", module);
    }
    
    return "";
}

std::string Clocks::GetChargerTypeName(ChargerType chargerType)
{
    switch(chargerType)
    {
        case ChargerType_Charger: // Official Charger
            return "Official";
        case ChargerType_Usb:
            return "Unofficial";
        default:
            return "None";
    }
}

bool Clocks::IsConsoleDocked()
{
    std::uint32_t mode = 0;
    Result rc = apmExtGetPerformanceMode(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

    return mode ? true : false;
}

ChargerType Clocks::GetConsoleChargerType()
{
    Result rc = 0;
    ChargerType chargerType;

    rc = psmGetChargerType(&chargerType);
    ASSERT_RESULT_OK(rc, "psmGetChargerType");

    return chargerType;
}

void Clocks::SetHz(PcvModule module, std::uint32_t hz)
{
    Result rc = pcvSetClockRate(module, hz);
    ASSERT_RESULT_OK(rc, "pcvSetClockRate");
}

std::uint32_t Clocks::GetCurrentHz(PcvModule module)
{
    std::uint32_t hz = 0;
    Result rc = pcvGetClockRate(module, &hz);
    ASSERT_RESULT_OK(rc, "pcvGetClockRate");

    return hz;
}

std::uint32_t Clocks::GetNearestHz(PcvModule module, bool docked, ChargerType chargerType, std::uint32_t inHz)
{
    std::uint32_t hz = GetNearestHz(module, inHz);
    std::uint32_t maxHz = GetMaxAllowedHz(module, docked, chargerType);

    if(maxHz != 0) {
        hz = std::min(hz, maxHz);
    }

    return hz;
}

std::uint32_t Clocks::GetMaxAllowedHz(PcvModule module, bool docked, ChargerType chargerType)
{
    if(module == PcvModule_Gpu) {
        if(chargerType == ChargerType_None) {
            return g_gpu_handheld_max;
        } else if(chargerType == ChargerType_Usb) {
            return g_gpu_unofficial_charger_max;
        }
    }

    return 0;
}

std::uint32_t Clocks::GetNearestHz(PcvModule module, std::uint32_t inHz)
{
    std::uint32_t *clocks = NULL;
    size_t clockCount = 0;
    GetList(module, &clocks, &clockCount);

    if (!clockCount)
    {
        ERROR_THROW("clockCount == 0 for PcvModule: %u", module);
    }

    for (int i = clockCount - 1; i > 0; i--)
    {
        if (inHz <= (clocks[i] + clocks[i - 1]) / 2)
        {
            return clocks[i];
        }
    }

    return clocks[0];
}
