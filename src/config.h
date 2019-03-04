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
#include <string>
#include <map>
#include <initializer_list>
#include <switch.h>
#include <inih.h>
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

    std::uint32_t GetClockHz(std::uint64_t tid, PcvModule module, ClockProfile profile);

  protected:
    time_t CheckModificationTime();
    std::uint32_t FindClockHzFromProfiles(std::uint64_t tid, PcvModule module, std::initializer_list<ClockProfile> profiles);

    std::map<std::uint64_t, std::string> tidSections;
    INIReader *ini;
    std::string path;
    time_t mtime;
};
