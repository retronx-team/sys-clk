/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include <stdint.h>

uint32_t sysclk_g_freq_table_mem_hz[] = {
    665600000,
    800000000,
    1065600000,
    1331200000,
    1600000000,
    0,
};

uint32_t sysclk_g_freq_table_cpu_hz[] = {
    612000000,
    714000000,
    816000000,
    918000000,
    1020000000,
    1122000000,
    1224000000,
    1326000000,
    1428000000,
    1581000000,
    1683000000,
    1785000000,
    0,
};

uint32_t sysclk_g_freq_table_gpu_hz[] = {
    76800000,
    153600000,
    230400000,
    307200000,
    384000000,
    460800000,
    537600000,
    614400000,
    691200000,
    768000000,
    844800000,
    921600000,
    0,
};
