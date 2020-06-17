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

#include <borealis/applet_frame.hpp>
#include <borealis/sidebar.hpp>
#include <string>
#include <vector>

namespace brls
{

// An applet frame containing a sidebar on the left with multiple tabs
class TabFrame : public AppletFrame
{
  public:
    TabFrame();

    /**
     * Adds a tab with given label and view
     * All tabs and separators must be added
     * before the TabFrame is itself added to
     * the view hierarchy
     */
    void addTab(std::string label, View* view);
    void addSeparator();

    View* getDefaultFocus() override;

    virtual bool onCancel() override;

    ~TabFrame();

  private:
    Sidebar* sidebar;
    BoxLayout* layout;
    View* rightPane = nullptr;

    void switchToView(View* view);
};

} // namespace brls
