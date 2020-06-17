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

#pragma once

#include <borealis/applet_frame.hpp>
#include <borealis/box_layout.hpp>
#include <borealis/button.hpp>
#include <borealis/image.hpp>

namespace brls
{

// The sidebar used in ThumbnailFrame
class ThumbnailSidebar : public View
{
  private:
    Image* image   = nullptr;
    Button* button = nullptr;

    Label* title    = nullptr;
    Label* subTitle = nullptr;

  public:
    ThumbnailSidebar();
    ~ThumbnailSidebar();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    View* getDefaultFocus() override;

    void setThumbnail(std::string imagePath);
    void setThumbnail(unsigned char* buffer, size_t bufferSize);

    void setTitle(std::string title);
    void setSubtitle(std::string subTitle);

    Button* getButton();
};

// An applet frame with a sidebar on the right, containing a thumbnail
// and a button (similar to the Wi-Fi settings in HOS)
class ThumbnailFrame : public AppletFrame
{
  private:
    ThumbnailSidebar* sidebar = nullptr;
    BoxLayout* boxLayout      = nullptr;

    View* thumbnailContentView = nullptr;

  public:
    ThumbnailFrame();
    ~ThumbnailFrame();

    void setContentView(View* view) override;

  protected:
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

    ThumbnailSidebar* getSidebar();
};

} // namespace brls
