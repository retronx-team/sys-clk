/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

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

#include <borealis/animations.hpp>
#include <borealis/label.hpp>
#include <borealis/view.hpp>

#define BRLS_NOTIFICATIONS_MAX 8

// TODO: check in HOS that the animation duration + notification timeout are correct

namespace brls
{

class Notification : public View
{
  public:
    Notification(std::string text);
    ~Notification();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

    menu_timer_t timeoutTimer;

  private:
    Label* label;
};

class NotificationManager : public View
{
  private:
    Notification* notifications[BRLS_NOTIFICATIONS_MAX];

    void layoutNotification(size_t i);

  public:
    NotificationManager();
    ~NotificationManager();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

    void notify(std::string text);
};

}; // namespace brls
