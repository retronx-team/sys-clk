/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  WerWolv

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

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace brls
{

class View;

typedef std::function<bool(void)> ActionListener;

// ZL and ZR do not exist here because GLFW doesn't know them
enum class Key
{
    A      = GLFW_GAMEPAD_BUTTON_A,
    B      = GLFW_GAMEPAD_BUTTON_B,
    X      = GLFW_GAMEPAD_BUTTON_X,
    Y      = GLFW_GAMEPAD_BUTTON_Y,
    LSTICK = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
    RSTICK = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
    L      = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
    R      = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
    PLUS   = GLFW_GAMEPAD_BUTTON_START,
    MINUS  = GLFW_GAMEPAD_BUTTON_BACK,
    DLEFT  = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
    DUP    = GLFW_GAMEPAD_BUTTON_DPAD_UP,
    DRIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
    DDOWN  = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
};

struct Action
{
    Key key;

    std::string hintText;
    bool available;
    bool hidden;
    ActionListener actionListener;

    bool operator==(const Key other)
    {
        return this->key == other;
    }
};

} // namespace brls
