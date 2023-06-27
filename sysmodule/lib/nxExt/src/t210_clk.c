/*
 * Copyright (c) 2018 naehrwert
 * Copyright (c) 2018-2022 CTCaer
 * Copyright (c) 2022 p-sam
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nxExt/t210_clk.h"

#define WAIT_NS 1000000000UL

#define usleep(x) svcSleepThread(1000UL * x)

#define GPU_TRIM_SYS_GPCPLL_COEFF 0x4
#define GPU_TRIM_SYS_GPCPLL(x) (*(volatile u32 *)(g_gpu_base + 0x137000ul + (x)))

#define CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL 0x60
#define CLK_RST_CONTROLLER_PTO_CLK_CNT_STATUS 0x64
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_X 0x280
#define CLK_RST_CONTROLLER_RST_DEVICES_X 0x28C

/*! PTO_CLK_CNT */
#define PTO_REF_CLK_WIN_CFG_MASK 0xF
#define PTO_REF_CLK_WIN_CFG_16P  0xF
#define PTO_CNT_EN               BIT(9)
#define PTO_CNT_RST              BIT(10)
#define PTO_CLK_ENABLE           BIT(13)
#define PTO_SRC_SEL_SHIFT        14
#define PTO_SRC_SEL_MASK         0x1FF
#define PTO_DIV_SEL_MASK         (3 << 23)
#define PTO_DIV_SEL_GATED        (0 << 23)
#define PTO_DIV_SEL_DIV1         (1 << 23)
#define PTO_DIV_SEL_DIV2_RISING  (2 << 23)
#define PTO_DIV_SEL_DIV2_FALLING (3 << 23)
#define PTO_DIV_SEL_CPU_EARLY    (0 << 23)
#define PTO_DIV_SEL_CPU_LATE     (1 << 23)

#define PTO_CLK_CNT_BUSY         BIT(31)
#define PTO_CLK_CNT              0xFFFFFF
#define CLK_PTO_CCLK_G           0x12
#define CLK_PTO_EMC              0x24

#define CLOCK(x) (*(volatile u32 *)(g_clk_base + (x)))

static uintptr_t g_clk_base = 0;
static uintptr_t g_gpu_base = 0;
static u64 g_update_ticks = 0;
static u32 g_cpu_freq = 0;
static u32 g_gpu_freq = 0;
static u32 g_mem_freq = 0;

static u32 _clock_get_dev_freq(u32 id)
{
    const u32 pto_win = 16;
    const u32 pto_osc = 32768;

    u32 val = ((id & PTO_SRC_SEL_MASK) << PTO_SRC_SEL_SHIFT) | PTO_DIV_SEL_DIV1 | PTO_CLK_ENABLE | (pto_win - 1);
    CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL) = val;
    (void)CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL);
    usleep(2);

    CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL) = val | PTO_CNT_RST;
    (void)CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL);
    usleep(2);

    CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL) = val;
    (void)CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL);
    usleep(2);

    CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL) = val | PTO_CNT_EN;
    (void)CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL);
    usleep((1000000ULL * pto_win / pto_osc) + 12 + 2); // 502 us.

    while (CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_STATUS) & PTO_CLK_CNT_BUSY)
        ;

    u32 cnt = CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_STATUS) & PTO_CLK_CNT;

    CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL) = 0;
    (void)CLOCK(CLK_RST_CONTROLLER_PTO_CLK_CNT_CNTL);
    usleep(2);

    u32 freq_khz = (u64)cnt * pto_osc / pto_win / 1000;

    return freq_khz;
}

static inline Result _svcQueryIoMappingFallback(u64* virtaddr, u64 physaddr, u64 size)
{
    if(hosversionAtLeast(10,0,0))
    {
        u64 out_size;
        return svcQueryIoMapping(virtaddr, &out_size, physaddr, size);
    }
    else
    {
        return svcLegacyQueryIoMapping(virtaddr, physaddr, size);
    }
}

static void _clock_update_freqs(void)
{
    u64 ticks = armGetSystemTick();
    if(armTicksToNs(ticks - g_update_ticks) <= WAIT_NS)
    {
        return;
    }

    g_update_ticks = ticks;

    if (!g_clk_base)
    {
        _svcQueryIoMappingFallback(&g_clk_base, 0x60006000ul, 0x1000);
    }

    if(!g_clk_base)
    {
        return;
    }

    g_mem_freq = _clock_get_dev_freq(CLK_PTO_EMC) * 1000;
    g_cpu_freq = _clock_get_dev_freq(CLK_PTO_CCLK_G) * 1000;

    if (!g_gpu_base)
    {
        _svcQueryIoMappingFallback(&g_gpu_base, 0x57000000ul, 0x1000000);
    }

    if (!g_gpu_base)
    {
        return;
    }

    bool gpu_enabled = (CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_X) & BIT(24)) && !(CLOCK(CLK_RST_CONTROLLER_RST_DEVICES_X) & BIT(24));
    if(!gpu_enabled)
    {
        return;
    }

    const u32 osc = 38400000;
    u32 coeff = GPU_TRIM_SYS_GPCPLL(GPU_TRIM_SYS_GPCPLL_COEFF);
    u32 divm  = coeff & 0xFF;
    u32 divn  = (coeff >>  8) & 0xFF;
    u32 divp  = (coeff >> 16) & 0x3F;
    g_gpu_freq = osc * divn / (divm * divp) / 2;
}


u32 t210ClkCpuFreq(void)
{
    _clock_update_freqs();
    return g_cpu_freq;
}

u32 t210ClkMemFreq(void)
{
    _clock_update_freqs();
    return g_mem_freq;
}

u32 t210ClkGpuFreq(void)
{
    _clock_update_freqs();
    return g_gpu_freq;
}
