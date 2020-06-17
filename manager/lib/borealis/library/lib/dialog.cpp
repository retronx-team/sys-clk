/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala

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
#include <borealis/button.hpp>
#include <borealis/dialog.hpp>

// TODO: different open animation?

namespace brls
{

Dialog::Dialog(View* contentView)
    : contentView(contentView)
{
    if (contentView)
        contentView->setParent(this);

    this->registerAction("Back", Key::B, [this] { return this->onCancel(); });
}

Dialog::Dialog(std::string text)
    : Dialog(new Label(LabelStyle::DIALOG, text, true))
{
}

void Dialog::addButton(std::string label, GenericEvent::Callback cb)
{
    if (this->buttons.size() >= 3)
        return;

    DialogButton* button = new DialogButton();
    button->label        = label;
    button->cb           = cb;

    this->buttons.push_back(button);

    this->rebuildButtons();
    this->invalidate();
}

void Dialog::open()
{
    Application::pushView(this);

    if (this->buttons.size() == 0)
        Application::blockInputs();
}

// TODO: do something better in case another view was pushed in the meantime
void Dialog::close(std::function<void(void)> cb)
{
    Application::popView(ViewAnimation::FADE, cb);

    if (this->buttons.size() == 0)
        Application::unblockInputs();
}

void Dialog::setCancelable(bool cancelable)
{
    this->cancelable = cancelable;
}

void Dialog::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    // Backdrop
    nvgFillColor(vg, a(ctx->theme->dialogBackdrop));
    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, height);
    nvgFill(vg);

    // Shadow
    float shadowWidth   = style->Dialog.shadowWidth;
    float shadowFeather = style->Dialog.shadowFeather;
    float shadowOpacity = style->Dialog.shadowOpacity;
    float shadowOffset  = style->Dialog.shadowOffset;

    NVGpaint shadowPaint = nvgBoxGradient(vg,
        this->frameX, this->frameY + shadowWidth,
        this->frameWidth, this->frameHeight,
        style->Dialog.cornerRadius * 2, shadowFeather,
        RGBA(0, 0, 0, shadowOpacity * alpha), transparent);

    nvgBeginPath(vg);
    nvgRect(vg, this->frameX - shadowOffset, this->frameY - shadowOffset,
        this->frameWidth + shadowOffset * 2, this->frameHeight + shadowOffset * 3);
    nvgRoundedRect(vg, this->frameX, this->frameY, this->frameWidth, this->frameHeight, style->Dialog.cornerRadius);
    nvgPathWinding(vg, NVG_HOLE);
    nvgFillPaint(vg, shadowPaint);
    nvgFill(vg);

    // Frame
    nvgFillColor(vg, a(ctx->theme->dialogColor));
    nvgBeginPath(vg);
    nvgRoundedRect(vg, this->frameX, this->frameY, this->frameWidth, this->frameHeight, style->Dialog.cornerRadius);
    nvgFill(vg);

    // Content view
    if (this->contentView)
        this->contentView->frame(ctx);

    // Buttons separator
    if (this->buttons.size() > 0)
    {
        unsigned buttonsHeight = this->getButtonsHeight();
        nvgFillColor(vg, a(ctx->theme->dialogButtonSeparatorColor));

        // First vertical separator
        nvgBeginPath(vg);
        nvgRect(vg, this->frameX, this->frameY + this->frameHeight - buttonsHeight, this->frameWidth, style->Dialog.buttonSeparatorHeight);
        nvgFill(vg);

        // Second vertical separator
        if (this->buttons.size() == 3)
        {
            nvgBeginPath(vg);
            nvgRect(vg, this->frameX, this->frameY + this->frameHeight - style->Dialog.buttonHeight, this->frameWidth, style->Dialog.buttonSeparatorHeight);
            nvgFill(vg);
        }

        // Horizontal separator
        if (this->buttons.size() >= 2)
        {
            nvgBeginPath(vg);
            nvgRect(
                vg,
                this->frameX + this->frameWidth / 2 + style->Dialog.buttonSeparatorHeight / 2,
                this->frameY + this->frameHeight - style->Dialog.buttonHeight + 1, // offset by 1 to fix aliasing artifact
                style->Dialog.buttonSeparatorHeight,
                style->Dialog.buttonHeight - 1);
            nvgFill(vg);
        }
    }

    // Buttons
    if (this->verticalButtonsLayout)
        this->verticalButtonsLayout->frame(ctx);
}

View* Dialog::getDefaultFocus()
{
    if (this->buttons.size() > 0 && this->verticalButtonsLayout)
        return this->verticalButtonsLayout->getDefaultFocus();

    return nullptr;
}

bool Dialog::onCancel()
{
    if (this->cancelable)
        this->close();

    return this->cancelable;
}

unsigned Dialog::getButtonsHeight()
{
    Style* style = Application::getStyle();
    if (this->buttons.size() == 3)
        return style->Dialog.buttonHeight * 2;
    else if (this->buttons.size() > 0) // 1 or 2
        return style->Dialog.buttonHeight;
    else
        return 0;
}

