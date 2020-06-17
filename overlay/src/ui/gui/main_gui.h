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

#include "base_menu_gui.h"

class MainGui : public BaseMenuGui
{
    protected:
        tsl::elm::ToggleListItem* enabledToggle;

    public:
        MainGui() {}
        ~MainGui() {}
        void listUI() override;
        void refresh() override;
};
