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

u32 t210ClkCpuFreq(void);
u32 t210ClkMemFreq(void);
u32 t210ClkGpuFreq(void);
u32 t210EmcLoadAll(void);
u32 t210EmcLoadCpu(void);

#ifdef __cplusplus
}
#endif
