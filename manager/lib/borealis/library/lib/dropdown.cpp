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

#include <borealis/animations.hpp>
#include <borealis/application.hpp>
#include <borealis/dropdown.hpp>
#include <borealis/logger.hpp>

#define SELECT_VIEW_MAX_ITEMS 6 // for max height

#define min(a, b) ((a < b) ? a : b)

// TODO: Turns out the fade out animation is the same as the fade in (top -> bottom)

namespace brls
{

Dropdown::Dropdown(std::string title, std::vector<std::string> values, ValueSelectedEvent::Callback cb, size_t selected)
    : title(title)
{
    Style* style = Application::getStyle();

    this->valueEvent.subscribe(cb);

    this->topOffset = (float)style->Dropdown.listPadding / 8.0f;

    this->valuesCount = values.size();

    this->list = new List(selected);
    this->list->setParent(this);
    this->list->setMargins(1, 0, 1, 0);

    for (size_t i = 0; i < values.size(); i++)
    {
        std::string value = values[i];

        ListItem* item = new ListItem(value);

        if (i == selected)
            item->setChecked(true);

        item->setHeight(style->Dropdown.listItemHeight);
        item->setTextSize(style->Dropdown.listItemTextSize);

        item->getClickEvent()->subscribe([this, i](View* view) {
            this->valueEvent.fire(i);
            Application::popView();
        });

        this->list->addView(item);
    }

    this->hint = new Hint();
    this->hint->setParent(this);

    this->registerAction("Back", Key::B, [this] { return this->onCancel(); });
}

void Dropdown::show(std::function<void(void)> cb, bool animate, ViewAnimation animation)
{
    View::show(cb);

    menu_animation_ctx_entry_t entry;

    entry.duration     = this->getShowAnimationDuration(animation);
    entry.easing_enum  = EASING_OUT_QUAD;
    entry.subject      = &this->topOffset;
    entry.tag          = (uintptr_t) nullptr;
    entry.target_value = 0.0f;
    entry.tick         = [this](void* userdata) { this->invalidate(); };
    entry.userdata     = nullptr;

    menu_animation_push(&entry);
}

void Dropdown::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    unsigned top = this->list->getY() - style->Dropdown.headerHeight - style->Dropdown.listPadding;

    // Backdrop
    nvgFillColor(vg, a(ctx->theme->dropdownBackgroundColor));
    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, top);
    nvgFill(vg);

    // TODO: Shadow

    // Background
    nvgFillColor(vg, a(ctx->theme->sidebarColor));
    nvgBeginPath(vg);
    nvgRect(vg, x, top, width, height - top);
    nvgFill(vg);

    // List
    this->list->frame(ctx);

    // Footer
    this->hint->frame(ctx);

    nvgFillColor(vg, ctx->theme->separatorColor); // we purposely don't apply opacity

    nvgBeginPath(vg);
    nvgRect(vg, x + style->AppletFrame.separatorSpacing, y + height - style->AppletFrame.footerHeight, width - style->AppletFrame.separatorSpacing * 2, 1);
    nvgFill(vg);

    nvgFillColor(vg, ctx->theme->textColor); // we purposely don't apply opacity
    nvgFontSize(vg, style->AppletFrame.footerTextSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgBeginPath(vg);
    nvgText(vg, x + style->AppletFrame.separatorSpacing + style->AppletFrame.footerTextSpacing, y + height - style->AppletFrame.footerHeight / 2, Application::getCommonFooter()->c_str(), nullptr);

    // Header
    nvgFillColor(vg, a(ctx->theme->separatorColor));
    nvgBeginPath(vg);
    nvgRect(vg, x + style->AppletFrame.separatorSpacing, top + style->Dropdown.headerHeight - 1, width - style->AppletFrame.separatorSpacing * 2, 1);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillColor(vg, a(ctx->theme->textColor));
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgFontSize(vg, style->Dropdown.headerFontSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + style->Dropdown.headerPadding, top + style->Dropdown.headerHeight / 2, this->title.c_str(), nullptr);
}

bool Dropdown::onCancel()
{
    this->valueEvent.fire(-1);
    Application::popView();
    return true;
}

unsigned Dropdown::getShowAnimationDuration(ViewAnimation animation)
{
    return View::getShowAnimationDuration(animation) / 2;
}

void Dropdown::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    // Layout and move the list
    unsigned listHeight = min(SELECT_VIEW_MAX_ITEMS, this->valuesCount) * style->Dropdown.listItemHeight - (unsigned)this->topOffset;
    unsigned listWidth  = style->Dropdown.listWidth + style->List.marginLeftRight * 2;

    this->list->setBoundaries(
        this->width / 2 - listWidth / 2,
        this->height - style->AppletFrame.footerHeight - listHeight - style->Dropdown.listPadding + (unsigned)this->topOffset,
        listWidth,
        listHeight);
    this->list->invalidate(true); // call layout directly to update scrolling

    // Hint
    // TODO: convert the bottom-left footer into a Label to get its width and avoid clipping with the hint
    unsigned hintWidth = this->width - style->AppletFrame.separatorSpacing * 2 - style->AppletFrame.footerTextSpacing * 2;

    this->hint->setBoundaries(
        this->x + this->width - hintWidth - style->AppletFrame.separatorSpacing - style->AppletFrame.footerTextSpacing,
        this->y + this->height - style->AppletFrame.footerHeight,
        hintWidth,
        style->AppletFrame.footerHeight);
    this->hint->invalidate();
}

View* Dropdown::getDefaultFocus()
{
    return this->list->getDefaultFocus();
}

void Dropdown::open(std::string title, std::vector<std::string> values, ValueSelectedEvent::Callback cb, int selected)
{
    Dropdown* dropdown = new Dropdown(title, values, cb, selected);
    Application::pushView(dropdown);
}

void Dropdown::willAppear(bool resetState)
{
    if (this->list)
        this->list->willAppear(resetState);

    if (this->hint)
        this->hint->willAppear(resetState);
}

void Dropdown::willDisappear(bool resetState)
{
    if (this->list)
        this->list->willDisappear(resetState);

    if (this->hint)
        this->hint->willDisappear(resetState);
}

Dropdown::~Dropdown()
{
    delete this->list;
    delete this->hint;
}

} // namespace brls
