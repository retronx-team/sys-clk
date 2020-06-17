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

#include <borealis/application.hpp>
#include <borealis/sidebar.hpp>

namespace brls
{

Sidebar::Sidebar()
    : BoxLayout(BoxLayoutOrientation::VERTICAL)
{
    Style* style = Application::getStyle();

    this->setWidth(style->Sidebar.width);
    this->setSpacing(style->Sidebar.spacing);
    this->setMargins(style->Sidebar.marginTop, style->Sidebar.marginRight, style->Sidebar.marginBottom, style->Sidebar.marginLeft);
    this->setBackground(Background::SIDEBAR);
}

View* Sidebar::getDefaultFocus()
{
    // Sanity check
    if (this->lastFocus >= this->children.size())
        this->lastFocus = 0;

    // Try to focus last focused one
    View* toFocus = this->children[this->lastFocus]->view->getDefaultFocus();
    if (toFocus)
        return toFocus;

    // Otherwise just get the first available item
    return BoxLayout::getDefaultFocus();
}

void Sidebar::onChildFocusGained(View* child)
{
    size_t position = *((size_t*)child->getParentUserData());

    this->lastFocus = position;

    BoxLayout::onChildFocusGained(child);
}

SidebarItem* Sidebar::addItem(std::string label, View* view)
{
    SidebarItem* item = new SidebarItem(label, this);
    item->setAssociatedView(view);

    if (this->isEmpty())
        setActive(item);

    this->addView(item);

    return item;
}

void Sidebar::addSeparator()
{
    SidebarSeparator* separator = new SidebarSeparator();
    this->addView(separator);
}

void Sidebar::setActive(SidebarItem* active)
{
    if (currentActive)
        currentActive->setActive(false);

    currentActive = active;
    active->setActive(true);
}

SidebarItem::SidebarItem(std::string label, Sidebar* sidebar)
    : label(label)
    , sidebar(sidebar)
{
    Style* style = Application::getStyle();
    this->setHeight(style->Sidebar.Item.height);

    this->registerAction("OK", Key::A, [this] { return this->onClick(); });
}

void SidebarItem::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    // Label
    nvgFillColor(vg, a(this->active ? ctx->theme->activeTabColor : ctx->theme->textColor));
    nvgFontSize(vg, style->Sidebar.Item.textSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgBeginPath(vg);
    nvgText(vg, x + style->Sidebar.Item.textOffsetX + style->Sidebar.Item.padding, y + height / 2, this->label.c_str(), nullptr);

    // Active marker
    if (this->active)
    {
        nvgFillColor(vg, a(ctx->theme->activeTabColor));
        nvgBeginPath(vg);
        nvgRect(vg, x + style->Sidebar.Item.padding, y + style->Sidebar.Item.padding, style->Sidebar.Item.activeMarkerWidth, style->Sidebar.Item.height - style->Sidebar.Item.padding * 2);
        nvgFill(vg);
    }
}

bool SidebarItem::onClick()
{
    Application::onGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, false);
    return true;
}

void SidebarItem::setActive(bool active)
{
    this->active = active;
}

SidebarSeparator::SidebarSeparator()
{
    Style* style = Application::getStyle();
    this->setHeight(style->Sidebar.Separator.height);
}

void SidebarSeparator::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    nvgFillColor(vg, a(ctx->theme->sidebarSeparatorColor));
    nvgBeginPath(vg);
    nvgRect(vg, x, y + height / 2, width, 1);
    nvgFill(vg);
}

void SidebarItem::setAssociatedView(View* view)
{
    this->associatedView = view;
}

bool SidebarItem::isActive()
{
    return this->active;
}

void SidebarItem::onFocusGained()
{
    this->sidebar->setActive(this);
    View::onFocusGained();
}

View* SidebarItem::getAssociatedView()
{
    return this->associatedView;
}

SidebarItem::~SidebarItem()
{
    if (this->associatedView)
        delete this->associatedView;
}

} // namespace brls
