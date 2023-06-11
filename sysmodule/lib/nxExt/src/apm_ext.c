/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "nxExt/apm_ext.h"

#include <stdatomic.h>

static Service g_apmSrv;
static Service g_apmSysSrv;
static atomic_size_t g_refCnt;

Result apmExtInitialize(void)
{
    g_refCnt++;

    if (serviceIsActive(&g_apmSrv))
    {
        return 0;
    }

    Result rc = 0;

    rc = smGetService(&g_apmSrv, "apm");
    if(R_SUCCEEDED(rc))
    {
        rc = smGetService(&g_apmSysSrv, "apm:sys");
    }

    if (R_FAILED(rc))
    {
        apmExtExit();
    }

    return rc;
}

void apmExtExit(void)
{
    if (--g_refCnt == 0)
    {
        serviceClose(&g_apmSrv);
        serviceClose(&g_apmSysSrv);
    }
}

Result apmExtGetPerformanceMode(u32* out_mode)
{
    return serviceDispatchOut(&g_apmSrv, 1, *out_mode);
}

Result apmExtSysRequestPerformanceMode(u32 mode)
{
    return serviceDispatchIn(&g_apmSysSrv, 0, mode);
}

Result apmExtGetCurrentPerformanceConfiguration(u32* out_conf)
{
    return serviceDispatchOut(&g_apmSysSrv, 7, *out_conf);
}
