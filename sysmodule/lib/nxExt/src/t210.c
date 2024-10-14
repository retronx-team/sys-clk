/*
 * Copyright (c) 2020-2023 CTCaer
 * Copyright (c) 2023 p-sam
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

#include "nxExt/t210.h"

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

/* Actmon Global registers */
#define ACTMON_GLB_STATUS          0x0
#define ACTMON_MCCPU_MON_ACT      BIT(8)
#define ACTMON_MCALL_MON_ACT      BIT(9)
#define ACTMON_CPU_FREQ_MON_ACT   BIT(10)
#define ACTMON_BPMP_MON_ACT       BIT(14)
#define ACTMON_CPU_MON_ACT        BIT(15)

#define ACTMON_GLB_PERIOD_CTRL     0x4
#define ACTMON_GLB_PERIOD_USEC    BIT(8)
#define ACTMON_GLB_PERIOD_SAMPLE(n) (((n) - 1) & 0xFF)

/* Actmon Device Registers */
#define ACTMON_DEV_SIZE           0x40
/* Actmon CTRL */
#define ACTMON_DEV_CTRL_K_VAL(k)                       (((k) & 7) << 10)
#define ACTMON_DEV_CTRL_ENB_PERIODIC                   BIT(18)
#define ACTMON_DEV_CTRL_ENB                            BIT(31)

#define ACTMON_PERIOD_MS 20
#define DEV_COUNT_WEIGHT 1024

#define ACTMON_BASE     (g_act_base + 0x800)
#define ACTMON_DEV_BASE (ACTMON_BASE + 0x80)
#define ACTMON(x) (*(volatile u32 *)(ACTMON_BASE + (x)))

typedef enum _actmon_dev_t
{
    ACTMON_DEV_CPU,
    ACTMON_DEV_BPMP,
    ACTMON_DEV_AHB,
    ACTMON_DEV_APB,
    ACTMON_DEV_CPU_FREQ,
    ACTMON_DEV_MC_ALL,
    ACTMON_DEV_MC_CPU,

    ACTMON_DEV_NUM,
} actmon_dev_t;

typedef struct _actmon_dev_reg_t
{
    vu32 ctrl;
    vu32 upper_wnark;
    vu32 lower_wmark;
    vu32 init_avg;
    vu32 avg_upper_wmark;
    vu32 avg_lower_wmark;
    vu32 count_weight;
    vu32 count;
    vu32 avg_count;
    vu32 intr_status;
    vu32 ctrl2;
    vu32 rsvd[5];
} actmon_dev_reg_t;

static uintptr_t g_clk_base = 0;
static uintptr_t g_gpu_base = 0;
static uintptr_t g_act_base = 0;
static u64 g_update_ticks = 0;
static u32 g_cpu_freq = 0;
static u32 g_gpu_freq = 0;
static u32 g_mem_freq = 0;
static u32 g_emc_lall = 0;
static u32 g_emc_lcpu = 0;

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

static void _actmon_dev_enable(actmon_dev_t dev, u32 freq, u32 weight)
{
    actmon_dev_reg_t *regs = (actmon_dev_reg_t *)(ACTMON_DEV_BASE + (dev * ACTMON_DEV_SIZE));

    regs->init_avg = (u32)freq * ACTMON_PERIOD_MS / 2;
    regs->count_weight = weight;

    regs->ctrl = ACTMON_DEV_CTRL_ENB | ACTMON_DEV_CTRL_ENB_PERIODIC | ACTMON_DEV_CTRL_K_VAL(3); // 8 samples average.
}

static u32 _actmon_dev_get_count_avg(actmon_dev_t dev)
{
    actmon_dev_reg_t *regs = (actmon_dev_reg_t *)(ACTMON_DEV_BASE + (dev * ACTMON_DEV_SIZE));

    return regs->avg_count;
}

static inline Result _svcQueryMemoryMappingFallback(u64* virtaddr, u64 physaddr, u64 size)
{
    if(hosversionAtLeast(10,0,0))
    {
        u64 out_size;
        return svcQueryMemoryMapping(virtaddr, &out_size, physaddr, size);
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
        _svcQueryMemoryMappingFallback(&g_clk_base, 0x60006000ul, 0x1000);
    }

    if(!g_clk_base)
    {
        return;
    }

    g_mem_freq = _clock_get_dev_freq(CLK_PTO_EMC) * 1000;
    g_cpu_freq = _clock_get_dev_freq(CLK_PTO_CCLK_G) * 1000;

    if (!g_gpu_base)
    {
        _svcQueryMemoryMappingFallback(&g_gpu_base, 0x57000000ul, 0x1000000);
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

    if (!g_act_base)
    {
        _svcQueryMemoryMappingFallback(&g_act_base, 0x6000C000ul, 0x1000);
    }

    if(!g_act_base)
    {
        return;
    }

    const u32 osc = 38400000;
    u32 coeff = GPU_TRIM_SYS_GPCPLL(GPU_TRIM_SYS_GPCPLL_COEFF);
    u32 divm  = coeff & 0xFF;
    u32 divn  = (coeff >>  8) & 0xFF;
    u32 divp  = (coeff >> 16) & 0x3F;
    g_gpu_freq = osc * divn / (divm * divp) / 2;

    u32 emc_freq = g_mem_freq / 1000;

    // Check if actmon is disabled
    if (!(ACTMON(ACTMON_GLB_STATUS) & ACTMON_MCALL_MON_ACT))
    {
        ACTMON(ACTMON_GLB_PERIOD_CTRL) = ACTMON_GLB_PERIOD_SAMPLE(ACTMON_PERIOD_MS);
        _actmon_dev_enable(ACTMON_DEV_MC_ALL, emc_freq, 256 * 4);
    }

    // Check if actmon is disabled
    if (!(ACTMON(ACTMON_GLB_STATUS) & ACTMON_MCCPU_MON_ACT))
        _actmon_dev_enable(ACTMON_DEV_MC_CPU, emc_freq, 256 * 4);

    // Get 1000 -> 100.0.
    g_emc_lall = (u64)_actmon_dev_get_count_avg(ACTMON_DEV_MC_ALL) * 10 * 100 / (emc_freq * ACTMON_PERIOD_MS);
    g_emc_lcpu = (u64)_actmon_dev_get_count_avg(ACTMON_DEV_MC_CPU) * 10 * 100 / (emc_freq * ACTMON_PERIOD_MS);
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

u32 t210EmcLoadAll()
{
    _clock_update_freqs();
    return g_emc_lall;
}

u32 t210EmcLoadCpu()
{
    _clock_update_freqs();
    return g_emc_lcpu;
}
