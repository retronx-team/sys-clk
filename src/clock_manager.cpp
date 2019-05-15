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

ClockManager::ClockManager()
{
    this->config = new Config(FILE_CONFIG_DIR "/config.ini");
    this->applicationTid = 0;
    this->profile = ClockProfile_Handheld;
    this->freqs = new std::uint32_t[ClockModule_EnumMax];
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
        for (unsigned int module = 0; module < ClockModule_EnumMax; module++)
        {
            hz = this->config->GetClockHz(this->applicationTid, (ClockModule)module, this->profile);

            if (hz)
            {
                hz = Clocks::GetNearestHz((ClockModule)module, this->profile, hz);

                if (hz != this->freqs[module])
                {
                    FileUtils::LogLine("[mgr] Setting %s clock to %u", Clocks::GetModuleName((ClockModule)module, true), hz);
                    Clocks::SetHz((ClockModule)module, hz);
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
        FileUtils::LogLine("[mgr] Application TitleID changed to: %016lX", applicationTid);
        this->applicationTid = applicationTid;
        hasChanged = true;
    }

    ClockProfile profile = Clocks::GetCurrentProfile();
    if (profile != this->profile)
    {
        FileUtils::LogLine("[mgr] Console profile changed to: %s", Clocks::GetProfileName(profile, true));
        this->profile = profile;
        hasChanged = true;
    }

    // restore clocks to stock values on app or profile change
    if(hasChanged)
    {
        Clocks::ResetToStock();
    }

    std::uint32_t hz = 0;
    for (unsigned int module = 0; module < ClockModule_EnumMax; module++)
    {
        hz = Clocks::GetCurrentHz((ClockModule)module);
        if (hz != 0 && hz != this->freqs[module])
        {
            FileUtils::LogLine("[mgr] %s clock is now %u", Clocks::GetModuleName((ClockModule)module, true), hz);
            this->freqs[module] = hz;
            hasChanged = true;
        }
    }

    return hasChanged;
}
