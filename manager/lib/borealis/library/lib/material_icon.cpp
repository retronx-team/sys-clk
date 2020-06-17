/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

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

#include <borealis/material_icon.hpp>

namespace brls
{

MaterialIcon::MaterialIcon(std::string icon)
    : icon(icon)
{
}

void MaterialIcon::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    NVGcolor color = a(ctx->theme->textColor);

    nvgTextLineHeight(vg, 1.0f);
    nvgFillColor(vg, color);
    nvgFontSize(vg, height);
    nvgFontFaceId(vg, ctx->fontStash->material);
    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgBeginPath(vg);
    nvgText(vg, this->middleX, this->middleY, this->icon.c_str(), nullptr);
}

void MaterialIcon::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    this->middleX = this->getX() + this->getWidth() / 2;
    this->middleY = this->getY() + this->getHeight() / 2;
}

}; // namespace brls
