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

void errorResult(std::string tag, Result rc)
{
#ifdef __SWITCH__
    brls::Logger::error("[0x%x] %s failed - %04d-%04d", rc, tag.c_str(), R_MODULE(rc), R_DESCRIPTION(rc));
#else
    brls::Logger::error("[0x%x] %s failed - xxxx-xxxx", rc, tag.c_str());
#endif
}

// TODO: Merge ticker for single line labels in Borealis and remove usage of this
std::string formatListItemTitle(const std::string str, size_t maxScore) {
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

brls::SelectListItem* createFreqListItem(SysClkModule module, uint32_t selectedFreqInMhz, std::string defaultString)
{
    std::string name;
    uint32_t* table;

    switch (module)
    {
        case SysClkModule_CPU:
            name = "CPU Frequency";
            table = sysclk_g_freq_table_cpu_hz;
            break;
        case SysClkModule_GPU:
            name = "GPU Frequency";
            table = sysclk_g_freq_table_gpu_hz;
            break;
        case SysClkModule_MEM:
            name = "MEM Frequency";
            table = sysclk_g_freq_table_mem_hz;
            break;
        default:
            return nullptr;
    }

    size_t selected = 0;
    size_t i        = 0;

    std::vector<std::string> clocks;

    clocks.push_back(defaultString);

    while (table[i] != 0)
    {
        uint32_t freq = table[i];

        if (freq / 1000000 == selectedFreqInMhz)
            selected = i + 1;

        char clock[16];
        snprintf(clock, sizeof(clock), "%d MHz", freq / 1000000);

        clocks.push_back(std::string(clock));

        i++;
    }

    return new brls::SelectListItem(name, clocks, selected);
}
