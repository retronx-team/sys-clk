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

#include "types.h"
#include "../config.h"
#include "../board.h"
#include "../ipc.h"

bool sysclkIpcRunning();
Result sysclkIpcInitialize(void);
void sysclkIpcExit(void);

Result sysclkIpcGetAPIVersion(u32* out_ver);
Result sysclkIpcGetVersionString(char* out, size_t len);
Result sysclkIpcGetCurrentContext(SysClkContext* out_context);
Result sysclkIpcGetProfileCount(u64 tid, u8* out_count);
Result sysclkIpcSetEnabled(bool enabled);
Result sysclkIpcExitCmd();
Result sysclkIpcSetOverride(SysClkModule module, u32 hz);
Result sysclkIpcGetProfiles(u64 tid, SysClkTitleProfileList* out_profiles);
Result sysclkIpcSetProfiles(u64 tid, SysClkTitleProfileList* profiles);
Result sysclkIpcGetConfigValues(SysClkConfigValueList* out_configValues);
Result sysclkIpcSetConfigValues(SysClkConfigValueList* configValues);
Result sysclkIpcGetFreqList(SysClkModule module, u32* list, u32 maxCount, u32* outCount);

static inline Result sysclkIpcRemoveOverride(SysClkModule module)
{
    return sysclkIpcSetOverride(module, 0);
}
