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
    if (module == PcvModule_Cpu)
    {
        *outClocks = &g_cpu_clocks[0];
        *outClockCount = g_cpu_clock_count;
    }
    else if (module == PcvModule_Gpu)
    {
        *outClocks = &g_gpu_clocks[0];
        *outClockCount = g_gpu_clock_count;
    }
    else if (module == PcvModule_Emc)
    {
        *outClocks = &g_mem_clocks[0];
        *outClockCount = g_mem_clock_count;
    }
    else
    {
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
}

void Clocks::Exit()
{
    pcvExit();
    apmExit();
}

std::string Clocks::GetModeName(bool docked)
{
    if (docked)
    {
        return "docked";
    }
    else
    {
        return "handheld";
    }
}

std::string Clocks::GetModuleName(PcvModule module)
{
    if (module == PcvModule_Cpu)
    {
        return "cpu";
    }
    else if (module == PcvModule_Gpu)
    {
        return "gpu";
    }
    else if (module == PcvModule_Emc)
    {
        return "mem";
    }

    ERROR_THROW("No such PcvModule: %u", module);
    
    return "";
}

bool Clocks::IsConsoleDocked()
{
    std::uint32_t mode = 0;
    Result rc = apmExtGetPerformanceMode(&mode);
    ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

    return mode == 1;
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

std::uint32_t Clocks::GetNearestHz(PcvModule module, bool docked, std::uint32_t inHz)
{
    std::uint32_t hz = Clocks::GetNearestHz(module, inHz);

    if (!docked && module == PcvModule_Gpu && hz > g_gpu_handheld_max)
    {
        hz = g_gpu_handheld_max;
    }

    return hz;
}

std::uint32_t Clocks::GetNearestHz(PcvModule module, std::uint32_t inHz)
{
    std::uint32_t *clocks = NULL;
    size_t clockCount = 0;
    GetList(module, &clocks, &clockCount);

    if (clockCount == 0)
    {
        ERROR_THROW("clockCount = 0 for PcvModule: %u", module);
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
