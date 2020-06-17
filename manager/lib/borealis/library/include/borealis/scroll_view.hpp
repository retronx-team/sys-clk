/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  natinusala

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

#include <borealis/view.hpp>

namespace brls
{

// TODO: horizontal scrolling, either in ScrollView or in a separate class (like Android has)

// A view that automatically scrolls vertically
// when one of its children gains focus
class ScrollView : public View
{
  private:
    View* contentView = nullptr;

    bool ready = false; // has layout been called at least once?

    unsigned middleY = 0; // y + height/2
    unsigned bottomY = 0; // y + height

    float scrollY = 0.0f; // from 0.0f to 1.0f, in % of content view height

    bool updateScrollingOnNextLayout = false;
    bool updateScrollingOnNextFrame  = false;

    unsigned getYCenter(View* view);

    void prebakeScrolling();
    bool updateScrolling(bool animated);
    void startScrolling(bool animated, float newScroll);
    void scrollAnimationTick();

  public:
    ~ScrollView();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;
    View* getDefaultFocus() override;
    void onChildFocusGained(View* child) override;
    void onWindowSizeChanged() override;

    void setContentView(View* view);
    View* getContentView();
};

} // namespace brls
