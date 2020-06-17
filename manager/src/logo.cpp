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

#include "logo.h"

Logo::Logo(LogoStyle style) {
    this->logoLabel = new brls::Label(brls::LabelStyle::LIST_ITEM, "sys-clk", style == LogoStyle::ABOUT);
    this->logoLabel->setParent(this);

    int logoFont = brls::Application::findFont(LOGO_FONT_NAME);
    if (logoFont >= 0)
    {
        this->logoLabel->setFont(logoFont);
    }

    if (style == LogoStyle::ABOUT)
    {
        this->logoLabel->setFontSize(LOGO_ABOUT_FONT_SIZE);
        this->logoLabel->setHorizontalAlign(NVG_ALIGN_CENTER);
    }

    if (style == LogoStyle::HEADER)
    {
        this->logoLabel->setFontSize(LOGO_HEADER_FONT_SIZE);
        this->descLabel = new brls::Label(brls::LabelStyle::LIST_ITEM, "manager");
        this->descLabel->setParent(this);
        this->descLabel->setFontSize(LOGO_DESC_FONT_SIZE);
    }

}

Logo::~Logo() {
    delete this->logoLabel;

    if (this->descLabel)
        delete this->descLabel;
}

void Logo::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{
    this->logoLabel->frame(ctx);

    if (this->descLabel)
        this->descLabel->frame(ctx);
}

void Logo::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    this->logoLabel->setBoundaries(this->x, this->y + LOGO_OFFSET, this->width, this->height);
    this->logoLabel->layout(vg, style, stash);
    this->height = this->logoLabel->getHeight();

    if (this->descLabel)
    {
        this->descLabel->layout(vg, style, stash);
        this->descLabel->setBoundaries(this->x + LOGO_HEADER_SPACING + this->logoLabel->getWidth(), this->y + style->AppletFrame.titleOffset - 1, this->descLabel->getWidth(), height);
    }
}
