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

void Clocks::GetList(ClockModule module, std::uint32_t **outClocks, size_t *outClockCount)
{
    switch(module)
    {
        case ClockModule_CPU:
            *outClocks = &g_cpu_clocks[0];
            *outClockCount = g_cpu_clock_count;
            break;
        case ClockModule_GPU:
            *outClocks = &g_gpu_clocks[0];
            *outClockCount = g_gpu_clock_count;
            break;
        case ClockModule_MEM:
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
    apmExtExit();
    psmExit();
}

const char* Clocks::GetModuleName(ClockModule module, bool pretty)
{
    switch(module)
    {
        case ClockModule_CPU:
            return pretty ? "CPU" : "cpu";
        case ClockModule_GPU:
            return pretty ? "GPU" : "gpu";
        case ClockModule_MEM:
            return pretty ? "Memory" : "mem";
        default:
            ERROR_THROW("No such PcvModule: %u", module);
    }

    return "";
}

const char* Clocks::GetProfileName(ClockProfile profile, bool pretty)
{
    switch(profile)
    {
        case ClockProfile_Docked:
            return pretty ? "Docked" : "docked";
        case ClockProfile_Handheld:
            return pretty ? "Handheld" : "handheld";
        case ClockProfile_HandheldCharging:
            return pretty ? "Handheld (Charging?)" : "handheld_charging";
        case ClockProfile_HandheldChargingUSB:
            return pretty ? "Handheld (Charging: USB)" : "handheld_charging_usb";
        case ClockProfile_HandheldChargingOfficial:
            return pretty ? "Handheld (Charging: Official)" : "handheld_charging_official";
        default:
            ERROR_THROW("No such ClockProfile: %u", profile);
    }

    return "";
}

PcvModule Clocks::GetPcvModule(ClockModule clockmodule) {
    switch(clockmodule)
    {
        case ClockModule_CPU:
            return PcvModule_Cpu;
        case ClockModule_GPU:
            return PcvModule_Gpu;
        case ClockModule_MEM:
            return PcvModule_Emc;
        default:
            ERROR_THROW("No such ClockModule: %u", clockmodule);
    }

    return (PcvModule)0;
}

std::uint32_t Clocks::ResetToStock() {
    std::uint32_t mode = 0;
    Result rc = apmExtGetPerformanceMode(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

    rc = apmExtSysRequestPerformanceMode(mode);
    ASSERT_RESULT_OK(rc, "apmExtSysRequestPerformanceMode");

    return mode;
}

ClockProfile Clocks::GetCurrentProfile()
{
    std::uint32_t mode = 0;
    Result rc = apmExtGetPerformanceMode(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

    if(mode) {
        return ClockProfile_Docked;
    }

    ChargerType chargerType;

    rc = psmGetChargerType(&chargerType);
    ASSERT_RESULT_OK(rc, "psmGetChargerType");

    if(chargerType == ChargerType_Charger)
    {
        return ClockProfile_HandheldChargingOfficial;
    }
    else if(chargerType == ChargerType_Usb)
    {
        return ClockProfile_HandheldChargingUSB;
    }

    return ClockProfile_Handheld;
}

void Clocks::SetHz(ClockModule module, std::uint32_t hz)
{
    Result rc = pcvSetClockRate(Clocks::GetPcvModule(module), hz);
    ASSERT_RESULT_OK(rc, "pcvSetClockRate");
}

std::uint32_t Clocks::GetCurrentHz(ClockModule module)
{
    std::uint32_t hz = 0;
    Result rc = pcvGetClockRate(Clocks::GetPcvModule(module), &hz);
    ASSERT_RESULT_OK(rc, "pcvGetClockRate");

    return hz;
}

std::uint32_t Clocks::GetNearestHz(ClockModule module, ClockProfile profile, std::uint32_t inHz)
{
    std::uint32_t hz = GetNearestHz(module, inHz);
    std::uint32_t maxHz = GetMaxAllowedHz(module, profile);

    if(maxHz != 0)
    {
        hz = std::min(hz, maxHz);
    }

    return hz;
}

std::uint32_t Clocks::GetMaxAllowedHz(ClockModule module, ClockProfile profile)
{
    if(module == ClockModule_CPU)
    {
        if(profile < ClockProfile_HandheldCharging)
        {
            return g_gpu_handheld_max;
        }
        else if(profile <= ClockProfile_HandheldChargingUSB)
        {
            return g_gpu_unofficial_charger_max;
        }
    }

    return 0;
}

std::uint32_t Clocks::GetNearestHz(ClockModule module, std::uint32_t inHz)
{
    std::uint32_t *clocks = NULL;
    size_t clockCount = 0;
    GetList(module, &clocks, &clockCount);

    if (!clockCount)
    {
        ERROR_THROW("clockCount == 0 for ClockModule: %u", module);
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
