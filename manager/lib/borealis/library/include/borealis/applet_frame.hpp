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
#include <borealis/hint.hpp>
#include <borealis/image.hpp>
#include <borealis/view.hpp>
#include <string>

namespace brls
{

enum class HeaderStyle
{
    REGULAR,
    POPUP // Only meant for PopupFrames. Using it in other contexts might cause weird behaviour
};

// A Horizon settings-like frame, with header and footer (no sidebar)
class AppletFrame : public View
{
  private:
    std::string title      = "";
    std::string footerText = "";

    std::string subTitleLeft = "", subTitleRight = "";

    View* icon = nullptr;
    Hint* hint = nullptr;

    View* contentView = nullptr;

    bool slideOut = false;
    bool slideIn  = false;

    ViewAnimation animation;

  protected:
    HeaderStyle headerStyle = HeaderStyle::REGULAR;

    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

    unsigned leftPadding  = 0;
    unsigned rightPadding = 0;

  public:
    AppletFrame(bool padLeft, bool padRight);

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    View* getDefaultFocus() override;
    virtual bool onCancel();
    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;
    void show(std::function<void(void)> cb, bool animate = true, ViewAnimation animation = ViewAnimation::FADE) override;
    void hide(std::function<void(void)> cb, bool animated = true, ViewAnimation animation = ViewAnimation::FADE) override;
    void onWindowSizeChanged() override;

    void setTitle(std::string title);
    void setFooterText(std::string footerText);
    void setSubtitle(std::string left, std::string right);
    void setIcon(unsigned char* buffer, size_t bufferSize);
    void setIcon(std::string imagePath);
    void setIcon(View* view);
    virtual void setContentView(View* view);
    bool hasContentView();
    void setHeaderStyle(HeaderStyle headerStyle);

    void setAnimateHint(bool animate)
    {
        this->hint->setAnimate(animate);
    }

    ~AppletFrame();
};

} // namespace brls
