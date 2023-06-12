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

class AppProfileGui : public BaseMenuGui
{
    protected:
        std::uint64_t applicationId;
        SysClkTitleProfileList* profileList;

        void openFreqChoiceGui(tsl::elm::ListItem* listItem, SysClkProfile profile, SysClkModule module);
        void addModuleListItem(SysClkProfile profile, SysClkModule module);
        void addProfileUI(SysClkProfile profile);

    public:
        AppProfileGui(std::uint64_t applicationId, SysClkTitleProfileList* profileList);
        ~AppProfileGui();
        void listUI() override;
        static void changeTo(std::uint64_t applicationId);
        void update() override;
};
