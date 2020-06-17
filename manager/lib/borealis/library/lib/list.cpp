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

#include <math.h>

#include <borealis/animations.hpp>
#include <borealis/application.hpp>
#include <borealis/dropdown.hpp>
#include <borealis/header.hpp>
#include <borealis/list.hpp>
#include <borealis/logger.hpp>
#include <borealis/swkbd.hpp>
#include <borealis/table.hpp>

// TODO: Scrollbar

namespace brls
{

ListContentView::ListContentView(List* list, size_t defaultFocus)
    : BoxLayout(BoxLayoutOrientation::VERTICAL, defaultFocus)
    , list(list)
{
    Style* style = Application::getStyle();
    this->setMargins(style->List.marginTopBottom, style->List.marginLeftRight, style->List.marginTopBottom, style->List.marginLeftRight);
    this->setSpacing(style->List.spacing);
    this->setRememberFocus(true);
}

void ListContentView::customSpacing(View* current, View* next, int* spacing)
{
    // Don't add spacing to the first list item
    // if it doesn't have a description and the second one is a
    // list item too
    // Or if the next item is a ListItemGroupSpacing
    if (ListItem* currentItem = dynamic_cast<ListItem*>(current))
    {
        if (currentItem->getReduceDescriptionSpacing())
        {
            if (next != nullptr)
                *spacing /= 2;
        }
        else if (ListItem* nextItem = dynamic_cast<ListItem*>(next))
        {
            if (!currentItem->hasDescription())
            {
                *spacing = 2;

                nextItem->setDrawTopSeparator(currentItem->isCollapsed());
            }
        }
        else if (dynamic_cast<ListItemGroupSpacing*>(next))
        {
            *spacing = 0;
        }
        else if (dynamic_cast<Table*>(next))
        {
            *spacing /= 2;
        }
    }
    // Table custom spacing
    else if (dynamic_cast<Table*>(current))
    {
        *spacing /= 2;
    }
    // ListItemGroupSpacing custom spacing
    else if (dynamic_cast<ListItemGroupSpacing*>(current))
    {
        *spacing /= 2;
    }
    // Header custom spacing
    else if (dynamic_cast<Header*>(current) || dynamic_cast<Header*>(next))
    {
        if (dynamic_cast<Header*>(current) && dynamic_cast<ListItem*>(next))
        {
            *spacing = 1;
        }
        else if (dynamic_cast<Label*>(current) && dynamic_cast<Header*>(next))
        {
            // Keep default spacing
        }
        else
        {
            Style* style = Application::getStyle();
            *spacing     = style->Header.padding;
        }
    }

    // Call list custom spacing
    if (this->list)
        this->list->customSpacing(current, next, spacing);
}

ListItem::ListItem(std::string label, std::string description, std::string subLabel)
    : label(label)
    , subLabel(subLabel)
{
    Style* style = Application::getStyle();

    this->setHeight(subLabel != "" ? style->List.Item.heightWithSubLabel : style->List.Item.height);
    this->setTextSize(style->Label.listItemFontSize);

    if (description != "")
    {
        this->descriptionView = new Label(LabelStyle::DESCRIPTION, description, true);
        this->descriptionView->setParent(this);
    }

    this->registerAction("OK", Key::A, [this] { return this->onClick(); });
}

void ListItem::setThumbnail(Image* image)
{
    if (this->thumbnailView)
        delete this->thumbnailView;
    if (image != NULL)
    {
        this->thumbnailView = image;
        this->thumbnailView->setParent(this);
        this->invalidate();
    }
}

void ListItem::setThumbnail(std::string imagePath)
{
    if (this->thumbnailView)
        this->thumbnailView->setImage(imagePath);
    else
        this->thumbnailView = new Image(imagePath);

    this->thumbnailView->setParent(this);
    this->thumbnailView->setScaleType(ImageScaleType::FIT);
    this->invalidate();
}

void ListItem::setThumbnail(unsigned char* buffer, size_t bufferSize)
{
    if (this->thumbnailView)
        this->thumbnailView->setImage(buffer, bufferSize);
    else
        this->thumbnailView = new Image(buffer, bufferSize);

    this->thumbnailView->setParent(this);
    this->thumbnailView->setScaleType(ImageScaleType::FIT);
    this->invalidate();
}

bool ListItem::getReduceDescriptionSpacing()
{
    return this->reduceDescriptionSpacing;
}

void ListItem::setReduceDescriptionSpacing(bool value)
{
    this->reduceDescriptionSpacing = value;
}

void ListItem::setIndented(bool indented)
{
    this->indented = indented;
}

void ListItem::setTextSize(unsigned textSize)
{
    this->textSize = textSize;
}

void ListItem::setChecked(bool checked)
{
    this->checked = checked;
}

bool ListItem::onClick()
{
    return this->clickEvent.fire(this);
}

GenericEvent* ListItem::getClickEvent()
{
    return &this->clickEvent;
}

void ListItem::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    // Description
    if (this->descriptionView)
    {
        unsigned indent = style->List.Item.descriptionIndent;

        if (this->indented)
            indent += style->List.Item.indent;

        this->height = style->List.Item.height;
        this->descriptionView->setBoundaries(this->x + indent, this->y + this->height + style->List.Item.descriptionSpacing, this->width - indent * 2, 0);
        this->descriptionView->invalidate(true); // we must call layout directly
        this->height += this->descriptionView->getHeight() + style->List.Item.descriptionSpacing;
    }

