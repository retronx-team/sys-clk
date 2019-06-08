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
#include "nx/ipc/apm_ext.h"

void Clocks::GetList(SysClkModule module, std::uint32_t **outClocks, size_t *outClockCount)
{
    switch(module)
    {
        case SysClkModule_CPU:
            *outClocks = &g_cpu_clocks[0];
            *outClockCount = g_cpu_clock_count;
            break;
        case SysClkModule_GPU:
            *outClocks = &g_gpu_clocks[0];
            *outClockCount = g_gpu_clock_count;
            break;
        case SysClkModule_MEM:
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

    if(hosversionAtLeast(8,0,0))
    {
        rc = clkrstInitialize();
        ASSERT_RESULT_OK(rc, "pcvInitialize");
    }
    else
    {
        rc = pcvInitialize();
        ASSERT_RESULT_OK(rc, "pcvInitialize");
    }

    rc = apmExtInitialize();
    ASSERT_RESULT_OK(rc, "apmExtInitialize");

    rc = psmInitialize();
    ASSERT_RESULT_OK(rc, "psmInitialize");
}

void Clocks::Exit()
{
    if(hosversionAtLeast(8,0,0))
    {
        pcvExit();
    }
    else
    {
        clkrstExit();
    }
    apmExtExit();
    psmExit();
}

const char* Clocks::GetModuleName(SysClkModule module, bool pretty)
{
    switch(module)
    {
        case SysClkModule_CPU:
            return pretty ? "CPU" : "cpu";
        case SysClkModule_GPU:
            return pretty ? "GPU" : "gpu";
        case SysClkModule_MEM:
            return pretty ? "Memory" : "mem";
        default:
            ERROR_THROW("No such PcvModule: %u", module);
    }

    return "";
}

const char* Clocks::GetProfileName(SysClkProfile profile, bool pretty)
{
    switch(profile)
    {
        case SysClkProfile_Docked:
            return pretty ? "Docked" : "docked";
        case SysClkProfile_Handheld:
            return pretty ? "Handheld" : "handheld";
        case SysClkProfile_HandheldCharging:
            return pretty ? "Handheld (Charging?)" : "handheld_charging";
        case SysClkProfile_HandheldChargingUSB:
            return pretty ? "Handheld (Charging: USB)" : "handheld_charging_usb";
        case SysClkProfile_HandheldChargingOfficial:
            return pretty ? "Handheld (Charging: Official)" : "handheld_charging_official";
        default:
            ERROR_THROW("No such SysClkProfile: %u", profile);
    }

    return "";
}

PcvModule Clocks::GetPcvModule(SysClkModule SysClkModule)
{
    switch(SysClkModule)
    {
        case SysClkModule_CPU:
            return PcvModule_CpuBus;
        case SysClkModule_GPU:
            return PcvModule_GPU;
        case SysClkModule_MEM:
            return PcvModule_EMC;
        default:
            ERROR_THROW("No such SysClkModule: %u", SysClkModule);
    }

    return (PcvModule)0;
}

PcvModuleId Clocks::GetPcvModuleId(SysClkModule SysClkModule)
{
    PcvModuleId pcvModuleId;
    Result rc = pcvGetModuleId(&pcvModuleId, GetPcvModule(SysClkModule));
    ASSERT_RESULT_OK(rc, "pcvGetModuleId");

    return pcvModuleId;
}

std::uint32_t Clocks::ResetToStock()
{
    std::uint32_t mode = 0;
    Result rc = apmExtGetPerformanceMode(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

    rc = apmExtSysRequestPerformanceMode(mode);
    ASSERT_RESULT_OK(rc, "apmExtSysRequestPerformanceMode");

    return mode;
}

SysClkProfile Clocks::GetCurrentProfile()
{
    std::uint32_t mode = 0;
    Result rc = apmExtGetPerformanceMode(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

    if(mode)
    {
        return SysClkProfile_Docked;
    }

    ChargerType chargerType;

    rc = psmGetChargerType(&chargerType);
    ASSERT_RESULT_OK(rc, "psmGetChargerType");

    if(chargerType == ChargerType_Charger)
    {
        return SysClkProfile_HandheldChargingOfficial;
    }
    else if(chargerType == ChargerType_Usb)
    {
        return SysClkProfile_HandheldChargingUSB;
    }

    return SysClkProfile_Handheld;
}

void Clocks::SetHz(SysClkModule module, std::uint32_t hz)
{
    Result rc = 0;

    if(hosversionAtLeast(8,0,0))
    {
        ClkrstSession session = {0};

        rc = clkrstOpenSession(&session, Clocks::GetPcvModuleId(module), 3);
        ASSERT_RESULT_OK(rc, "clkrstOpenSession");

        rc = clkrstSetClockRate(&session, hz);
        ASSERT_RESULT_OK(rc, "clkrstSetClockRate");

        clkrstCloseSession(&session);
    }
    else
    {
        rc = pcvSetClockRate(Clocks::GetPcvModule(module), hz);
        ASSERT_RESULT_OK(rc, "pcvSetClockRate");
    }
}

std::uint32_t Clocks::GetCurrentHz(SysClkModule module)
{
    Result rc = 0;
    std::uint32_t hz = 0;

    if(hosversionAtLeast(8,0,0))
    {
        ClkrstSession session = {0};

        rc = clkrstOpenSession(&session, Clocks::GetPcvModuleId(module), 3);
        ASSERT_RESULT_OK(rc, "clkrstOpenSession");

        rc = clkrstGetClockRate(&session, &hz);
        ASSERT_RESULT_OK(rc, "clkrstSetClockRate");

        clkrstCloseSession(&session);
    }
    else
    {
        rc = pcvGetClockRate(Clocks::GetPcvModule(module), &hz);
        ASSERT_RESULT_OK(rc, "pcvGetClockRate");
    }

    return hz;
}

std::uint32_t Clocks::GetNearestHz(SysClkModule module, SysClkProfile profile, std::uint32_t inHz)
{
    std::uint32_t hz = GetNearestHz(module, inHz);
    std::uint32_t maxHz = GetMaxAllowedHz(module, profile);

    if(maxHz != 0)
    {
        hz = std::min(hz, maxHz);
    }

    return hz;
}

std::uint32_t Clocks::GetMaxAllowedHz(SysClkModule module, SysClkProfile profile)
{
    if(module == SysClkModule_GPU)
    {
        if(profile < SysClkProfile_HandheldCharging)
        {
            return g_gpu_handheld_max;
        }
        else if(profile <= SysClkProfile_HandheldChargingUSB)
        {
            return g_gpu_unofficial_charger_max;
        }
    }

    return 0;
}

std::uint32_t Clocks::GetNearestHz(SysClkModule module, std::uint32_t inHz)
{
    std::uint32_t *clocks = NULL;
    size_t clockCount = 0;
    GetList(module, &clocks, &clockCount);

    if (!clockCount)
    {
        ERROR_THROW("clockCount == 0 for SysClkModule: %u", module);
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
