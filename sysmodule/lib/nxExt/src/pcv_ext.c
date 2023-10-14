/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "nxExt/pcv_ext.h"

Result pcvExtGetPossibleClockRates(PcvModule module, u32 *rates, s32 max_count, PcvExtClockRatesListType *out_type, s32 *out_count)
{
    if(hosversionAtLeast(8,0,0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        PcvModule module;
        s32 max_count;
    } in = { module, max_count };

    struct {
        s32 type;
        s32 count;
    } out;

    Result rc = serviceDispatchInOut(pcvGetServiceSession(), 5, in, out,
        .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcPointer, },
        .buffers = { { rates, max_count * sizeof(u32) }, }
    );

    if (R_SUCCEEDED(rc) && out_type) *out_type = out.type;
    if (R_SUCCEEDED(rc) && out_count) *out_count = out.count;

    return rc;
}

Result clkrstExtGetPossibleClockRates(ClkrstSession *session, u32 *rates, s32 max_count, PcvExtClockRatesListType *out_type, s32 *out_count) {
    struct {
        s32 type;
        s32 count;
    } out;

    Result rc = serviceDispatchInOut(&session->s, 10, max_count, out,
        .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcAutoSelect, },
        .buffers = { { rates, max_count * sizeof(u32) }, }
    );

    if (R_SUCCEEDED(rc) && out_type) *out_type = out.type;
    if (R_SUCCEEDED(rc) && out_count) *out_count = out.count;

    return rc;
}
