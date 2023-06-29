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

#include <stdint.h>
#include "board.h"
#include "clock_manager.h"

#define SYSCLK_IPC_API_VERSION 3
#define SYSCLK_IPC_SERVICE_NAME "sys:clk"

enum SysClkIpcCmd
{
    SysClkIpcCmd_GetApiVersion = 0,
    SysClkIpcCmd_GetVersionString = 1,
    SysClkIpcCmd_GetCurrentContext = 2,
    SysClkIpcCmd_Exit = 3,
    SysClkIpcCmd_GetProfileCount = 4,
    SysClkIpcCmd_GetProfiles = 5,
    SysClkIpcCmd_SetProfiles = 6,
    SysClkIpcCmd_SetEnabled = 7,
    SysClkIpcCmd_SetOverride = 8,
    SysClkIpcCmd_GetConfigValues = 9,
    SysClkIpcCmd_SetConfigValues = 10,
    SysClkIpcCmd_GetFreqList = 11,
};


typedef struct
{
    uint64_t tid;
    SysClkTitleProfileList profiles;
} SysClkIpc_SetProfiles_Args;

typedef struct
{
    SysClkModule module;
    uint32_t hz;
} SysClkIpc_SetOverride_Args;

typedef struct
{
    SysClkModule module;
    uint32_t maxCount;
} SysClkIpc_GetFreqList_Args;
