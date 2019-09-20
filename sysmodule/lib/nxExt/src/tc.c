/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "nxExt/tc.h"

static Service g_tcSrv;
static u64 g_refCnt;

Result tcInitialize(void)
{
    atomicIncrement64(&g_refCnt);

    if (serviceIsActive(&g_tcSrv))
    {
        return 0;
    }

    Result rc = smGetService(&g_tcSrv, "tc");

    if (R_FAILED(rc))
    {
        tcExit();
    }

    return rc;
}

void tcExit(void)
{
    if (atomicDecrement64(&g_refCnt) == 0)
    {
        serviceClose(&g_tcSrv);
    }
}

Result tcGetSkinTemperatureMilliC(s32 *out_millis)
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
    raw->cmd_id = 9;

    Result rc = serviceIpcDispatch(&g_tcSrv);

    if (R_SUCCEEDED(rc))
    {
        IpcParsedCommand r;
        ipcParse(&r);

        struct
        {
            u64 magic;
            u64 result;
            s32 millis;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc))
        {
            *out_millis = resp->millis;
        }
    }

    return rc;
}

