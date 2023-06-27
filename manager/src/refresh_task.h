/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam
    Copyright (C) 2019  m4xw

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

#include <borealis.hpp>

#include "ipc/client.h"

#include <functional>

typedef brls::Event<SysClkModule, uint32_t> FreqUpdateEvent;
typedef brls::Event<uint64_t> AppIdUpdateEvent;
typedef brls::Event<SysClkProfile> ProfileUpdateEvent;
typedef brls::Event<SysClkThermalSensor, uint32_t> TempUpdateEvent;

class RefreshTask : public brls::RepeatingTask
{
    private:
        SysClkContext oldContext;

        FreqUpdateEvent freqUpdateEvent;
        FreqUpdateEvent realFreqUpdateEvent;
        AppIdUpdateEvent appIdUpdateEvent;
        ProfileUpdateEvent profileUpdateEvent;
        TempUpdateEvent tempUpdateEvent;

        bool shouldNotifyTempChange = true;

    public:
        RefreshTask();
        ~RefreshTask() {}

        void onStart() override;
        void run(retro_time_t currentTime) override;

        inline FreqUpdateEvent::Subscription registerFreqListener(FreqUpdateEvent::Callback cb) {
            return this->freqUpdateEvent.subscribe(cb);
        }
        inline void unregisterFreqListener(FreqUpdateEvent::Subscription subscription) {
            this->freqUpdateEvent.unsubscribe(subscription);
        }

        inline FreqUpdateEvent::Subscription registerRealFreqListener(FreqUpdateEvent::Callback cb) {
            return this->realFreqUpdateEvent.subscribe(cb);
        }
        inline void unregisterRealFreqListener(FreqUpdateEvent::Subscription subscription) {
            this->realFreqUpdateEvent.unsubscribe(subscription);
        }

        inline AppIdUpdateEvent::Subscription registerAppIdListener(AppIdUpdateEvent::Callback cb) {
            return this->appIdUpdateEvent.subscribe(cb);
        }
        inline void unregisterAppIdListener(AppIdUpdateEvent::Subscription subscription) {
            this->appIdUpdateEvent.unsubscribe(subscription);
        }

        inline ProfileUpdateEvent::Subscription registerProfileListener(ProfileUpdateEvent::Callback cb) {
            return this->profileUpdateEvent.subscribe(cb);
        }
        inline void unregisterProfileListener(ProfileUpdateEvent::Subscription subscription) {
            this->profileUpdateEvent.unsubscribe(subscription);
        }

        inline TempUpdateEvent::Subscription registerTempListener(TempUpdateEvent::Callback cb) {
            return this->tempUpdateEvent.subscribe(cb);
        }
        inline void unregisterTempListener(TempUpdateEvent::Subscription subscription) {
            this->tempUpdateEvent.unsubscribe(subscription);
        }
};