    // Thumbnail
    if (this->thumbnailView)
    {
        Style* style           = Application::getStyle();
        unsigned thumbnailSize = height - style->List.Item.thumbnailPadding * 2;

        this->thumbnailView->setBoundaries(
            x + style->List.Item.thumbnailPadding,
            y + style->List.Item.thumbnailPadding,
            thumbnailSize,
            thumbnailSize);
        this->thumbnailView->invalidate();
    }
}

void ListItem::getHighlightInsets(unsigned* top, unsigned* right, unsigned* bottom, unsigned* left)
{
    Style* style = Application::getStyle();
    View::getHighlightInsets(top, right, bottom, left);

    if (descriptionView)
        *bottom = -(descriptionView->getHeight() + style->List.Item.descriptionSpacing);

    if (indented)
        *left = -style->List.Item.indent;
}

void ListItem::resetValueAnimation()
{
    this->valueAnimation = 0.0f;

    menu_animation_ctx_tag tag = (uintptr_t) & this->valueAnimation;
    menu_animation_kill_by_tag(&tag);
}

void ListItem::setValue(std::string value, bool faint, bool animate)
{
    this->oldValue      = this->value;
    this->oldValueFaint = this->valueFaint;

    this->value      = value;
    this->valueFaint = faint;

    this->resetValueAnimation();

    if (animate && this->oldValue != "")
    {
        Style* style = Application::getStyle();

        menu_animation_ctx_tag tag = (uintptr_t) & this->valueAnimation;
        menu_animation_ctx_entry_t entry;

        entry.cb           = [this](void* userdata) { this->resetValueAnimation(); };
        entry.duration     = style->AnimationDuration.highlight;
        entry.easing_enum  = EASING_IN_OUT_QUAD;
        entry.subject      = &this->valueAnimation;
        entry.tag          = tag;
        entry.target_value = 1.0f;
        entry.tick         = [](void* userdata) {};
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    }
}

std::string ListItem::getValue()
{
    return this->value;
}

void ListItem::setDrawTopSeparator(bool draw)
{
    this->drawTopSeparator = draw;
}

View* ListItem::getDefaultFocus()
{
    if (this->collapseState != 1.0f)
        return nullptr;

    return this;
}

