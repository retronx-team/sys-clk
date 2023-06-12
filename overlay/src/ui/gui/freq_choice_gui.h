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

#include <list>

#include "base_menu_gui.h"

using FreqChoiceListener = std::function<bool(std::uint32_t hz)>;

#define FREQ_DEFAULT_TEXT "Do not override"

class FreqChoiceGui : public BaseMenuGui
{
    protected:
        std::uint32_t selectedHz;
        std::uint32_t* hzList;
        std::uint32_t hzCount;
        FreqChoiceListener listener;
        tsl::elm::ListItem* createFreqListItem(std::uint32_t hz, bool selected);

    public:
        FreqChoiceGui(std::uint32_t selectedHz, std::uint32_t* hzList, std::uint32_t hzCount, FreqChoiceListener listener);
        ~FreqChoiceGui() {}
        void listUI() override;
};
