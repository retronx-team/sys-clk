/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  WerWolv
    Copyright (C) 2020  natinusala

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
#include <borealis/label.hpp>
#include <borealis/view.hpp>

namespace brls
{

// Displays button hints for the currently focused view
// Depending on the view's available actions
// there can only be one Hint visible at any time
class Hint : public BoxLayout
{
  private:
    bool animate;

    GenericEvent::Subscription globalFocusEventSubscriptor;
    VoidEvent::Subscription globalHintsUpdateEventSubscriptor;

    static inline std::vector<Hint*> globalHintStack;

    static void pushHint(Hint* hint);
    static void popHint(Hint* hint);
    static void animateHints();

    static std::string getKeyIcon(Key key);

    void rebuildHints();

  public:
    Hint(bool animate = true);
    ~Hint();

    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;

    void setAnimate(bool animate)
    {
        this->animate = animate;
    }
};

} // namespace brls
