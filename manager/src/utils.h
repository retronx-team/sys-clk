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

#include <string>

#include <sysclk.h>
#include "ipc/ipc.h"

#include <borealis.hpp>

#define APP_ASSET(p) APP_RESOURCES p

extern uint32_t g_freq_table_hz[SysClkModule_EnumMax][SYSCLK_FREQ_LIST_MAX+1];

Result cacheFreqList();
std::string formatListItemTitle(const std::string str, size_t maxScore = 140);
brls::SelectListItem* createFreqListItem(SysClkModule module, uint32_t selectedFreqInMHz, std::string defaultString = "Do not override");

std::string formatFreq(uint32_t freq);
std::string formatTid(uint64_t tid);
std::string formatProfile(SysClkProfile profile);
std::string formatTemp(uint32_t temp);
std::string formatPower(int32_t power);

void errorResult(std::string tag, Result rc);
