/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam

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

#include <borealis/application.hpp>
#include <borealis/header.hpp>

namespace brls
{

Header::Header(std::string label, bool separator, std::string sublabel)
    : label(label)
    , sublabel(sublabel)
    , separator(separator)
{
    Style* style = Application::getStyle();
    this->setHeight(style->Header.height);
}

void Header::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    unsigned padding = style->Header.padding;

    // Rectangle
    nvgBeginPath(vg);
    nvgFillColor(vg, a(ctx->theme->headerRectangleColor));
    nvgRect(vg, x, y + padding, style->Header.rectangleWidth, height - padding * 2);
    nvgFill(vg);

    // Label
    nvgBeginPath(vg);
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgFontSize(vg, style->Header.fontSize);
    nvgFillColor(vg, a(ctx->theme->textColor));
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + style->Header.rectangleWidth + padding, y + height / 2, this->label.c_str(), nullptr);

    // Sublabel
    if (this->sublabel != "")
    {
        nvgBeginPath(vg);
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgFontSize(vg, style->Header.fontSize);
        nvgFillColor(vg, a(ctx->theme->descriptionColor));
        nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgText(vg, x + width - style->Header.rectangleWidth - padding, y + height / 2, this->sublabel.c_str(), nullptr);
    }

    // Separator
    if (this->separator)
    {
        nvgBeginPath(vg);
        nvgFillColor(vg, a(ctx->theme->listItemSeparatorColor));
        nvgRect(vg, x, y + height, width, 1);
        nvgFill(vg);
    }
}

} // namespace brls
