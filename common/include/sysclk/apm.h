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

#include "clocks.h"

typedef struct {
    uint32_t id;
    uint32_t cpu_hz;
    uint32_t gpu_hz;
    uint32_t mem_hz;
} SysClkApmConfiguration;

extern SysClkApmConfiguration sysclk_g_apm_configurations[];