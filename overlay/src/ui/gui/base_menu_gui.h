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
#include "base_gui.h"

class BaseMenuGui : public BaseGui
{
    protected:
        SysClkContext* context;
        std::uint64_t lastContextUpdate;
        tsl::elm::List* listElement;

    public:
        BaseMenuGui();
        ~BaseMenuGui();
        void preDraw(tsl::gfx::Renderer* renderer) override;
        tsl::elm::Element* baseUI() override;
        void refresh() override;
        virtual void listUI() = 0;
};
