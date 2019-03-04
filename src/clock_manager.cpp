/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "clock_manager.h"
#include "file_utils.h"
#include "clocks.h"
#include "process_management.h"

static PcvModule g_modules[] = {
    PcvModule_Cpu,
    PcvModule_Gpu,
    PcvModule_Emc
};

static size_t g_modules_count = sizeof(g_modules) / sizeof(g_modules[0]);

ClockManager::ClockManager()
{
    this->config = new Config(FILE_CONFIG_DIR "/config.ini");
    this->applicationTid = 0;
    this->profile = ClockProfile_Handheld;
    this->freqs = new std::uint32_t[g_modules_count];
}

ClockManager::~ClockManager()
{
    delete this->config;
    delete[] this->freqs;
}

void ClockManager::Tick()
{
    if (this->RefreshContext() || this->config->Refresh())
    {
        std::uint32_t hz = 0;
        for (size_t i = 0; i < g_modules_count; i++)
        {
            hz = this->config->GetClockHz(this->applicationTid, g_modules[i], this->profile);

            if (!hz)
            {
                hz = Clocks::GetNearestHz(g_modules[i], this->profile, hz);

                if (hz != this->freqs[i])
                {
                    FileUtils::Log("* setting %s clock to %u\n", Clocks::GetModuleName(g_modules[i]).c_str(), hz);
                    Clocks::SetHz(g_modules[i], hz);
                }
            }
        }
    }
}

bool ClockManager::RefreshContext()
{
    bool hasChanged = false;

    std::uint64_t applicationTid = ProcessManagement::GetCurrentApplicationTitleId();
    if (applicationTid != this->applicationTid)
    {
        FileUtils::Log("* applicationTid changed to: %016lX\n", applicationTid);
        this->applicationTid = applicationTid;
        hasChanged = true;
    }

    ClockProfile profile = Clocks::GetCurrentProfile();
    if (profile != this->profile)
    {
        FileUtils::Log("* console profile changed to: %s\n", Clocks::GetProfileName(profile).c_str());
        this->profile = profile;
        hasChanged = true;
    }

    // restore clocks to stock values on app or profile change
    if(hasChanged)
    {
        Clocks::ResetToStock();
    }

    std::uint32_t hz = 0;
    for (size_t i = 0; i < g_modules_count; i++)
    {
        hz = Clocks::GetCurrentHz(g_modules[i]);
        if (hz != 0 && hz != this->freqs[i])
        {
            FileUtils::Log("* %s clock is now %u\n", Clocks::GetModuleName(g_modules[i]).c_str(), hz);
            this->freqs[i] = hz;
            hasChanged = true;
        }
    }

    return hasChanged;
}
