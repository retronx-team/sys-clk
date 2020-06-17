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

#pragma once

#include <borealis/box_layout.hpp>
#include <borealis/view.hpp>

namespace brls
{

// TODO: Add a "can be cancelled with B" flag
// TODO: Add buttons at creation time
// TODO: Add the blurred dialog type once the blur is finished

class DialogButton
{
  public:
    std::string label;
    GenericEvent::Callback cb;
};

// A modal dialog with zero to three buttons
// and anything as content
// Create the dialog then use open() and close()
class Dialog : public View
{
  private:
    View* contentView = nullptr;

    unsigned frameX, frameY, frameWidth, frameHeight;

    std::vector<DialogButton*> buttons;
    BoxLayout* verticalButtonsLayout   = nullptr;
    BoxLayout* horizontalButtonsLayout = nullptr;

    void rebuildButtons();

    unsigned getButtonsHeight();

    bool cancelable = true;

  public:
    Dialog(std::string text);
    Dialog(View* contentView);
    ~Dialog();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    View* getDefaultFocus() override;
    virtual bool onCancel();

    /**
     * Adds a button to this dialog, with a maximum of three
     * The position depends on the add order
     *
     * Adding a button after the dialog has been opened is
     * NOT SUPPORTED
     */
    void addButton(std::string label, GenericEvent::Callback cb);

    /**
     * A cancelable dialog is closed when
     * the user presses B (defaults to true)
     *
     * A dialog without any buttons cannot
     * be cancelable
     */
    void setCancelable(bool cancelable);

    void open();
    void close(std::function<void(void)> cb = []() {});

    bool isTranslucent() override
    {
        return true;
    }
};

} // namespace brls
