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
#include <cstring>
#include "errors.h"
#include "file_utils.h"

Config::Config(std::string path)
{
    this->path = path;
    this->loaded = false;
    this->profileMhzMap = std::map<std::tuple<std::uint64_t, SysClkProfile, SysClkModule>, std::uint32_t>();
    this->profileCountMap = std::map<std::uint64_t, std::uint8_t>();
    this->mtime = 0;
    this->enabled = false;
    for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
    {
        this->overrideFreqs[i] = 0;
    }
}

Config::~Config()
{
    std::scoped_lock lock{this->profileMutex};
    this->Close();
}

Config *Config::CreateDefault()
{
    return new Config(FILE_CONFIG_DIR "/config.ini");
}

void Config::Load()
{
    FileUtils::LogLine("[cfg] reading %s", this->path.c_str());

    this->Close();
    this->mtime = this->CheckModificationTime();
    if(!this->mtime)
    {
        FileUtils::LogLine("[cfg] Error finding file");
    }
    else if (!ini_browse(&BrowseIniFunc, this, this->path.c_str()))
    {
        FileUtils::LogLine("[cfg] Error loading file");
    }

    this->loaded = true;
}

void Config::Close()
{
    this->loaded = false;
    this->profileMhzMap.clear();
    this->profileCountMap.clear();
}

bool Config::Refresh()
{
    std::scoped_lock lock{this->profileMutex};
    if (!this->loaded || this->mtime != this->CheckModificationTime())
    {
        this->Load();
        return true;
    }
    return false;
}

