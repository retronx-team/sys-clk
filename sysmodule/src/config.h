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
#include <atomic>
#include <ctime>
#include <map>
#include <mutex>
#include <initializer_list>
#include <string>
#include <switch.h>
#include <minIni.h>
#include <nxExt.h>
#include "board.h"

#define CONFIG_VAL_SECTION "values"

class Config
{
  public:
    Config(std::string path);
    virtual ~Config();

    static Config* CreateDefault();

    bool Refresh();

    bool HasProfilesLoaded();

    std::uint8_t GetProfileCount(std::uint64_t tid);
    void GetProfiles(std::uint64_t tid, SysClkTitleProfileList* out_profiles);
    bool SetProfiles(std::uint64_t tid, SysClkTitleProfileList* profiles, bool immediate);
    std::uint32_t GetAutoClockHz(std::uint64_t tid, SysClkModule module, SysClkProfile profile);

    void SetEnabled(bool enabled);
    bool Enabled();
    void SetOverrideHz(SysClkModule module, std::uint32_t hz);
    std::uint32_t GetOverrideHz(SysClkModule module);

    std::uint64_t GetConfigValue(SysClkConfigValue val);
    const char* GetConfigValueName(SysClkConfigValue val, bool pretty);
    void GetConfigValues(SysClkConfigValueList* out_configValues);
    bool SetConfigValues(SysClkConfigValueList* configValues, bool immediate);
  protected:
    void Load();
    void Close();

    time_t CheckModificationTime();
    std::uint32_t FindClockMHz(std::uint64_t tid, SysClkModule module, SysClkProfile profile);
    std::uint32_t FindClockHzFromProfiles(std::uint64_t tid, SysClkModule module, std::initializer_list<SysClkProfile> profiles);
    static int BrowseIniFunc(const char* section, const char* key, const char* value, void* userdata);

    std::map<std::tuple<std::uint64_t, SysClkProfile, SysClkModule>, std::uint32_t> profileMHzMap;
    std::map<std::uint64_t, std::uint8_t> profileCountMap;
    bool loaded;
    std::string path;
    time_t mtime;
    LockableMutex configMutex;
    LockableMutex overrideMutex;
    std::atomic_bool enabled;
    std::uint32_t overrideFreqs[SysClkModule_EnumMax];
    std::uint64_t configValues[SysClkConfigValue_EnumMax];
};