void Dialog::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    this->frameWidth  = style->Dialog.width;
    this->frameHeight = style->Dialog.height;

    unsigned buttonsHeight = this->getButtonsHeight();
    this->frameHeight += buttonsHeight;

    this->frameX = getWidth() / 2 - this->frameWidth / 2;
    this->frameY = getHeight() / 2 - this->frameHeight / 2;

    unsigned contentX      = this->frameX + style->Dialog.paddingLeftRight;
    unsigned contentY      = this->frameY + style->Dialog.paddingTopBottom;
    unsigned contentWidth  = this->frameWidth - style->Dialog.paddingLeftRight * 2;
    unsigned contentHeight = this->frameHeight - style->Dialog.paddingTopBottom * 2 - buttonsHeight;

    if (this->contentView)
    {
        // First layout to get height
        this->contentView->setBoundaries(
            contentX,
            contentY,
            contentWidth,
            contentHeight);

        this->contentView->invalidate(true); // layout directly to get height

        // Center the content view in the dialog
        // or resize it if needed
        unsigned newContentHeight = this->contentView->getHeight();

        int difference = contentHeight - newContentHeight;

        if (difference < 0)
        {
            this->frameHeight += -difference;
        }
        else
        {
            contentY += difference / 2;

            this->contentView->setBoundaries(
                contentX,
                contentY,
                contentWidth,
                contentHeight);

            this->contentView->invalidate();
        }
    }

    // Buttons
    if (this->verticalButtonsLayout)
    {
        this->verticalButtonsLayout->setBoundaries(
            this->frameX,
            this->frameY + this->frameHeight - buttonsHeight,
            this->frameWidth,
            style->Dialog.buttonHeight);

        // Only one big button
        if (this->buttons.size() == 1)
        {
            this->verticalButtonsLayout->getChild(0)->setHeight(style->Dialog.buttonHeight);
        }
        // Two buttons on one row
        else if (this->buttons.size() == 2)
        {
            this->horizontalButtonsLayout->setHeight(style->Dialog.buttonHeight);

            this->horizontalButtonsLayout->getChild(0)->setWidth(this->frameWidth / 2);
            this->horizontalButtonsLayout->getChild(1)->setWidth(this->frameWidth / 2);
        }
        // Two rows: one with one button and one with two
        else if (this->buttons.size() == 3)
        {
            this->verticalButtonsLayout->getChild(0)->setHeight(style->Dialog.buttonHeight);

            this->horizontalButtonsLayout->setHeight(style->Dialog.buttonHeight);

            this->horizontalButtonsLayout->getChild(0)->setWidth(this->frameWidth / 2);
            this->horizontalButtonsLayout->getChild(1)->setWidth(this->frameWidth / 2);
        }

        this->verticalButtonsLayout->invalidate();
        if (this->horizontalButtonsLayout)
            this->horizontalButtonsLayout->invalidate();
    }
}

void Dialog::rebuildButtons()
{
    if (this->verticalButtonsLayout)
        delete this->verticalButtonsLayout;
    this->verticalButtonsLayout = nullptr;

    // horizontal box layout will be deleted by
    // the vertical layout destructor

    if (this->buttons.size() > 0)
    {
        this->verticalButtonsLayout = new BoxLayout(BoxLayoutOrientation::VERTICAL);
        this->verticalButtonsLayout->setParent(this);

        // Only one big button
        if (this->buttons.size() == 1)
        {
            Button* button = (new Button(ButtonStyle::DIALOG))->setLabel(this->buttons[0]->label);
            button->getClickEvent()->subscribe(this->buttons[0]->cb);
            this->verticalButtonsLayout->addView(button);
        }
        // Two buttons on one row
        else if (this->buttons.size() == 2)
        {
            this->horizontalButtonsLayout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
            this->verticalButtonsLayout->addView(this->horizontalButtonsLayout);

            for (DialogButton* dialogButton : this->buttons)
            {
                Button* button = (new Button(ButtonStyle::DIALOG))->setLabel(dialogButton->label);
                button->getClickEvent()->subscribe(dialogButton->cb);
                this->horizontalButtonsLayout->addView(button);
            }
        }
        // Two rows: one with one button and one with two
        else if (this->buttons.size() == 3)
        {
            Button* button = (new Button(ButtonStyle::DIALOG))->setLabel(this->buttons[0]->label);
            button->getClickEvent()->subscribe(this->buttons[0]->cb);
            this->verticalButtonsLayout->addView(button);

            this->horizontalButtonsLayout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
            this->verticalButtonsLayout->addView(this->horizontalButtonsLayout);

            for (size_t i = 1; i < this->buttons.size(); i++)
            {
                DialogButton* dialogButton = this->buttons[i];
                Button* button             = (new Button(ButtonStyle::DIALOG))->setLabel(dialogButton->label);
                button->getClickEvent()->subscribe(dialogButton->cb);
                this->horizontalButtonsLayout->addView(button);
            }
        }
    }
}

Dialog::~Dialog()
{
    if (this->contentView)
        delete this->contentView;

    if (this->verticalButtonsLayout)
        delete this->verticalButtonsLayout;

    for (DialogButton* dialogButton : this->buttons)
        delete dialogButton;

    // horizontal box layout will be deleted by
    // the vertical layout destructor
}

} // namespace brls
