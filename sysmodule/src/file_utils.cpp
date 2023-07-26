/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "file_utils.h"
#include <nxExt.h>

static LockableMutex g_log_mutex;
static LockableMutex g_csv_mutex;
static std::atomic_bool g_has_initialized = false;
static bool g_log_enabled = false;
static std::uint64_t g_last_flag_check = 0;

extern "C" void __libnx_init_time(void);

static void _FileUtils_InitializeThreadFunc(void* args)
{
    FileUtils::Initialize();
}

bool FileUtils::IsInitialized()
{
    return g_has_initialized;
}

void FileUtils::LogLine(const char* format, ...)
{
    std::scoped_lock lock{g_log_mutex};

    va_list args;
    va_start(args, format);
    if (g_has_initialized)
    {
        FileUtils::RefreshFlags(false);

        if(g_log_enabled)
        {
            FILE* file = fopen(FILE_LOG_FILE_PATH, "a");

            if (file)
            {
                struct timespec now;
                clock_gettime(CLOCK_REALTIME, &now);
                struct tm* nowTm = localtime(&now.tv_sec);

                fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d.%03ld] ", nowTm->tm_year+1900, nowTm->tm_mon+1, nowTm->tm_mday, nowTm->tm_hour, nowTm->tm_min, nowTm->tm_sec, now.tv_nsec / 1000000UL);
                vfprintf(file, format, args);
                fprintf(file, "\n");
                fclose(file);
            }
        }
    }
    va_end(args);
}

void FileUtils::WriteContextToCsv(const SysClkContext* context)
{
    std::scoped_lock lock{g_csv_mutex};

    FILE* file = fopen(FILE_CONTEXT_CSV_PATH, "a");

    if (file)
    {
        // Print header
        if(!ftell(file))
        {
            fprintf(file, "timestamp,profile,app_tid");

            for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
            {
                fprintf(file, ",%s_hz", sysclkFormatModule((SysClkModule)module, false));
            }

            for (unsigned int sensor = 0; sensor < SysClkThermalSensor_EnumMax; sensor++)
            {
                fprintf(file, ",%s_milliC", sysclkFormatThermalSensor((SysClkThermalSensor)sensor, false));
            }

            for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
            {
                fprintf(file, ",%s_real_hz", sysclkFormatModule((SysClkModule)module, false));
            }

            for (unsigned int sensor = 0; sensor < SysClkPowerSensor_EnumMax; sensor++)
            {
                fprintf(file, ",%s_mw", sysclkFormatPowerSensor((SysClkPowerSensor)sensor, false));
            }

            fprintf(file, "\n");
        }

        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);

        fprintf(file, "%ld%03ld,%s,%016lx", now.tv_sec, now.tv_nsec / 1000000UL, sysclkFormatProfile(context->profile, false), context->applicationId);

        for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
        {
            fprintf(file, ",%d", context->freqs[module]);
        }

        for (unsigned int sensor = 0; sensor < SysClkThermalSensor_EnumMax; sensor++)
        {
            fprintf(file, ",%d", context->temps[sensor]);
        }

        for (unsigned int module = 0; module < SysClkModule_EnumMax; module++)
        {
            fprintf(file, ",%d", context->realFreqs[module]);
        }

        for (unsigned int sensor = 0; sensor < SysClkPowerSensor_EnumMax; sensor++)
        {
            fprintf(file, ",%d", context->power[sensor]);
        }

        fprintf(file, "\n");
        fclose(file);
    }
}

void FileUtils::RefreshFlags(bool force)
{
    std::uint64_t now = armTicksToNs(armGetSystemTick());
    if(!force && (now - g_last_flag_check) < FILE_FLAG_CHECK_INTERVAL_NS)
    {
        return;
    }

    FILE* file = fopen(FILE_LOG_FLAG_PATH, "r");
    if (file)
    {
        g_log_enabled = true;
        fclose(file);
    } else {
        g_log_enabled = false;
    }

    g_last_flag_check = now;
}

void FileUtils::InitializeAsync()
{
    Thread initThread = {0};
    threadCreate(&initThread, _FileUtils_InitializeThreadFunc, NULL, NULL, 0x4000, 0x15, 0);
    threadStart(&initThread);
}

Result FileUtils::Initialize()
{
    Result rc = 0;

    if (R_SUCCEEDED(rc))
    {
        rc = timeInitialize();
    }

    __libnx_init_time();
    timeExit();

    if (R_SUCCEEDED(rc))
    {
        rc = fsInitialize();
    }

    if (R_SUCCEEDED(rc))
    {
        rc = fsdevMountSdmc();
    }

    if (R_SUCCEEDED(rc))
    {
        FileUtils::RefreshFlags(true);
        g_has_initialized = true;
        FileUtils::LogLine("=== " TARGET " " TARGET_VERSION " ===");
    }

    return rc;
}

void FileUtils::Exit()
{
    if (!g_has_initialized)
    {
        return;
    }

    g_has_initialized = false;
    g_log_enabled = false;

    fsdevUnmountAll();
    fsExit();
}
