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

#pragma once

#include <borealis/frame_context.hpp>
#include <borealis/view.hpp>

namespace brls
{

// A solid color rectangle
class Rectangle : public View
{
  protected:
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

  public:
    Rectangle(NVGcolor color);

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;

    void setColor(NVGcolor color);

    ~Rectangle() {}

  private:
    NVGcolor color = nvgRGB(0, 0, 255);
};

} // namespace brls
