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
#include <stddef.h>

typedef enum
{
    SysClkProfile_Handheld = 0,
    SysClkProfile_HandheldCharging,
    SysClkProfile_HandheldChargingUSB,
    SysClkProfile_HandheldChargingOfficial,
    SysClkProfile_Docked,
    SysClkProfile_EnumMax
} SysClkProfile;

typedef enum
{
    SysClkModule_CPU = 0,
    SysClkModule_GPU,
    SysClkModule_MEM,
    SysClkModule_EnumMax
} SysClkModule;

typedef enum
{
    SysClkThermalSensor_SOC = 0,
    SysClkThermalSensor_PCB,
    SysClkThermalSensor_EnumMax
} SysClkThermalSensor;

typedef struct
{
    uint8_t enabled;
    uint64_t applicationTid;
    SysClkProfile profile;
    uint32_t freqs[SysClkModule_EnumMax];
    uint32_t overrideFreqs[SysClkModule_EnumMax];
    uint32_t temps[SysClkThermalSensor_EnumMax];
} SysClkContext;

typedef struct
{
    uint64_t applicationTid;
    SysClkModule module;
    SysClkProfile profile;
} SysClkProfileKey;

#define SYSCLK_GPU_HANDHELD_MAX_HZ 460800000
#define SYSCLK_GPU_UNOFFICIAL_CHARGER_MAX_HZ 768000000

extern uint32_t sysclk_g_freq_table_mem_hz[];
extern uint32_t sysclk_g_freq_table_cpu_hz[];
extern uint32_t sysclk_g_freq_table_gpu_hz[];

#define SYSCLK_ENUM_VALID(n, v) ((v) < n##_EnumMax)

static inline const char* SysClkFormatModule(SysClkModule module, bool pretty)
{
    switch(module)
    {
        case SysClkModule_CPU:
            return pretty ? "CPU" : "cpu";
        case SysClkModule_GPU:
            return pretty ? "GPU" : "gpu";
        case SysClkModule_MEM:
            return pretty ? "Memory" : "mem";
        default:
            return NULL;
    }
}

static inline const char* SysClkFormatThermalSensor(SysClkThermalSensor thermSensor, bool pretty)
{
    switch(thermSensor)
    {
        case SysClkThermalSensor_SOC:
            return pretty ? "SOC" : "soc";
        case SysClkThermalSensor_PCB:
            return pretty ? "PCB" : "pcb";
        default:
            return NULL;
    }
}

static inline const char* SysClkFormatProfile(SysClkProfile profile, bool pretty)
{
    switch(profile)
    {
        case SysClkProfile_Docked:
            return pretty ? "Docked" : "docked";
        case SysClkProfile_Handheld:
            return pretty ? "Handheld" : "handheld";
        case SysClkProfile_HandheldCharging:
            return pretty ? "Charging" : "handheld_charging";
        case SysClkProfile_HandheldChargingUSB:
            return pretty ? "USB Charger" : "handheld_charging_usb";
        case SysClkProfile_HandheldChargingOfficial:
            return pretty ? "Official Charger" : "handheld_charging_official";
        default:
            return NULL;
    }
}
