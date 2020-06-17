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

#include <borealis/box_layout.hpp>
#include <string>
#include <vector>

namespace brls
{

// A sidebar with multiple tabs
class SidebarSeparator : public View
{
  public:
    SidebarSeparator();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
};

class Sidebar;

// TODO: Use a Label view with integrated ticker for label and sublabel
// TODO: Have the label always tick when active
class SidebarItem : public View
{
  private:
    std::string label;
    bool active = false;

    Sidebar* sidebar     = nullptr;
    View* associatedView = nullptr;

  public:
    SidebarItem(std::string label, Sidebar* sidebar);

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;

    View* getDefaultFocus() override
    {
        return this;
    }

    virtual bool onClick();

    void setActive(bool active);
    bool isActive();

    void onFocusGained() override;

    void setAssociatedView(View* view);
    View* getAssociatedView();

    ~SidebarItem();
};

// TODO: Add a style with icons, make it collapsible?
class Sidebar : public BoxLayout
{
  private:
    SidebarItem* currentActive = nullptr;

  public:
    Sidebar();

    SidebarItem* addItem(std::string label, View* view);
    void addSeparator();

    void setActive(SidebarItem* item);

    View* getDefaultFocus() override;
    void onChildFocusGained(View* child) override;

    size_t lastFocus = 0;
};

} // namespace brls
