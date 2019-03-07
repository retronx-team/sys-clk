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

    rc = apmInitialize();
    ASSERT_RESULT_OK(rc, "apmInitialize");

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

std::string Clocks::GetModuleName(PcvModule module, bool pretty)
{
    switch(module)
    {
        case PcvModule_Cpu:
            return pretty ? "CPU" : "cpu";
        case PcvModule_Gpu:
            return pretty ? "GPU" : "gpu";
        case PcvModule_Emc:
            return pretty ? "Memory" : "mem";
        default:
            ERROR_THROW("No such PcvModule: %u", module);
    }

    return "";
}

std::string Clocks::GetProfileName(ClockProfile profile, bool pretty)
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

void Clocks::ResetToStock() {
    std::uint32_t mode = 0;
    Result rc = apmExtGetPerformanceMode(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

    std::uint32_t conf = 0;
    rc = apmGetPerformanceConfiguration(mode, &conf);
    ASSERT_RESULT_OK(rc, "apmGetPerformanceConfiguration");

    rc = apmSetPerformanceConfiguration(mode, conf);
    ASSERT_RESULT_OK(rc, "apmSetPerformanceConfiguration");
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

std::uint32_t Clocks::GetNearestHz(PcvModule module, ClockProfile profile, std::uint32_t inHz)
{
    std::uint32_t hz = GetNearestHz(module, inHz);
    std::uint32_t maxHz = GetMaxAllowedHz(module, profile);

    if(maxHz != 0)
    {
        hz = std::min(hz, maxHz);
    }

    return hz;
}

std::uint32_t Clocks::GetMaxAllowedHz(PcvModule module, ClockProfile profile)
{
    if(module == PcvModule_Gpu)
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
