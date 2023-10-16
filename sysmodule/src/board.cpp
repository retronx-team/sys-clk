/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include <nxExt.h>
#include "board.h"
#include "errors.h"

#define HOSSVC_HAS_CLKRST (hosversionAtLeast(8,0,0))
#define HOSSVC_HAS_TC (hosversionAtLeast(5,0,0))

static SysClkSocType g_socType = SysClkSocType_Erista;

const char* Board::GetModuleName(SysClkModule module, bool pretty)
{
    ASSERT_ENUM_VALID(SysClkModule, module);
    return sysclkFormatModule(module, pretty);
}

const char* Board::GetProfileName(SysClkProfile profile, bool pretty)
{
    ASSERT_ENUM_VALID(SysClkProfile, profile);
    return sysclkFormatProfile(profile, pretty);
}

const char* Board::GetThermalSensorName(SysClkThermalSensor sensor, bool pretty)
{
    ASSERT_ENUM_VALID(SysClkThermalSensor, sensor);
    return sysclkFormatThermalSensor(sensor, pretty);
}

const char* Board::GetPowerSensorName(SysClkPowerSensor sensor, bool pretty)
{
    ASSERT_ENUM_VALID(SysClkPowerSensor, sensor);
    return sysclkFormatPowerSensor(sensor, pretty);
}

PcvModule Board::GetPcvModule(SysClkModule sysclkModule)
{
    switch(sysclkModule)
    {
        case SysClkModule_CPU:
            return PcvModule_CpuBus;
        case SysClkModule_GPU:
            return PcvModule_GPU;
        case SysClkModule_MEM:
            return PcvModule_EMC;
        default:
            ASSERT_ENUM_VALID(SysClkModule, sysclkModule);
    }

    return (PcvModule)0;
}

PcvModuleId Board::GetPcvModuleId(SysClkModule sysclkModule)
{
    PcvModuleId pcvModuleId;
    Result rc = pcvGetModuleId(&pcvModuleId, GetPcvModule(sysclkModule));
    ASSERT_RESULT_OK(rc, "pcvGetModuleId");

    return pcvModuleId;
}

