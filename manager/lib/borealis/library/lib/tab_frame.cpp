/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
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

#include <borealis/application.hpp>
#include <borealis/box_layout.hpp>
#include <borealis/rectangle.hpp>
#include <borealis/sidebar.hpp>
#include <borealis/tab_frame.hpp>

namespace brls
{

TabFrame::TabFrame()
    : AppletFrame(false, true)
{
    //Create sidebar
    this->sidebar = new Sidebar();

    // Setup content view
    this->layout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
    layout->addView(sidebar);

    this->setContentView(layout);
}

bool TabFrame::onCancel()
{
    // Go back to sidebar if not already focused
    if (!this->sidebar->isChildFocused())
    {
        Application::onGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_LEFT, false);
        return true;
    }

    return AppletFrame::onCancel();
}

void TabFrame::switchToView(View* view)
{
    if (this->rightPane == view)
        return;

    if (this->layout->getViewsCount() > 1)
    {
        if (this->rightPane)
            this->rightPane->willDisappear(true);
        this->layout->removeView(1, false);
    }

    if (view != nullptr)
    {
        this->layout->addView(view, true, true); // addView() calls willAppear()
        this->rightPane = view;
    }
}

void TabFrame::addTab(std::string label, View* view)
{
    SidebarItem* item = this->sidebar->addItem(label, view);
    item->getFocusEvent()->subscribe([this](View* view) {
        if (SidebarItem* item = dynamic_cast<SidebarItem*>(view))
            this->switchToView(item->getAssociatedView());
    });

    // Switch to first one as soon as we add it
    if (!this->rightPane)
    {
        Logger::debug("Switching to the first tab");
        this->switchToView(view);
    }
}

void TabFrame::addSeparator()
{
    this->sidebar->addSeparator();
}

View* TabFrame::getDefaultFocus()
{
    // Try to focus the right pane
    if (this->layout->getViewsCount() > 1)
    {
        View* newFocus = this->rightPane->getDefaultFocus();

        if (newFocus)
            return newFocus;
    }

    // Otherwise focus sidebar
    return this->sidebar->getDefaultFocus();
}

TabFrame::~TabFrame()
{
    switchToView(nullptr);

    // Content view is freed by ~AppletFrame()
}

} // namespace brls
