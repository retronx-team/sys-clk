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

#include <borealis/applet_frame.hpp>
#include <borealis/sidebar.hpp>
#include <string>
#include <vector>

namespace brls
{

// An applet frame for implementing a stage based app with a progress display in top right
class StagedAppletFrame : public AppletFrame
{
  public:
    StagedAppletFrame();
    ~StagedAppletFrame();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;

    void addStage(View* view);
    void nextStage();
    void previousStage();

    unsigned getCurrentStage();
    unsigned getStagesCount();
    unsigned isLastStage();

  private:
    size_t currentStage = 0;
    std::vector<View*> stageViews;

    void enterStage(int index, bool requestFocus);
};

} // namespace brls