void ListItem::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    unsigned baseHeight = this->height;
    bool hasSubLabel    = this->subLabel != "";
    bool hasThumbnail   = this->thumbnailView;

    unsigned leftPadding = hasThumbnail ? this->thumbnailView->getWidth() + style->List.Item.thumbnailPadding * 2 : style->List.Item.padding;

    if (this->indented)
    {
        x += style->List.Item.indent;
        width -= style->List.Item.indent;
    }

    // Description
    if (this->descriptionView)
    {
        // Don't count description as part of list item
        baseHeight -= this->descriptionView->getHeight() + style->List.Item.descriptionSpacing;
        this->descriptionView->frame(ctx);
    }

    // Value
    unsigned valueX = x + width - style->List.Item.padding;
    unsigned valueY = y + (hasSubLabel ? baseHeight - baseHeight / 3 : baseHeight / 2);

    nvgTextAlign(vg, NVG_ALIGN_RIGHT | (hasSubLabel ? NVG_ALIGN_TOP : NVG_ALIGN_MIDDLE));
    nvgFontFaceId(vg, ctx->fontStash->regular);
    if (this->valueAnimation != 0.0f)
    {
        //Old value
        NVGcolor valueColor = a(this->oldValueFaint ? ctx->theme->listItemFaintValueColor : ctx->theme->listItemValueColor);
        valueColor.a *= (1.0f - this->valueAnimation);
        nvgFillColor(vg, valueColor);
        nvgFontSize(vg, style->List.Item.valueSize * (1.0f - this->valueAnimation));
        nvgBeginPath(vg);
        nvgText(vg, valueX, valueY, this->oldValue.c_str(), nullptr);

        //New value
        valueColor = a(this->valueFaint ? ctx->theme->listItemFaintValueColor : ctx->theme->listItemValueColor);
        valueColor.a *= this->valueAnimation;
        nvgFillColor(vg, valueColor);
        nvgFontSize(vg, style->List.Item.valueSize * this->valueAnimation);
        nvgBeginPath(vg);
        nvgText(vg, valueX, valueY, this->value.c_str(), nullptr);
    }
    else
    {
        nvgFillColor(vg, a(this->valueFaint ? ctx->theme->listItemFaintValueColor : ctx->theme->listItemValueColor));
        nvgFontSize(vg, style->List.Item.valueSize);
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgBeginPath(vg);
        nvgText(vg, valueX, valueY, this->value.c_str(), nullptr);
    }

    // Checked marker
    if (this->checked)
    {
        unsigned radius  = style->List.Item.selectRadius;
        unsigned centerX = x + width - radius - style->List.Item.padding;
        unsigned centerY = y + baseHeight / 2;

        float radiusf = (float)radius;

        int thickness = roundf(radiusf * 0.10f);

        // Background
        nvgFillColor(vg, a(ctx->theme->listItemValueColor));
        nvgBeginPath(vg);
        nvgCircle(vg, centerX, centerY, radiusf);
        nvgFill(vg);

        // Check mark
        nvgFillColor(vg, a(ctx->theme->backgroundColorRGB));

        // Long stroke
        nvgSave(vg);
        nvgTranslate(vg, centerX, centerY);
        nvgRotate(vg, -NVG_PI / 4.0f);

        nvgBeginPath(vg);
        nvgRect(vg, -(radiusf * 0.55f), 0, radiusf * 1.3f, thickness);
        nvgFill(vg);
        nvgRestore(vg);

        // Short stroke
        nvgSave(vg);
        nvgTranslate(vg, centerX - (radiusf * 0.65f), centerY);
        nvgRotate(vg, NVG_PI / 4.0f);

        nvgBeginPath(vg);
        nvgRect(vg, 0, -(thickness / 2), radiusf * 0.53f, thickness);
        nvgFill(vg);

        nvgRestore(vg);
    }

    // Label
    nvgFillColor(vg, a(ctx->theme->textColor));
    nvgFontSize(vg, this->textSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgBeginPath(vg);
    nvgText(vg, x + leftPadding, y + baseHeight / (hasSubLabel ? 3 : 2), this->label.c_str(), nullptr);

    // Sub Label
    if (hasSubLabel)
    {
        nvgFillColor(vg, a(ctx->theme->descriptionColor));
        nvgFontSize(vg, style->Label.descriptionFontSize);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgBeginPath(vg);
        nvgText(vg, x + leftPadding, y + baseHeight - baseHeight / 3, this->subLabel.c_str(), nullptr);
    }

    // Thumbnail
    if (hasThumbnail)
        this->thumbnailView->frame(ctx);

    // Separators
    // Offset by one to be hidden by highlight
    nvgFillColor(vg, a(ctx->theme->listItemSeparatorColor));

    // Top
    if (this->drawTopSeparator)
    {
        nvgBeginPath(vg);
        nvgRect(vg, x, y - 1, width, 1);
        nvgFill(vg);
    }

    // Bottom
    nvgBeginPath(vg);
    nvgRect(vg, x, y + 1 + baseHeight, width, 1);
    nvgFill(vg);
}

