/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019  natinusala
    Copyright (C) 2019-2020 p-sam
    Copyright (C) 2019  m4xw

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <borealis.hpp>

#include "utils.h"

enum class LogoStyle
{
    HEADER = 0,
    ABOUT
};

#define LOGO_FONT_NAME "logo"
#define LOGO_FONT_PATH APP_ASSET("fira/FiraSans-Medium-rnx.ttf")

#define LOGO_HEADER_FONT_SIZE 45
#define LOGO_HEADER_SPACING 12
#define LOGO_ABOUT_FONT_SIZE 55
#define LOGO_DESC_FONT_SIZE 28
#define LOGO_OFFSET 2

class Logo : public brls::View
{
    protected:
        brls::Label* logoLabel = nullptr;
        brls::Label* descLabel = nullptr;
        void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash);
        void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
    public:
        Logo(LogoStyle style);
        virtual ~Logo();
};
