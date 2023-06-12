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

#include "../../ipc.h"
#include "base_menu_gui.h"
#include "freq_choice_gui.h"

class GlobalOverrideGui : public BaseMenuGui
{
    protected:
        tsl::elm::ListItem* listItems[SysClkModule_EnumMax];
        std::uint32_t listHz[SysClkModule_EnumMax];

        void openFreqChoiceGui(SysClkModule module);
        void addModuleListItem(SysClkModule module);

    public:
        GlobalOverrideGui();
        ~GlobalOverrideGui() {}
        void listUI() override;
        void refresh() override;
};
