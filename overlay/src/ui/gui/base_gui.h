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

#include <tesla.hpp>

#include "../style.h"

class BaseGui : public tsl::Gui
{
    public:
        BaseGui() {}
        ~BaseGui() {}
         virtual void preDraw(tsl::gfx::Renderer* renderer);
        void update() override;
        tsl::elm::Element* createUI() override;
        virtual tsl::elm::Element* baseUI() = 0;
        virtual void refresh() {}
};
