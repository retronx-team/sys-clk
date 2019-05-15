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
    this->ini = NULL;
    this->profileMhzMap = std::map<std::tuple<std::uint64_t, ClockProfile, ClockModule>, std::uint32_t>();
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
    FileUtils::LogLine("[cfg] reading %s", this->path.c_str());

    this->Close();
    this->mtime = this->CheckModificationTime();
    if(!this->mtime) {
        FileUtils::LogLine("[cfg] Error finding file");
    }

    this->ini = new minIni(path);

    if (!this->ini->browse(&BrowseIniFunc, this))
    {
        FileUtils::LogLine("[cfg] Error loading file");
    }
}

void Config::Close()
{
    if (this->ini)
    {
        delete this->ini;
    }
    this->profileMhzMap.clear();
}

bool Config::Refresh()
{
    if (!this->HasLoaded() || this->mtime != this->CheckModificationTime())
    {
        this->Load();
        return true;
    }
    return false;
}

bool Config::HasLoaded()
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

std::uint32_t Config::FindClockHzFromProfiles(std::uint64_t tid, ClockModule module, std::initializer_list<ClockProfile> profiles) {
    std::uint32_t mhz = 0;

    if (this->HasLoaded())
    {
        for(auto profile: profiles)
        {
            std::map<std::tuple<std::uint64_t, ClockProfile, ClockModule>, std::uint32_t>::iterator it = this->profileMhzMap.find(std::make_tuple(tid, profile, module));
            if (it != this->profileMhzMap.end()) {
                mhz = it->second;

                if(mhz > 0) {
                    break;
                }
            }
        }
    }

    return std::max((std::uint32_t)0, mhz * 1000000);
}

std::uint32_t Config::GetClockHz(std::uint64_t tid, ClockModule module, ClockProfile profile)
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
        default:
            ERROR_THROW("Unhandled ClockProfile: %u", profile);
    }

    return 0;
}

int Config::BrowseIniFunc(const char* section, const char* key, const char* value, void *userdata) {
    std::uint64_t tid = strtoul(section, NULL, 16);

    if(!tid) {
        FileUtils::LogLine("[cfg] Skipping key '%s' in section '%s': Invalid TitleID", key, section);
        return 1;
    }

    ClockProfile parsedProfile = ClockProfile_EnumMax;
    ClockModule parsedModule = ClockModule_EnumMax;

    for(unsigned int profile = 0; profile < ClockProfile_EnumMax; profile++) {
        const char* profileCode = Clocks::GetProfileName((ClockProfile)profile, false);
        size_t profileCodeLen = strlen(profileCode);

        if(!strncmp(key, profileCode, profileCodeLen) && key[profileCodeLen] == '_') {
            const char* subkey = key + profileCodeLen + 1;

            for(unsigned int module = 0; module < ClockModule_EnumMax; module++) {
                const char* moduleCode = Clocks::GetModuleName((ClockModule)module, false);
                size_t moduleCodeLen = strlen(moduleCode);
                if(!strncmp(subkey, moduleCode, moduleCodeLen) && subkey[moduleCodeLen] == '\0') {
                    parsedProfile = (ClockProfile)profile;
                    parsedModule = (ClockModule)module;
                }
            }
        }
    }

    if(parsedModule == ClockModule_EnumMax || parsedProfile == ClockProfile_EnumMax) {
        FileUtils::LogLine("[cfg] Skipping key '%s' in section '%s': Unrecognized key", key, section);
        return 1;
    }

    std::uint32_t mhz = strtoul(value, NULL, 10);
    if(!mhz) {
        FileUtils::LogLine("[cfg] Skipping key '%s' in section '%s': Invalid value", key, section);
        return 1;
    }

    Config* config = (Config*)userdata;
    config->profileMhzMap[std::make_tuple(tid, parsedProfile, parsedModule)] = mhz;

    return 1;
}