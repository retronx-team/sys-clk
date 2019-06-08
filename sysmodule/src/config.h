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
#include <ctime>
#include <map>
#include <initializer_list>
#include <switch.h>
#include <minIni.h>
#include "clocks.h"

class Config
{
  public:
    Config(std::string path);
    virtual ~Config();

    static Config *CreateDefault();

    void Load();
    void Close();
    bool Refresh();

    bool HasLoaded();
    std::string LastError();

    std::uint32_t GetClockHz(std::uint64_t tid, ClockModule module, ClockProfile profile);
  protected:
    time_t CheckModificationTime();
    std::uint32_t FindClockHzFromProfiles(std::uint64_t tid, ClockModule module, std::initializer_list<ClockProfile> profiles);
    static int BrowseIniFunc(const char* section, const char* key, const char* value, void *userdata);

    std::map<std::tuple<std::uint64_t, ClockProfile, ClockModule>, std::uint32_t> profileMhzMap;
    minIni *ini;
    std::string path;
    time_t mtime;
};
