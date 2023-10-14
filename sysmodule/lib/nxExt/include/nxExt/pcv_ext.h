/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <switch.h>

typedef enum {
    PcvExtClockRatesListType_Invalid  = 0,
    PcvExtClockRatesListType_Discrete = 1,
    PcvExtClockRatesListType_Range    = 2,
} PcvExtClockRatesListType;

Result pcvExtGetPossibleClockRates(PcvModule module, u32 *rates, s32 max_count, PcvExtClockRatesListType *out_type, s32 *out_count);
Result clkrstExtGetPossibleClockRates(ClkrstSession *session, u32 *rates, s32 max_count, PcvExtClockRatesListType *out_type, s32 *out_count);

#ifdef __cplusplus
}
#endif