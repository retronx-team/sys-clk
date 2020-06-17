/*
    Borealis, a Nintendo Switch UI Library
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

#include <algorithm>
#include <functional>
#include <list>

namespace brls
{

// Simple observer pattern implementation
//
// Usage:
// 1. typedef your event type
// 2. create as many events as you want using that type
// 3. call subscribe on the events with your observers
// 4. call fire when you want to fire the events
//    it wil return true if at least one subscriber exists
//    for that event
template <typename... Ts>
class Event
{
  public:
    typedef std::function<void(Ts...)> Callback;
    typedef std::list<Callback> CallbacksList;
    typedef typename CallbacksList::iterator Subscription;

    Subscription subscribe(Callback cb);
    void unsubscribe(Subscription subscription);
    bool fire(Ts... args);

  private:
    CallbacksList callbacks;
};

template <typename... Ts>
typename Event<Ts...>::Subscription Event<Ts...>::subscribe(Event<Ts...>::Callback cb)
{
    this->callbacks.push_back(cb);
    return --this->callbacks.end();
}

template <typename... Ts>
void Event<Ts...>::unsubscribe(Event<Ts...>::Subscription subscription)
{
    this->callbacks.erase(subscription);
}

template <typename... Ts>
bool Event<Ts...>::fire(Ts... args)
{
    for (Callback cb : this->callbacks)
        cb(args...);

    return !this->callbacks.empty();
}

}; // namespace brls
