/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam
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

#include "refresh_task.h"

class StatusCell : public brls::View
{
    private:
        std::string label;
        std::string value;

        NVGcolor valueColor;

    public:
        StatusCell(std::string label, std::string value);

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) override;

        void setValue(std::string value);
        void setValueColor(NVGcolor color);
        void resetValueColor();
};

class InfoGrid : public brls::BoxLayout
{
    public:
        InfoGrid();

        void layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash) override;
};

class StatusGrid : public brls::BoxLayout
{
    public:
        StatusGrid();

        void layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash) override;
};

class StatusTab : public brls::List
{
    private:
        RefreshTask *refreshTask;

        FreqUpdateEvent::Subscription freqListenerSub;
        FreqUpdateEvent::Subscription realFreqListenerSub;
        AppIdUpdateEvent::Subscription appIdListenerSub;
        ProfileUpdateEvent::Subscription profileListenerSub;
        TempUpdateEvent::Subscription tempListenerSub;

        StatusCell *cpuFreqCell;
        StatusCell *gpuFreqCell;
        StatusCell *memFreqCell;

        StatusCell *realCpuFreqCell;
        StatusCell *realGpuFreqCell;
        StatusCell *realMemFreqCell;

        StatusCell *socTempCell;
        StatusCell *pcbTempCell;
        StatusCell *skinTempCell;

        StatusCell *nowPowerCell;
        StatusCell *avgPowerCell;

        StatusCell *profileCell;
        StatusCell *tidCell;

    public:
        StatusTab(RefreshTask *refreshTask);
        ~StatusTab();
};
