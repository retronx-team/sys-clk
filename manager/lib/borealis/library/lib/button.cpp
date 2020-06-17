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

#include <borealis/application.hpp>
#include <borealis/button.hpp>

namespace brls
{

Button::Button(ButtonStyle style)
    : style(style)
{
    this->registerAction("OK", Key::A, [this] { return this->onClick(); });
}

LabelStyle Button::getLabelStyle()
{
    if (this->style == ButtonStyle::BORDERLESS)
        return LabelStyle::BUTTON_BORDERLESS;
    else if (this->style == ButtonStyle::DIALOG)
        return LabelStyle::BUTTON_DIALOG;
    else if (this->style == ButtonStyle::CRASH)
        return LabelStyle::CRASH;

    if (this->state == ButtonState::DISABLED)
        return LabelStyle::BUTTON_PLAIN_DISABLED;
    else
        return LabelStyle::BUTTON_PLAIN;
}

Button::~Button()
{
    if (this->label != nullptr)
        delete this->label;
    if (this->image != nullptr)
        delete this->image;
}

void Button::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    unsigned imageWidth  = this->label ? this->getHeight() : this->getWidth();
    unsigned imageHeight = this->getHeight();

    if (!this->image)
        imageWidth = 0;

    if (this->label != nullptr)
    {
        this->label->setWidth(this->getWidth() - imageWidth);
        this->label->invalidate(true);
        this->label->setBoundaries(
            this->x + imageWidth,
            this->y + this->getHeight() / 2 - this->label->getHeight() / 2,
            this->label->getWidth(),
            this->label->getHeight());
        this->label->invalidate();
    }
    if (this->image != nullptr)
    {
        this->image->setHeight(imageHeight);
        this->image->setWidth(imageWidth);
        this->image->invalidate(true);
        this->image->setBoundaries(
            this->x,
            this->y + this->getHeight() / 2 - this->image->getHeight() / 2,
            this->image->getWidth(),
            this->image->getHeight());
    }
}

Button* Button::setLabel(std::string label)
{
    if (this->label != nullptr)
        delete this->label;

    this->label = new Label(this->getLabelStyle(), label, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);

    return this;
}

Button* Button::setImage(std::string path)
{
    this->image = new Image(path);
    this->image->setParent(this);
    return this;
}

Button* Button::setImage(unsigned char* buffer, size_t bufferSize)
{
    this->image = new Image(buffer, bufferSize);
    this->image->setParent(this);
    return this;
}

void Button::setState(ButtonState state)
{
    this->state = state;
    if (this->label != nullptr)
        this->label->setStyle(this->getLabelStyle());
}

ButtonState Button::getState()
{
    return this->state;
}

void Button::getHighlightInsets(unsigned* top, unsigned* right, unsigned* bottom, unsigned* left)
{
    if (this->style == ButtonStyle::DIALOG)
    {
        View::getHighlightInsets(top, right, bottom, left);
        *right -= 1;
        return;
    }

    Style* style = Application::getStyle();
    *top         = style->Button.highlightInset;
    *right       = style->Button.highlightInset;
    *bottom      = style->Button.highlightInset;
    *left        = style->Button.highlightInset;
}

void Button::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    float cornerRadius = this->cornerRadiusOverride ? this->cornerRadiusOverride : (float)style->Button.cornerRadius;

    // Background
    switch (this->style)
    {
        case ButtonStyle::PLAIN:
        {
            nvgFillColor(vg, a(this->state == ButtonState::DISABLED ? ctx->theme->buttonPlainDisabledBackgroundColor : ctx->theme->buttonPlainEnabledBackgroundColor));
            nvgBeginPath(vg);
            nvgRoundedRect(vg, x, y, width, height, cornerRadius);
            nvgFill(vg);
            break;
        }
        default:
            break;
    }

    // Shadow
    if (this->state == ButtonState::ENABLED && this->style == ButtonStyle::PLAIN)
    {
        float shadowWidth   = style->Button.shadowWidth;
        float shadowFeather = style->Button.shadowFeather;
        float shadowOpacity = style->Button.shadowOpacity;
        float shadowOffset  = style->Button.shadowOffset;

        NVGpaint shadowPaint = nvgBoxGradient(vg,
            x, y + shadowWidth,
            width, height,
            cornerRadius * 2, shadowFeather,
            RGBA(0, 0, 0, shadowOpacity * alpha), transparent);

        nvgBeginPath(vg);
        nvgRect(vg, x - shadowOffset, y - shadowOffset,
            width + shadowOffset * 2, height + shadowOffset * 3);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, shadowPaint);
        nvgFill(vg);
    }

    // Label
    if (this->label != nullptr)
        this->label->frame(ctx);

    if (this->image != nullptr)
        this->image->frame(ctx);
}

bool Button::onClick()
{
    if (this->state == ButtonState::DISABLED)
        return false;

    return this->clickEvent.fire(this);
}

GenericEvent* Button::getClickEvent()
{
    return &this->clickEvent;
}

void Button::setCornerRadius(float cornerRadius)
{
    this->cornerRadiusOverride = cornerRadius;

    if (this->image != nullptr)
        this->image->setCornerRadius(cornerRadius);
}
} // namespace brls
