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

#pragma once

#include <functional>
#include <string>

namespace brls
{

class Swkbd
{
  public:
    static bool openForText(std::function<void(std::string)> f, std::string headerText = "", std::string subText = "", int maxStringLength = 32, std::string initialText = "");
    static bool openForNumber(std::function<void(int)> f, std::string headerText = "", std::string subText = "", int maxStringLength = 32, std::string initialText = "", std::string leftButton = "", std::string rightButton = "");
};

} // namespace brls
