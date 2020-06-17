/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  WerWolv
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
#include <borealis/list.hpp>
#include <borealis/view.hpp>
#include <string>

namespace brls
{

class PopupFrame : public View
{
  private:
    PopupFrame(std::string title, unsigned char* imageBuffer, size_t imageBufferSize, AppletFrame* contentView, std::string subTitleLeft = "", std::string subTitleRight = "");
    PopupFrame(std::string title, std::string imagePath, AppletFrame* contentView, std::string subTitleLeft = "", std::string subTitleRight = "");
    PopupFrame(std::string title, AppletFrame* contentView, std::string subTitleLeft = "", std::string subTitleRight = "");

    AppletFrame* contentView = nullptr;

  protected:
    unsigned getShowAnimationDuration(ViewAnimation animation) override;

  public:
    ~PopupFrame();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    View* getDefaultFocus() override;
    virtual bool onCancel();
    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;

    static void open(std::string title, unsigned char* imageBuffer, size_t imageBufferSize, AppletFrame* contentView, std::string subTitleLeft = "", std::string subTitleRight = "");
    static void open(std::string title, std::string imagePath, AppletFrame* contentView, std::string subTitleLeft = "", std::string subTitleRight = "");
    static void open(std::string title, AppletFrame* contentView, std::string subTitleLeft = "", std::string subTitleRight = "");

    bool isTranslucent() override
    {
        return true;
    }
};

} // namespace brls
