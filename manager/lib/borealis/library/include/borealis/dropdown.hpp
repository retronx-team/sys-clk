/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
    Copyright (C) 2019-2020  p-sam

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

#include <borealis/event.hpp>
#include <borealis/list.hpp>
#include <borealis/view.hpp>
#include <string>

namespace brls
{

// Fired when the user has selected a value
//
// Parameter is either the selected value index
// or -1 if the user cancelled
//
// Assume that the Dropdown is deleted
// as soon as this function is called
typedef Event<int> ValueSelectedEvent;

// Allows the user to select between multiple
// values
// Use Dropdown::open()
class Dropdown : public View
{
  private:
    Dropdown(std::string title, std::vector<std::string> values, ValueSelectedEvent::Callback cb, size_t selected = 0);

    std::string title;

    int valuesCount;

    ValueSelectedEvent valueEvent;

    List* list;
    Hint* hint;

    float topOffset; // for slide in animation

  protected:
    unsigned getShowAnimationDuration(ViewAnimation animation) override;

  public:
    ~Dropdown();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    View* getDefaultFocus() override;
    virtual bool onCancel();
    void show(std::function<void(void)> cb, bool animate = true, ViewAnimation animation = ViewAnimation::FADE) override;
    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;

    static void open(std::string title, std::vector<std::string> values, ValueSelectedEvent::Callback cb, int selected = -1);

    bool isTranslucent() override
    {
        return true || View::isTranslucent();
    }
};

} // namespace brls
