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

#include "base_gui.h"

class FatalGui : public BaseGui
{
    protected:
        std::string message;
        std::string info;

    public:
        FatalGui(const std::string message, const std::string info);
        ~FatalGui() {}
        tsl::elm::Element* baseUI() override;
        bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight);
        static void openWithResultCode(std::string tag, Result rc);
};
