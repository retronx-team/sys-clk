/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
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

#include <borealis/button.hpp>
#include <borealis/hint.hpp>
#include <borealis/label.hpp>
#include <borealis/view.hpp>

namespace brls
{

// A screen similar to the "The software has closed" dialog
// pressing OK will exit the app
class CrashFrame : public View
{
  private:
    Label* label;
    Button* button;
    Hint* hint;

  public:
    CrashFrame(std::string text);

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    void onShowAnimationEnd() override;
    View* getDefaultFocus() override;

    bool isTranslucent() override
    {
        return true; // have it always translucent to disable fade out animation
    }

    ~CrashFrame();
};

} // namespace brls
