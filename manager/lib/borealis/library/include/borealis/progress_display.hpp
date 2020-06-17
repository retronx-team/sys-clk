/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  Billy Laws
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

#include <borealis/label.hpp>
#include <borealis/progress_spinner.hpp>
#include <borealis/view.hpp>

namespace brls
{

// TODO: Add the "ProgressDisplayFlags_" prefix to the members
enum ProgressDisplayFlags
{
    SPINNER    = 1u << 0,
    PERCENTAGE = 1u << 1
};

inline constexpr ProgressDisplayFlags DEFAULT_PROGRESS_DISPLAY_FLAGS = (ProgressDisplayFlags)(ProgressDisplayFlags::SPINNER | ProgressDisplayFlags::PERCENTAGE);

// A progress bar with an optional spinner and percentage text.
class ProgressDisplay : public View
{
  public:
    ProgressDisplay(ProgressDisplayFlags progressFlags = DEFAULT_PROGRESS_DISPLAY_FLAGS);
    ~ProgressDisplay();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;

    void setProgress(int current, int max);

  private:
    float progressPercentage = 0.0f;
    Label* label;
    ProgressSpinner* spinner;
};

} // namespace brls
