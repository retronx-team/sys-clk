/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "apm_ext.h"

static Service g_apmSrv;
static u64 g_refCnt;

Result apmExtInitialize(void)
{
    atomicIncrement64(&g_refCnt);

    if (serviceIsActive(&g_apmSrv))
    {
        return 0;
    }

    Result rc = 0;

    rc = smGetService(&g_apmSrv, "apm");

    if (R_FAILED(rc))
    {
        apmExtExit();
    }

    return rc;
}

void apmExtExit(void)
{
    if (atomicDecrement64(&g_refCnt) == 0)
    {
        serviceClose(&g_apmSrv);
    }
}

Result apmExtGetPerformanceMode(u32 *out_mode)
{
    IpcCommand c;
    ipcInitialize(&c);

    struct
    {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 1;

    Result rc = serviceIpcDispatch(&g_apmSrv);

    if (R_SUCCEEDED(rc))
    {
        IpcParsedCommand r;
        ipcParse(&r);

        struct
        {
            u64 magic;
            u64 result;
            u32 mode;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc))
        {
            *out_mode = resp->mode;
        }
    }

    return rc;
}
