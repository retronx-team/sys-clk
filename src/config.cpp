/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include "errors.h"
#include "file_utils.h"

Config::Config(std::string path)
{
    this->path = path;
    this->ini = NULL;
    this->tidSections = std::map<std::uint64_t, std::string>();
    this->mtime = 0;
}

Config::~Config()
{
    this->Close();
}

Config *Config::CreateDefault()
{
    return new Config(FILE_CONFIG_DIR "/config.ini");
}

void Config::Load()
{
    FileUtils::Log("[cfg] reading %s\n", this->path.c_str());

    this->Close();
    this->ini = new INIReader(path);
    this->mtime = this->CheckModificationTime();

    if (this->ini->ParseError() != 0)
    {
        FileUtils::Log("[cfg] Warning: %s while reading %s\n", this->LastError().c_str(), this->path.c_str());
    }

    if (this->ini->ParseError() >= 0)
    {
        std::uint64_t tid = 0;
        for (auto f : this->ini->Sections())
        {
            tid = strtoul(f.c_str(), NULL, 16);
            if (tid == 0)
            {
                FileUtils::Log("[cfg] skipped section '%s': invalid tid\n", f.c_str());
                continue;
            }
            this->tidSections[tid] = f;
        }
    }
}

void Config::Close()
{
    if (this->ini)
    {
        delete this->ini;
    }
    this->tidSections.clear();
}

bool Config::Refresh()
{
    if (!this->Loaded() || this->mtime != this->CheckModificationTime())
    {
        this->Load();
        return true;
    }
    return false;
}

bool Config::Loaded()
{
    return this->ini != NULL;
}

time_t Config::CheckModificationTime()
{
    time_t mtime = 0;
    struct stat st;
    if (stat(this->path.c_str(), &st) == 0)
    {
        mtime = st.st_mtime;
    }

    return mtime;
}

std::string Config::LastError()
{
    if (!this->Loaded())
    {
        return "Not loaded";
    }

    if (this->ini->ParseError() == -1)
    {
        return "File loading error";
    }

    if (this->ini->ParseError() > 0)
    {
        return "Parse error at line " + std::to_string(this->ini->ParseError());
    }

    return "";
}

std::uint32_t Config::FindClockHzFromProfiles(std::uint64_t tid, PcvModule module, std::initializer_list<ClockProfile> profiles) {
    std::uint32_t mhz = 0;

    if (this->Loaded())
    {
        std::map<std::uint64_t, std::string>::iterator it = this->tidSections.find(tid);
        if (it != this->tidSections.end())
        {
            for(auto profile: profiles)
            {
                std::string key = Clocks::GetProfileName(profile) + "_" + Clocks::GetModuleName(module);
                mhz = (std::uint32_t)this->ini->GetInteger(it->second, key, 0);

                if(mhz > 0) {
                    break;
                }
            }
        }
    }

    return std::max((std::uint32_t)0, mhz * 1000000);
}

std::uint32_t Config::GetClockHz(std::uint64_t tid, PcvModule module, ClockProfile profile)
{
    switch(profile) {
        case ClockProfile_Handheld:
            return FindClockHzFromProfiles(tid, module, {ClockProfile_Handheld});
        case ClockProfile_HandheldCharging:
        case ClockProfile_HandheldChargingUSB:
            return FindClockHzFromProfiles(tid, module, {ClockProfile_HandheldChargingUSB, ClockProfile_HandheldCharging, ClockProfile_Handheld});
        case ClockProfile_HandheldChargingOfficial:
            return FindClockHzFromProfiles(tid, module, {ClockProfile_HandheldChargingOfficial, ClockProfile_HandheldCharging, ClockProfile_Handheld});
        case ClockProfile_Docked:
            return FindClockHzFromProfiles(tid, module, {ClockProfile_Docked});
    }

    return 0;
}
