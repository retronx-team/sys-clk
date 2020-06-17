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

#include <borealis/application.hpp>
#include <borealis/logger.hpp>
#include <borealis/notification_manager.hpp>
#include <cstring>

// TODO: add a timeout duration enum for longer notifications

namespace brls
{

NotificationManager::NotificationManager()
{
    std::memset(this->notifications, 0, sizeof(Notification*) * BRLS_NOTIFICATIONS_MAX);
}

void NotificationManager::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    for (size_t i = 0; i < BRLS_NOTIFICATIONS_MAX; i++)
    {
        if (this->notifications[i])
        {
            Notification* notification = this->notifications[i];

            float alpha       = notification->getAlpha();
            float translation = 0.0f;

            if (alpha != 1.0f)
            {
                translation = (1.0f - alpha) * (float)style->Notification.slideAnimation;
                nvgTranslate(vg, translation, 0);
            }

            this->notifications[i]->frame(ctx);

            if (alpha != 1.0f)
                nvgTranslate(vg, -translation, 0);
        }
    }
}

void NotificationManager::notify(std::string text)
{
    // Find the first available notification slot
    bool found = false;
    size_t i   = 0;
    for (i = 0; i < BRLS_NOTIFICATIONS_MAX; i++)
    {
        if (!this->notifications[i])
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        brls::Logger::info("Discarding notification \"%s\"", text.c_str());
        return;
    }

    // Create the notification
    brls::Logger::info("Showing notification \"%s\"", text.c_str());

    Notification* notification = new Notification(text);
    notification->setParent(this);
    notification->show([]() {});

    // Timeout timer
    menu_timer_ctx_entry_t entry;

    entry.duration = Application::getStyle()->AnimationDuration.notificationTimeout;
    entry.tick     = [](void*) {};
    entry.userdata = nullptr;
    entry.cb       = [this, notification, i](void* userdata) {
        notification->hide([this, notification, i]() {
            delete notification;
            this->notifications[i] = nullptr;
        });
    };

    menu_timer_start(&notification->timeoutTimer, &entry);

    this->notifications[i] = notification;

    // Layout the notification
    this->layoutNotification(i);
}

void NotificationManager::layoutNotification(size_t index)
{
    Notification* notification = this->notifications[index];

    if (!notification)
        return;

    Style* style = Application::getStyle();

    // Get the position of the last notification
    Notification* lastNotification = nullptr;
    for (size_t i = 0; i < index; i++)
    {
        if (this->notifications[i])
            lastNotification = this->notifications[i];
    }

    unsigned y = lastNotification ? (lastNotification->getY() + lastNotification->getHeight()) : 0;

    // Layout the notification
    unsigned width = style->Notification.width;
    notification->setBoundaries(
        this->getX() + this->getWidth() - width,
        this->getY() + y,
        width,
        0 // height is dynamic
    );

    notification->invalidate(); // TODO: call layout directly to fix posting multiple notifications in one frame
}

void NotificationManager::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    for (size_t i = 0; i < BRLS_NOTIFICATIONS_MAX; i++)
    {
        if (this->notifications[i])
            this->layoutNotification(i);
    }
}

NotificationManager::~NotificationManager()
{
    for (size_t i = 0; i < BRLS_NOTIFICATIONS_MAX; i++)
    {
        if (this->notifications[i])
            delete this->notifications[i];
    }
}

Notification::Notification(std::string text)
{
    this->setBackground(Background::BACKDROP);

    this->label = new Label(LabelStyle::NOTIFICATION, text, true);
    label->setParent(this);
}

Notification::~Notification()
{
    delete this->label;
}

void Notification::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    this->label->frame(ctx);
}

void Notification::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    unsigned padding  = style->Notification.padding;
    unsigned fontSize = style->Label.notificationFontSize;
    float lineHeight  = style->Label.notificationLineHeight;

    // Layout the label
    this->label->setWidth(this->getWidth() - padding * 2);
    this->label->setHeight(0); // height is dynamic

    this->label->invalidate(true); // layout directly to update height

    unsigned minLabelHeight = (unsigned int)(lineHeight * fontSize) + fontSize; // 2 lines
    unsigned labelYAdvance  = padding;
    if (this->label->getHeight() < minLabelHeight)
    {
        labelYAdvance += (minLabelHeight - this->label->getHeight()) / 2;
    }

    this->label->setBoundaries(
        this->getX() + padding,
        this->getY() + labelYAdvance,
        this->label->getWidth(),
        this->label->getHeight());

    // Update our own height
    this->setHeight(std::max(
        this->label->getHeight() + padding * 2,
        minLabelHeight + padding * 2));
}

}; // namespace brls