bool ListItem::hasDescription()
{
    return this->descriptionView;
}

std::string ListItem::getLabel()
{
    return this->label;
}

ListItem::~ListItem()
{
    if (this->descriptionView)
        delete this->descriptionView;

    if (this->thumbnailView)
        delete this->thumbnailView;

    this->resetValueAnimation();
}

ToggleListItem::ToggleListItem(std::string label, bool initialValue, std::string description, std::string onValue, std::string offValue)
    : ListItem(label, description)
    , toggleState(initialValue)
    , onValue(onValue)
    , offValue(offValue)
{
    this->updateValue();
}

void ToggleListItem::updateValue()
{
    if (this->toggleState)
        this->setValue(this->onValue, false);
    else
        this->setValue(this->offValue, true);
}

bool ToggleListItem::onClick()
{
    this->toggleState = !this->toggleState;
    this->updateValue();

    ListItem::onClick();
    return true;
}

bool ToggleListItem::getToggleState()
{
    return this->toggleState;
}

InputListItem::InputListItem(std::string label, std::string initialValue, std::string helpText, std::string description, int maxInputLength)
    : ListItem(label, description)
    , helpText(helpText)
    , maxInputLength(maxInputLength)
{
    this->setValue(initialValue, false);
}

bool InputListItem::onClick()
{
    Swkbd::openForText([&](std::string text) {
        this->setValue(text, false);
    },
        this->helpText, "", this->maxInputLength, this->getValue());

    ListItem::onClick();
    return true;
}

IntegerInputListItem::IntegerInputListItem(std::string label, int initialValue, std::string helpText, std::string description, int maxInputLength)
    : InputListItem(label, std::to_string(initialValue), helpText, description, maxInputLength)
{
}

bool IntegerInputListItem::onClick()
{
    Swkbd::openForNumber([&](int number) {
        this->setValue(std::to_string(number), false);
    },
        this->helpText, "", this->maxInputLength, this->getValue());

    ListItem::onClick();
    return true;
}

ListItemGroupSpacing::ListItemGroupSpacing(bool separator)
    : Rectangle(nvgRGBA(0, 0, 0, 0))
{
    ThemeValues* theme = Application::getThemeValues();

    if (separator)
        this->setColor(theme->listItemSeparatorColor);
}

SelectListItem::SelectListItem(std::string label, std::vector<std::string> values, unsigned selectedValue)
    : ListItem(label, "")
    , values(values)
    , selectedValue(selectedValue)
{
    this->setValue(values[selectedValue], false, false);

    this->getClickEvent()->subscribe([this](View* view) {
        ValueSelectedEvent::Callback valueCallback = [this](int result) {
            if (result == -1)
                return;

            this->setValue(this->values[result], false, false);
            this->selectedValue = result;

            this->valueEvent.fire(result);
        };
        Dropdown::open(this->getLabel(), this->values, valueCallback, this->selectedValue);
    });
}

void SelectListItem::setSelectedValue(unsigned value)
{
    if (value >= 0 && value < this->values.size())
    {
        this->selectedValue = value;
        this->setValue(this->values[value], false, false);
    }
}

ValueSelectedEvent* SelectListItem::getValueSelectedEvent()
{
    return &this->valueEvent;
}

List::List(size_t defaultFocus)
{
    this->layout = new ListContentView(this, defaultFocus);

    this->layout->setResize(true);
    this->layout->setParent(this);

    this->setContentView(this->layout);
}

// Wrapped BoxLayout methods

void List::addView(View* view, bool fill)
{
    this->layout->addView(view, fill);
}

void List::setMargins(unsigned top, unsigned right, unsigned bottom, unsigned left)
{
    this->layout->setMargins(
        top,
        right,
        bottom,
        left);
}

void List::setSpacing(unsigned spacing)
{
    this->layout->setSpacing(spacing);
}

unsigned List::getSpacing()
{
    return this->layout->getSpacing();
}

void List::setMarginBottom(unsigned bottom)
{
    this->layout->setMarginBottom(bottom);
}

void List::customSpacing(View* current, View* next, int* spacing)
{
    // Nothing to do by default
}

List::~List()
{
    // ScrollView already deletes the content view
}

} // namespace brls
