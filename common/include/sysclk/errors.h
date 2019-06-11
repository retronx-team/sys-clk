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

#include <switch/result.h>

#define SYSCLK_ERROR_MODULE 388
#define SYSCLK_ERROR(desc) MAKERESULT(SYSCLK_ERROR_MODULE, SysClkError_##desc)

typedef enum
{
    SysClkError_Generic = 0,
    SysClkError_ConfigNotLoaded = 1,
    SysClkError_ConfigSaveFailed = 2,
} SysClkError;