bool Config::HasProfilesLoaded()
{
    std::scoped_lock lock{this->profileMutex};
    return this->loaded;
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

std::uint32_t Config::FindClockMhz(std::uint64_t tid, SysClkModule module, SysClkProfile profile)
{
    if (this->loaded)
    {
        std::map<std::tuple<std::uint64_t, SysClkProfile, SysClkModule>, std::uint32_t>::const_iterator it = this->profileMhzMap.find(std::make_tuple(tid, profile, module));
        if (it != this->profileMhzMap.end())
        {
            return it->second;
        }
    }

    return 0;
}

std::uint32_t Config::FindClockHzFromProfiles(std::uint64_t tid, SysClkModule module, std::initializer_list<SysClkProfile> profiles)
{
    std::uint32_t mhz = 0;

    if (this->loaded)
    {
        for(auto profile: profiles)
        {
            mhz = FindClockMhz(tid, module, profile);

            if(mhz)
            {
                break;
            }
        }
    }

    return std::max((std::uint32_t)0, mhz * 1000000);
}

std::uint32_t Config::GetAutoClockHz(std::uint64_t tid, SysClkModule module, SysClkProfile profile)
{
    std::scoped_lock lock{this->profileMutex};
    switch(profile)
    {
        case SysClkProfile_Handheld:
            return FindClockHzFromProfiles(tid, module, {SysClkProfile_Handheld});
        case SysClkProfile_HandheldCharging:
        case SysClkProfile_HandheldChargingUSB:
            return FindClockHzFromProfiles(tid, module, {SysClkProfile_HandheldChargingUSB, SysClkProfile_HandheldCharging, SysClkProfile_Handheld});
        case SysClkProfile_HandheldChargingOfficial:
            return FindClockHzFromProfiles(tid, module, {SysClkProfile_HandheldChargingOfficial, SysClkProfile_HandheldCharging, SysClkProfile_Handheld});
        case SysClkProfile_Docked:
            return FindClockHzFromProfiles(tid, module, {SysClkProfile_Docked});
        default:
            ERROR_THROW("Unhandled SysClkProfile: %u", profile);
    }

    return 0;
}

std::uint32_t Config::GetClockMhz(std::uint64_t tid, SysClkModule module, SysClkProfile profile)
{
    std::scoped_lock lock{this->profileMutex};
    return FindClockMhz(tid, module, profile);
}

bool Config::SetClockMhz(std::uint64_t tid, SysClkModule module, SysClkProfile profile, std::uint32_t mhz)
{
    std::scoped_lock lock{this->profileMutex};
    char key[0x100] = {0};
    char section[17] = {0};
    char val[11] = {0};

    snprintf(section, sizeof(section), "%016lX", tid);
    snprintf(key, sizeof(key), "%s_%s", Clocks::GetProfileName(profile, false), Clocks::GetModuleName(module, false));

    if(mhz)
    {
        snprintf(val, sizeof(val), "%d", mhz);
        return ini_puts(section, key, val, this->path.c_str());
    }
    else
    {
        return ini_puts(section, key, 0, this->path.c_str());
    }
}

std::uint8_t Config::GetProfileCount(std::uint64_t tid)
{
    std::map<std::uint64_t, std::uint8_t>::iterator it = this->profileCountMap.find(tid);
    if (it == this->profileCountMap.end())
    {
        return 0;
    }

    return it->second;
}

int Config::BrowseIniFunc(const char* section, const char* key, const char* value, void *userdata)
{
    std::uint64_t tid = strtoul(section, NULL, 16);

    if(!tid || strlen(section) != 16)
    {
        FileUtils::LogLine("[cfg] Skipping key '%s' in section '%s': Invalid TitleID", key, section);
        return 1;
    }

    SysClkProfile parsedProfile = SysClkProfile_EnumMax;
    SysClkModule parsedModule = SysClkModule_EnumMax;

    for(unsigned int profile = 0; profile < SysClkProfile_EnumMax; profile++)
    {
        const char* profileCode = Clocks::GetProfileName((SysClkProfile)profile, false);
        size_t profileCodeLen = strlen(profileCode);

        if(!strncmp(key, profileCode, profileCodeLen) && key[profileCodeLen] == '_')
        {
            const char* subkey = key + profileCodeLen + 1;

            for(unsigned int module = 0; module < SysClkModule_EnumMax; module++)
            {
                const char* moduleCode = Clocks::GetModuleName((SysClkModule)module, false);
                size_t moduleCodeLen = strlen(moduleCode);
                if(!strncmp(subkey, moduleCode, moduleCodeLen) && subkey[moduleCodeLen] == '\0')
                {
                    parsedProfile = (SysClkProfile)profile;
                    parsedModule = (SysClkModule)module;
                }
            }
        }
    }

    if(parsedModule == SysClkModule_EnumMax || parsedProfile == SysClkProfile_EnumMax)
    {
        FileUtils::LogLine("[cfg] Skipping key '%s' in section '%s': Unrecognized key", key, section);
        return 1;
    }

    std::uint32_t mhz = strtoul(value, NULL, 10);
    if(!mhz)
    {
        FileUtils::LogLine("[cfg] Skipping key '%s' in section '%s': Invalid value", key, section);
        return 1;
    }

    Config* config = (Config*)userdata;
    config->profileMhzMap[std::make_tuple(tid, parsedProfile, parsedModule)] = mhz;
    std::map<std::uint64_t, std::uint8_t>::iterator it = config->profileCountMap.find(tid);
    if (it == config->profileCountMap.end())
    {
        config->profileCountMap[tid] = 1;
    }
    else
    {
        it->second++;
    }

    return 1;
}

void Config::SetEnabled(bool enabled)
{
    this->enabled = enabled;
}

bool Config::Enabled()
{
    return this->enabled;
}

void Config::SetOverrideHz(SysClkModule module, std::uint32_t hz)
{
    std::scoped_lock lock{this->overrideMutex};
    if(!SYSCLK_ENUM_VALID(SysClkModule, module))
    {
        ERROR_THROW("Unhandled SysClkModule: %u", module);
    }
    this->overrideFreqs[module] = hz;
}

std::uint32_t Config::GetOverrideHz(SysClkModule module)
{
    std::scoped_lock lock{this->overrideMutex};
    if(!SYSCLK_ENUM_VALID(SysClkModule, module))
    {
        ERROR_THROW("Unhandled SysClkModule: %u", module);
    }
    return this->overrideFreqs[module];
}
