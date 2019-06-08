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

#define SYSCLK_IPC_API_VERSION 0
#define SYSCLK_IPC_SERVICE_NAME "sys:clk"

enum SysClkIpcCmd
{
    SysClkIpcCmd_GetApiVersion = 0,
    SysClkIpcCmd_GetVersionString = 1,
    SysClkIpcCmd_GetCurrentContext = 2,
    SysClkIpcCmd_Exit = 3,
};