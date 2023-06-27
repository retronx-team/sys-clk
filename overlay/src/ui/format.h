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

#include <cstdio>

#define FREQ_DEFAULT_TEXT "Do not override"

static inline std::string formatListFreqMHz(std::uint32_t mhz)
{
    if(mhz == 0)
    {
        return FREQ_DEFAULT_TEXT;
    }

    char buf[10];
    return std::string(buf, snprintf(buf, sizeof(buf), "%u MHz", mhz));
}

static inline std::string formatListFreqHz(std::uint32_t hz) { return formatListFreqMHz(hz / 1000000); }
