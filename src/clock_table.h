/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

static std::uint32_t g_mem_clocks[] = {
    1600000000,
    1331200000,
    1065600000,
    800000000,
    665600000,
};

static size_t g_mem_clock_count = sizeof(g_mem_clocks) / sizeof(g_mem_clocks[0]);

static std::uint32_t g_cpu_clocks[] = {
    //1785000000,
    1683000000,
    1581000000,
    1428000000,
    1326000000,
    1224000000,
    1122000000,
    1020000000,
    918000000,
    816000000,
    714000000,
    612000000,
};

static size_t g_cpu_clock_count = sizeof(g_cpu_clocks) / sizeof(g_cpu_clocks[0]);

static std::uint32_t g_gpu_clocks[] = {
    921600000,
    844800000,
    768000000,
    691200000,
    614400000,
    537600000,
    460800000,
    384000000,
    307200000,
    230400000,
    153600000,
    76800000,
};

static std::uint32_t g_gpu_handheld_max = 460800000;

static size_t g_gpu_clock_count = sizeof(g_gpu_clocks) / sizeof(g_gpu_clocks[0]);
