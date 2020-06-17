/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv
    Copyright (C) 2019  p-sam

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

#include <borealis/logger.hpp>
#include <borealis/swkbd.hpp>
#include <cstring>
#include <iostream>

#ifdef __SWITCH__
#include <switch.h>
#endif

namespace brls
{

#ifdef __SWITCH__
static SwkbdConfig createSwkbdBaseConfig(std::string headerText, std::string subText, int maxStringLength, std::string initialText)
{
    SwkbdConfig config;

    swkbdCreate(&config, 0);

    swkbdConfigMakePresetDefault(&config);
    swkbdConfigSetHeaderText(&config, headerText.c_str());
    swkbdConfigSetSubText(&config, subText.c_str());
    swkbdConfigSetStringLenMax(&config, maxStringLength);
    swkbdConfigSetInitialText(&config, initialText.c_str());
    swkbdConfigSetStringLenMaxExt(&config, 1);
    swkbdConfigSetBlurBackground(&config, true);

    return config;
}
#else
static std::string terminalInput(std::string text)
{
    printf("\033[0;94m[INPUT] \033[0;36m%s\033[0m: ", text.c_str());
    std::string line;
    std::getline(std::cin, line);
    return line;
}
#endif

bool Swkbd::openForText(std::function<void(std::string)> f, std::string headerText, std::string subText, int maxStringLength, std::string initialText)
{
#ifdef __SWITCH__
    SwkbdConfig config = createSwkbdBaseConfig(headerText, subText, maxStringLength, initialText);

    swkbdConfigSetType(&config, SwkbdType_Normal);
    swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_At | SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

    char buffer[0x100];

    if (R_SUCCEEDED(swkbdShow(&config, buffer, 0x100)) && strcmp(buffer, "") != 0)
    {
        f(buffer);

        swkbdClose(&config);
        return true;
    }

    swkbdClose(&config);

    return false;
#else
    std::string line = terminalInput(headerText);
    f(line);
    return true;
#endif
}

bool Swkbd::openForNumber(std::function<void(int)> f, std::string headerText, std::string subText, int maxStringLength, std::string initialText, std::string leftButton, std::string rightButton)
{
#ifdef __SWITCH__
    SwkbdConfig config = createSwkbdBaseConfig(headerText, subText, maxStringLength, initialText);

    swkbdConfigSetType(&config, SwkbdType_NumPad);
    swkbdConfigSetLeftOptionalSymbolKey(&config, leftButton.c_str());
    swkbdConfigSetRightOptionalSymbolKey(&config, rightButton.c_str());
    swkbdConfigSetKeySetDisableBitmask(&config, SwkbdKeyDisableBitmask_At | SwkbdKeyDisableBitmask_Percent | SwkbdKeyDisableBitmask_ForwardSlash | SwkbdKeyDisableBitmask_Backslash);

    char buffer[0x100];

    if (R_SUCCEEDED(swkbdShow(&config, buffer, 0x100)) && strcmp(buffer, "") != 0)
    {
        f(std::stol(buffer));

        swkbdClose(&config);
        return true;
    }

    swkbdClose(&config);

    return false;
#else
    std::string line = terminalInput(headerText);

    try
    {
        f(stol(line));
        return true;
    }
    catch (const std::exception& e)
    {
        Logger::error("Could not parse input, did you enter a valid integer?");
        return false;
    }
#endif
}

} // namespace brls
