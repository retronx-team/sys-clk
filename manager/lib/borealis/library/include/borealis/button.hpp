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

#include <borealis/image.hpp>
#include <borealis/label.hpp>
#include <borealis/view.hpp>

namespace brls
{

enum class ButtonStyle
{
    PLAIN = 0, // regular, plain button
    BORDERED, // text and a border
    BORDERLESS, // only text
    CRASH, // same as borderless but with a different text color
    DIALOG // same as borderless but with a different text color
};

enum class ButtonState
{
    ENABLED = 0,
    DISABLED
};

// A button
class Button : public View
{
  private:
    ButtonStyle style;
    Label* label = nullptr;
    Image* image = nullptr;

    GenericEvent clickEvent;

    LabelStyle getLabelStyle();

    ButtonState state = ButtonState::ENABLED;

    float cornerRadiusOverride = 0;

  public:
    Button(ButtonStyle style);
    ~Button();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    virtual bool onClick();
    void layout(NVGcontext* vg, Style* style, FontStash* stash);
    void getHighlightInsets(unsigned* top, unsigned* right, unsigned* bottom, unsigned* left) override;

    ButtonState getState();
    void setState(ButtonState state);

    Button* setLabel(std::string label);
    Button* setImage(std::string path);
    Button* setImage(unsigned char* buffer, size_t bufferSize);

    GenericEvent* getClickEvent();

    View* getDefaultFocus() override
    {
        return this;
    }

    void setCornerRadius(float cornerRadius);

    void getHighlightMetrics(Style* style, float* cornerRadius) override
    {
        if (cornerRadiusOverride)
            *cornerRadius = cornerRadiusOverride;
        else
            *cornerRadius = style->Button.cornerRadius;
    }

    bool isHighlightBackgroundEnabled() override
    {
        return false;
    }
};

} // namespace brls
