/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "nxExt/ts.h"

static Service g_tsSrv;
static u64 g_refCnt;

Result tsInitialize(void)
{
    Result rc = 0;

    atomicIncrement64(&g_refCnt);

    if (serviceIsActive(&g_tsSrv))
    {
        return 0;
    }

    rc = smGetService(&g_tsSrv, "ts");

    if (R_FAILED(rc))
    {
        tsExit();
    }

    return rc;
}

void tsExit(void)
{
    if (atomicDecrement64(&g_refCnt) == 0)
    {
        serviceClose(&g_tsSrv);
    }
}

Result tsGetPcbThermInt(u32 *out_therm)
{
    IpcCommand c;
    ipcInitialize(&c);

    struct
    {
        u64 magic;
        u64 cmd_id;
        u8 unk;
    } *raw;

    raw = serviceIpcPrepareHeader(&g_tsSrv, &c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 1;
    raw->unk = 1;

    Result rc = serviceIpcDispatch(&g_tsSrv);

    if (R_SUCCEEDED(rc))
    {
        IpcParsedCommand r;
        struct
        {
            u64 magic;
            u64 result;
            u32 therm;
        } *resp;

        serviceIpcParse(&g_tsSrv, &r, sizeof(*resp));
        resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc))
        {
            *out_therm = resp->therm;
        }
    }

    return rc;
}


Result tsGetSocThermMilli(u32 *out_therm)
{
    IpcCommand c;
    ipcInitialize(&c);

    struct
    {
        u64 magic;
        u64 cmd_id;
        u8 unk;
    } *raw;

    raw = serviceIpcPrepareHeader(&g_tsSrv, &c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 3;
    raw->unk = 1;

    Result rc = serviceIpcDispatch(&g_tsSrv);

    if (R_SUCCEEDED(rc))
    {
        IpcParsedCommand r;
        struct
        {
            u64 magic;
            u64 result;
            u32 therm;
        } *resp;

        serviceIpcParse(&g_tsSrv, &r, sizeof(*resp));
        resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc))
        {
            *out_therm = resp->therm;
        }
    }

    return rc;
}
