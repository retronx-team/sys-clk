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
#include <switch.h>
#include <cstdint>

#define PROCESS_MANAGEMENT_QLAUNCH_TID 0x0100000000001000ULL

class ProcessManagement
{
  public:
    static void Initialize();
    static void WaitForQLaunch();
    static std::uint64_t GetCurrentApplicationTid();
    static void Exit();
};
