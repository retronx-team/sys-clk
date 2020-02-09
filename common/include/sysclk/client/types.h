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

#ifdef __SWITCH__

#include <switch/types.h>
#include <switch/result.h>

#else

#define R_FAILED(res) ((res) != 0)
#define R_SUCCEEDED(res) ((res) == 0)

typedef std::uint32_t Result;
typedef std::uint32_t u32;
typedef std::int32_t s32;
typedef std::uint64_t u64;
typedef std::uint8_t u8;

#endif