void Board::Initialize()
{
    Result rc = 0;

    if(HOSSVC_HAS_CLKRST)
    {
        rc = clkrstInitialize();
        ASSERT_RESULT_OK(rc, "clkrstInitialize");
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

    rc = tsInitialize();
    ASSERT_RESULT_OK(rc, "tsInitialize");

    if(HOSSVC_HAS_TC)
    {
        rc = tcInitialize();
        ASSERT_RESULT_OK(rc, "tcInitialize");
    }

    rc = max17050Initialize();
    ASSERT_RESULT_OK(rc, "max17050Initialize");

    FetchHardwareInfos();
}

void Board::Exit()
{
    if(HOSSVC_HAS_CLKRST)
    {
        clkrstExit();
    }
    else
    {
        pcvExit();
    }

    apmExtExit();
    psmExit();
    tsExit();

    if(HOSSVC_HAS_TC)
    {
        tcExit();
    }

    max17050Exit();
}

SysClkProfile Board::GetProfile()
{
    std::uint32_t mode = 0;
    Result rc = apmExtGetPerformanceMode(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

    if(mode)
    {
        return SysClkProfile_Docked;
    }

    PsmChargerType chargerType;

    rc = psmGetChargerType(&chargerType);
    ASSERT_RESULT_OK(rc, "psmGetChargerType");

    if(chargerType == PsmChargerType_EnoughPower)
    {
        return SysClkProfile_HandheldChargingOfficial;
    }
    else if(chargerType == PsmChargerType_LowPower)
    {
        return SysClkProfile_HandheldChargingUSB;
    }

    return SysClkProfile_Handheld;
}

void Board::SetHz(SysClkModule module, std::uint32_t hz)
{
    Result rc = 0;

    if(HOSSVC_HAS_CLKRST)
    {
        ClkrstSession session = {0};

        rc = clkrstOpenSession(&session, Board::GetPcvModuleId(module), 3);
        ASSERT_RESULT_OK(rc, "clkrstOpenSession");

        rc = clkrstSetClockRate(&session, hz);
        ASSERT_RESULT_OK(rc, "clkrstSetClockRate");

        clkrstCloseSession(&session);
    }
    else
    {
        rc = pcvSetClockRate(Board::GetPcvModule(module), hz);
        ASSERT_RESULT_OK(rc, "pcvSetClockRate");
    }
}

std::uint32_t Board::GetHz(SysClkModule module)
{
    Result rc = 0;
    std::uint32_t hz = 0;

    if(HOSSVC_HAS_CLKRST)
    {
        ClkrstSession session = {0};

        rc = clkrstOpenSession(&session, Board::GetPcvModuleId(module), 3);
        ASSERT_RESULT_OK(rc, "clkrstOpenSession");

        rc = clkrstGetClockRate(&session, &hz);
        ASSERT_RESULT_OK(rc, "clkrstSetClockRate");

        clkrstCloseSession(&session);
    }
    else
    {
        rc = pcvGetClockRate(Board::GetPcvModule(module), &hz);
        ASSERT_RESULT_OK(rc, "pcvGetClockRate");
    }

    return hz;
}

std::uint32_t Board::GetRealHz(SysClkModule module)
{
    switch(module)
    {
        case SysClkModule_CPU:
            return t210ClkCpuFreq();
        case SysClkModule_GPU:
            return t210ClkGpuFreq();
        case SysClkModule_MEM:
            return t210ClkMemFreq();
        default:
            ASSERT_ENUM_VALID(SysClkModule, module);
    }

    return 0;
}

void Board::GetFreqList(SysClkModule module, std::uint32_t* outList, std::uint32_t maxCount, std::uint32_t* outCount)
{
    Result rc = 0;
    PcvExtClockRatesListType type;
    s32 tmpInMaxCount = maxCount;
    s32 tmpOutCount = 0;

    if(HOSSVC_HAS_CLKRST)
    {
        ClkrstSession session = {0};

        rc = clkrstOpenSession(&session, Board::GetPcvModuleId(module), 3);
        ASSERT_RESULT_OK(rc, "clkrstOpenSession");

        rc = clkrstExtGetPossibleClockRates(&session, outList, tmpInMaxCount, &type, &tmpOutCount);
        ASSERT_RESULT_OK(rc, "clkrstGetPossibleClockRates");

        clkrstCloseSession(&session);
    }
    else
    {
        rc = pcvExtGetPossibleClockRates(Board::GetPcvModule(module), outList, tmpInMaxCount, &type, &tmpOutCount);
        ASSERT_RESULT_OK(rc, "pcvGetPossibleClockRates");
    }

    if(type != PcvExtClockRatesListType_Discrete)
    {
        ERROR_THROW("Unexpected PcvClockRatesListType: %u (module = %s)", type, Board::GetModuleName(module, false));
    }

    *outCount = tmpOutCount;
}

void Board::ResetToStock()
{
    Result rc = 0;
    if(hosversionAtLeast(9,0,0))
    {
        std::uint32_t confId = 0;
        rc = apmExtGetCurrentPerformanceConfiguration(&confId);
        ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");

        SysClkApmConfiguration* apmConfiguration = NULL;
        for(size_t i = 0; sysclk_g_apm_configurations[i].id; i++)
        {
            if(sysclk_g_apm_configurations[i].id == confId)
            {
                apmConfiguration = &sysclk_g_apm_configurations[i];
                break;
            }
        }

        if(!apmConfiguration)
        {
            ERROR_THROW("Unknown apm configuration: %x", confId);
        }

        Board::SetHz(SysClkModule_CPU, apmConfiguration->cpu_hz);
        Board::SetHz(SysClkModule_GPU, apmConfiguration->gpu_hz);
        Board::SetHz(SysClkModule_MEM, apmConfiguration->mem_hz);
    }
    else
    {
        std::uint32_t mode = 0;
        rc = apmExtGetPerformanceMode(&mode);
        ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

        rc = apmExtSysRequestPerformanceMode(mode);
        ASSERT_RESULT_OK(rc, "apmExtSysRequestPerformanceMode");
    }
}

std::int32_t Board::GetTsTemperatureMilli(TsLocation location)
{
    Result rc;
    std::int32_t millis = 0;

    if(hosversionAtLeast(17,0,0))
    {
        TsExtSession session = {0};
        float temp = 0;

        rc = tsExtOpenSession(&session, location);
        ASSERT_RESULT_OK(rc, "tsExtOpenSession(%u)", location);

        rc = tsExtSessionGetTemperature(&session, &temp);
        ASSERT_RESULT_OK(rc, "tsExtSessionGetTemperature(%u)", location);
        millis = temp * 1000;

        tsExtCloseSession(&session);
    }
    else if(hosversionAtLeast(14,0,0))
    {
        rc = tsGetTemperature(location, &millis);
        ASSERT_RESULT_OK(rc, "tsGetTemperature(%u)", location);
        millis *= 1000;
    }
    else
    {
        rc = tsGetTemperatureMilliC(location, &millis);
        ASSERT_RESULT_OK(rc, "tsGetTemperatureMilliC(%u)", location);
    }

    return millis;
}

std::uint32_t Board::GetTemperatureMilli(SysClkThermalSensor sensor)
{
    std::int32_t millis = 0;

    if(sensor == SysClkThermalSensor_SOC)
    {
        millis = GetTsTemperatureMilli(TsLocation_External);
    }
    else if(sensor == SysClkThermalSensor_PCB)
    {
        millis = GetTsTemperatureMilli(TsLocation_Internal);
    }
    else if(sensor == SysClkThermalSensor_Skin)
    {
        if(HOSSVC_HAS_TC)
        {
            Result rc;
            rc = tcGetSkinTemperatureMilliC(&millis);
            ASSERT_RESULT_OK(rc, "tcGetSkinTemperatureMilliC");
        }
    }
    else
    {
        ASSERT_ENUM_VALID(SysClkThermalSensor, sensor);
    }

    return std::max(0, millis);
}

std::int32_t Board::GetPowerMw(SysClkPowerSensor sensor)
{
    switch(sensor)
    {
        case SysClkPowerSensor_Now:
            return max17050PowerNow();
        case SysClkPowerSensor_Avg:
            return max17050PowerAvg();
        default:
            ASSERT_ENUM_VALID(SysClkPowerSensor, sensor);
    }

    return 0;
}

SysClkSocType Board::GetSocType() {
    return g_socType;
}

void Board::FetchHardwareInfos()
{
    u64 sku = 0;
    Result rc = splInitialize();
    ASSERT_RESULT_OK(rc, "splInitialize");

    rc = splGetConfig(SplConfigItem_HardwareType, &sku);
    ASSERT_RESULT_OK(rc, "splGetConfig");

    splExit();

    switch(sku)
    {
        case 2 ... 5:
            g_socType = SysClkSocType_Mariko;
            break;
        default:
            g_socType = SysClkSocType_Erista;
    }
}