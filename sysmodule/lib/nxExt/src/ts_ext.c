/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "nxExt/ts_ext.h"

Result tsExtOpenSession(TsExtSession *out, TsLocation dev) {
    const u32 in = ((u32)dev + 1) | 0x41000000;
    return serviceDispatchIn(tsGetServiceSession(), 4, in,
        .out_num_objects = 1,
        .out_objects = &out->s,
    );
}


void tsExtCloseSession(TsExtSession *session) {
    serviceClose(&session->s);
}

Result tsExtSessionGetTemperature(TsExtSession *session, float *temperature) {
    return serviceDispatchOut(&session->s, 4, *temperature);
}
