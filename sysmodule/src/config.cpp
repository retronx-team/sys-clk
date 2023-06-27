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
    this->profileMHzMap = std::map<std::tuple<std::uint64_t, SysClkProfile, SysClkModule>, std::uint32_t>();
    this->profileCountMap = std::map<std::uint64_t, std::uint8_t>();
    this->mtime = 0;
    this->enabled = false;
    for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
    {
        this->overrideFreqs[i] = 0;
    }

    for(unsigned int i = 0; i < SysClkConfigValue_EnumMax; i++)
    {
        this->configValues[i] = sysclkDefaultConfigValue((SysClkConfigValue)i);
    }
}

Config::~Config()
{
    std::scoped_lock lock{this->configMutex};
    this->Close();
}

Config* Config::CreateDefault()
{
    return new Config(FILE_CONFIG_DIR "/config.ini");
}

void Config::Load()
{
    FileUtils::LogLine("[cfg] Reading %s", this->path.c_str());

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
    this->profileMHzMap.clear();
    this->profileCountMap.clear();

    for(unsigned int i = 0; i < SysClkConfigValue_EnumMax; i++)
    {
        this->configValues[i] = sysclkDefaultConfigValue((SysClkConfigValue)i);
    }
}

bool Config::Refresh()
{
    std::scoped_lock lock{this->configMutex};
    if (!this->loaded || this->mtime != this->CheckModificationTime())
    {
        this->Load();
        return true;
    }
    return false;
}

