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

#include "utils.h"

#include <borealis.hpp>

#include "ipc/client.h"

uint32_t g_freq_table_hz[SysClkModule_EnumMax][SYSCLK_FREQ_LIST_MAX+1];

Result cacheFreqList()
{
    Result rc;
    for(uint32_t i = 0; i < SysClkModule_EnumMax; i++)
    {
        rc = sysclkIpcGetFreqList((SysClkModule)i, &g_freq_table_hz[i][1], SYSCLK_FREQ_LIST_MAX, &g_freq_table_hz[i][0]);
        if(R_FAILED(rc))
        {
            return rc;
        }
    }

    return 0;
}

std::string formatFreq(uint32_t freq)
{
    char str[16];
    snprintf(str, sizeof(str), "%.1f MHz", (float)freq / 1000000.0f);
    return std::string(str);
}

std::string formatTid(uint64_t tid)
{
    char str[17];
    snprintf(str, sizeof(str), "%016lX", tid);
    return std::string(str);
}

std::string formatProfile(SysClkProfile profile)
{
    return std::string(sysclkFormatProfile(profile, true));
}

std::string formatTemp(uint32_t temp)
{
    char str[16];
    snprintf(str, sizeof(str), "%.1f °C", (float)temp / 1000.0f);
    return std::string(str);
}

std::string formatPower(int32_t power)
{
    char str[16];
    snprintf(str, sizeof(str), "%d mW", power);
    return std::string(str);
}

void errorResult(std::string tag, Result rc)
{
#ifdef __SWITCH__
    brls::Logger::error("[0x%x] %s failed - %04d-%04d", rc, tag.c_str(), R_MODULE(rc), R_DESCRIPTION(rc));
#else
    brls::Logger::error("[0x%x] %s failed - xxxx-xxxx", rc, tag.c_str());
#endif
}

// TODO: Merge ticker for single line labels in Borealis and remove usage of this
std::string formatListItemTitle(const std::string str, size_t maxScore)
{
    size_t score = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        score += std::isupper(str[i]) ? 4 : 3;
        if(score > maxScore)
        {
            return str.substr(0, i-1) + "\u2026";
        }
    }

    return str;
}

brls::SelectListItem* createFreqListItem(SysClkModule module, uint32_t selectedFreqInMHz, std::string defaultString)
{
    std::string name;

    switch (module)
    {
        case SysClkModule_CPU:
            name = "CPU Frequency";
            break;
        case SysClkModule_GPU:
            name = "GPU Frequency";
            break;
        case SysClkModule_MEM:
            name = "MEM Frequency";
            break;
        default:
            return nullptr;
    }

    uint32_t* table = &g_freq_table_hz[module][0];
    size_t selected = 0;
    size_t i        = 1;

    std::vector<std::string> clocks;

    clocks.push_back(defaultString);

    while (i <= table[0])
    {
        uint32_t freq = table[i];

        if (freq / 1000000 == selectedFreqInMHz)
            selected = i;

        char clock[16];
        snprintf(clock, sizeof(clock), "%d MHz", freq / 1000000);

        clocks.push_back(std::string(clock));

        i++;
    }

    return new brls::SelectListItem(name, clocks, selected);
}