bool Config::HasProfilesLoaded()
{
    std::scoped_lock lock{this->configMutex};
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

std::uint32_t Config::FindClockMHz(std::uint64_t tid, SysClkModule module, SysClkProfile profile)
{
    if (this->loaded)
    {
        std::map<std::tuple<std::uint64_t, SysClkProfile, SysClkModule>, std::uint32_t>::const_iterator it = this->profileMHzMap.find(std::make_tuple(tid, profile, module));
        if (it != this->profileMHzMap.end())
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
            mhz = FindClockMHz(tid, module, profile);

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
    std::scoped_lock lock{this->configMutex};
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

void Config::GetProfiles(std::uint64_t tid, SysClkTitleProfileList* out_profiles)
{
    std::scoped_lock lock{this->configMutex};

    for(unsigned int profile = 0; profile < SysClkProfile_EnumMax; profile++)
    {
        for(unsigned int module = 0; module < SysClkModule_EnumMax; module++)
        {
            out_profiles->mhzMap[profile][module] = FindClockMHz(tid, (SysClkModule)module, (SysClkProfile)profile);
        }
    }
}

bool Config::SetProfiles(std::uint64_t tid, SysClkTitleProfileList* profiles, bool immediate)
{
    std::scoped_lock lock{this->configMutex};
    uint8_t numProfiles = 0;

    // String pointer array passed to ini
    char* iniKeys[SysClkProfile_EnumMax * SysClkModule_EnumMax + 1];
    char* iniValues[SysClkProfile_EnumMax * SysClkModule_EnumMax + 1];

    // Char arrays to build strings
    char keysStr[SysClkProfile_EnumMax * SysClkModule_EnumMax * 0x40];
    char valuesStr[SysClkProfile_EnumMax * SysClkModule_EnumMax * 0x10];
    char section[17] = {0};

    // Iteration pointers
    char** ik = &iniKeys[0];
    char** iv = &iniValues[0];
    char* sk = &keysStr[0];
    char* sv = &valuesStr[0];
    std::uint32_t* mhz = &profiles->mhz[0];

    snprintf(section, sizeof(section), "%016lX", tid);

    for(unsigned int profile = 0; profile < SysClkProfile_EnumMax; profile++)
    {
        for(unsigned int module = 0; module < SysClkModule_EnumMax; module++)
        {
            if(*mhz)
            {
                numProfiles++;

                // Put key and value as string
                snprintf(sk, 0x40, "%s_%s", Board::GetProfileName((SysClkProfile)profile, false), Board::GetModuleName((SysClkModule)module, false));
                snprintf(sv, 0x10, "%d", *mhz);

                // Add them to the ini key/value str arrays
                *ik = sk;
                *iv = sv;
                ik++;
                iv++;

                // We used those chars, get to the next ones
                sk += 0x40;
                sv += 0x10;
            }

            mhz++;
        }
    }

    *ik = NULL;
    *iv = NULL;

    if(!ini_putsection(section, (const char**)iniKeys, (const char**)iniValues, this->path.c_str()))
    {
        return false;
    }

    // Only actually apply changes in memory after a succesful save
    if(immediate)
    {
        mhz = &profiles->mhz[0];
        this->profileCountMap[tid] = numProfiles;
        for(unsigned int profile = 0; profile < SysClkProfile_EnumMax; profile++)
        {
            for(unsigned int module = 0; module < SysClkModule_EnumMax; module++)
            {
                if(*mhz)
                {
                    this->profileMHzMap[std::make_tuple(tid, (SysClkProfile)profile, (SysClkModule)module)] = *mhz;
                }
                else
                {
                    this->profileMHzMap.erase(std::make_tuple(tid, (SysClkProfile)profile, (SysClkModule)module));
                }
                mhz++;
            }
        }
    }

    return true;
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

int Config::BrowseIniFunc(const char* section, const char* key, const char* value, void* userdata)
{
    Config* config = (Config*)userdata;
    std::uint64_t input;
    if(!strcmp(section, CONFIG_VAL_SECTION))
    {
        for(unsigned int kval = 0; kval < SysClkConfigValue_EnumMax; kval++)
        {
            if(!strcmp(key, sysclkFormatConfigValue((SysClkConfigValue)kval, false)))
            {
                input = strtoul(value, NULL, 0);
                if(!sysclkValidConfigValue((SysClkConfigValue)kval, input))
                {
                    input = sysclkDefaultConfigValue((SysClkConfigValue)kval);
                    FileUtils::LogLine("[cfg] Invalid value for key '%s' in section '%s': using default %d", key, section, input);
                }
                config->configValues[kval] = input;
                return 1;
            }
        }

        FileUtils::LogLine("[cfg] Skipping key '%s' in section '%s': Unrecognized config value", key, section);
        return 1;
    }

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
        const char* profileCode = Board::GetProfileName((SysClkProfile)profile, false);
        size_t profileCodeLen = strlen(profileCode);

        if(!strncmp(key, profileCode, profileCodeLen) && key[profileCodeLen] == '_')
        {
            const char* subkey = key + profileCodeLen + 1;

            for(unsigned int module = 0; module < SysClkModule_EnumMax; module++)
            {
                const char* moduleCode = Board::GetModuleName((SysClkModule)module, false);
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

    config->profileMHzMap[std::make_tuple(tid, parsedProfile, parsedModule)] = mhz;
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
    ASSERT_ENUM_VALID(SysClkModule, module);

    std::scoped_lock lock{this->overrideMutex};

    this->overrideFreqs[module] = hz;
}

std::uint32_t Config::GetOverrideHz(SysClkModule module)
{
    ASSERT_ENUM_VALID(SysClkModule, module);

    std::scoped_lock lock{this->overrideMutex};

    return this->overrideFreqs[module];
}

std::uint64_t Config::GetConfigValue(SysClkConfigValue kval)
{
    ASSERT_ENUM_VALID(SysClkConfigValue, kval);

    std::scoped_lock lock{this->configMutex};

    return this->configValues[kval];
}

const char* Config::GetConfigValueName(SysClkConfigValue kval, bool pretty)
{
    ASSERT_ENUM_VALID(SysClkConfigValue, kval);

    const char* result = sysclkFormatConfigValue(kval, pretty);

    return result;
}

void Config::GetConfigValues(SysClkConfigValueList* out_configValues)
{
    std::scoped_lock lock{this->configMutex};

    for(unsigned int kval = 0; kval < SysClkConfigValue_EnumMax; kval++)
    {
        out_configValues->values[kval] = this->configValues[kval];
    }
}

bool Config::SetConfigValues(SysClkConfigValueList* configValues, bool immediate)
{
    std::scoped_lock lock{this->configMutex};

    // String pointer array passed to ini
    const char* iniKeys[SysClkConfigValue_EnumMax + 1];
    char* iniValues[SysClkConfigValue_EnumMax + 1];

    // char arrays to build strings
    char valuesStr[SysClkConfigValue_EnumMax * 0x20];

    // Iteration pointers
    char* sv = &valuesStr[0];
    const char** ik = &iniKeys[0];
    char** iv = &iniValues[0];

    for(unsigned int kval = 0; kval < SysClkConfigValue_EnumMax; kval++)
    {
        if(!sysclkValidConfigValue((SysClkConfigValue)kval, configValues->values[kval]) || configValues->values[kval] == sysclkDefaultConfigValue((SysClkConfigValue)kval))
        {
            continue;
        }

        // Put key and value as string
        // And add them to the ini key/value str arrays
        snprintf(sv, 0x20, "%ld", configValues->values[kval]);
        *ik = sysclkFormatConfigValue((SysClkConfigValue)kval, false);
        *iv = sv;

        // We used those chars, get to the next ones
        sv += 0x20;
        ik++;
        iv++;
    }

    *ik = NULL;
    *iv = NULL;

    if(!ini_putsection(CONFIG_VAL_SECTION, (const char**)iniKeys, (const char**)iniValues, this->path.c_str()))
    {
        return false;
    }

    // Only actually apply changes in memory after a succesful save
    if(immediate)
    {
        for(unsigned int kval = 0; kval < SysClkConfigValue_EnumMax; kval++)
        {
            if(sysclkValidConfigValue((SysClkConfigValue)kval, configValues->values[kval]))
            {
                this->configValues[kval] = configValues->values[kval];
            }
            else
            {
                this->configValues[kval] = sysclkDefaultConfigValue((SysClkConfigValue)kval);
            }
        }
    }

    return true;
}
