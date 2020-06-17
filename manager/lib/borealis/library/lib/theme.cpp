/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
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

#include <borealis/theme.hpp>

#define DARK theme.colors[ThemeVariant_DARK]
#define LIGHT theme.colors[ThemeVariant_LIGHT]

namespace brls
{

// Default colors
Theme Theme::horizon()
{
    Theme theme = Theme();

    // Light variant
    LIGHT.backgroundColor[0] = 0.922f;
    LIGHT.backgroundColor[1] = 0.922f;
    LIGHT.backgroundColor[2] = 0.922f;
    LIGHT.backgroundColorRGB = nvgRGB(235, 235, 235);

    LIGHT.textColor        = nvgRGB(51, 51, 51);
    LIGHT.descriptionColor = nvgRGB(140, 140, 140);

    LIGHT.notificationTextColor = nvgRGB(255, 255, 255);
    LIGHT.backdropColor         = nvgRGBA(0, 0, 0, 178);

    LIGHT.separatorColor = nvgRGB(45, 45, 45);

    LIGHT.sidebarColor          = nvgRGB(240, 240, 240);
    LIGHT.activeTabColor        = nvgRGB(49, 79, 235);
    LIGHT.sidebarSeparatorColor = nvgRGB(208, 208, 208);

    LIGHT.highlightBackgroundColor = nvgRGB(252, 255, 248);
    LIGHT.highlightColor1          = nvgRGB(13, 182, 213);
    LIGHT.highlightColor2          = nvgRGB(80, 239, 217);

    LIGHT.listItemSeparatorColor  = nvgRGB(207, 207, 207);
    LIGHT.listItemValueColor      = nvgRGB(43, 81, 226);
    LIGHT.listItemFaintValueColor = nvgRGB(181, 184, 191);

    LIGHT.tableEvenBackgroundColor = nvgRGB(240, 240, 240);
    LIGHT.tableBodyTextColor       = nvgRGB(131, 131, 131);

    LIGHT.dropdownBackgroundColor = nvgRGBA(0, 0, 0, 178);

    LIGHT.nextStageBulletColor = nvgRGB(165, 165, 165);

    LIGHT.spinnerBarColor = nvgRGBA(131, 131, 131, 102);

    LIGHT.headerRectangleColor = nvgRGB(127, 127, 127);

    LIGHT.buttonPlainEnabledBackgroundColor  = nvgRGB(50, 79, 241);
    LIGHT.buttonPlainDisabledBackgroundColor = nvgRGB(201, 201, 209);
    LIGHT.buttonPlainEnabledTextColor        = nvgRGB(255, 255, 255);
    LIGHT.buttonPlainDisabledTextColor       = nvgRGB(220, 220, 228);

    LIGHT.dialogColor                = nvgRGB(240, 240, 240);
    LIGHT.dialogBackdrop             = nvgRGBA(0, 0, 0, 100);
    LIGHT.dialogButtonColor          = nvgRGB(46, 78, 255);
    LIGHT.dialogButtonSeparatorColor = nvgRGB(210, 210, 210);

    // Dark variant
    DARK.backgroundColor[0] = 0.176f;
    DARK.backgroundColor[1] = 0.176f;
    DARK.backgroundColor[2] = 0.176f;
    DARK.backgroundColorRGB = nvgRGB(45, 45, 45);

    DARK.textColor        = nvgRGB(255, 255, 255);
    DARK.descriptionColor = nvgRGB(163, 163, 163);

    DARK.notificationTextColor = nvgRGB(255, 255, 255);
    DARK.backdropColor         = nvgRGBA(0, 0, 0, 178);

    DARK.separatorColor = nvgRGB(255, 255, 255);

    DARK.sidebarColor          = nvgRGB(50, 50, 50);
    DARK.activeTabColor        = nvgRGB(0, 255, 204);
    DARK.sidebarSeparatorColor = nvgRGB(81, 81, 81);

    DARK.highlightBackgroundColor = nvgRGB(31, 34, 39);
    DARK.highlightColor1          = nvgRGB(25, 138, 198);
    DARK.highlightColor2          = nvgRGB(137, 241, 242);

    DARK.listItemSeparatorColor  = nvgRGB(78, 78, 78);
    DARK.listItemValueColor      = nvgRGB(88, 195, 169);
    DARK.listItemFaintValueColor = nvgRGB(93, 103, 105);

    DARK.tableEvenBackgroundColor = nvgRGB(57, 58, 60);
    DARK.tableBodyTextColor       = nvgRGB(155, 157, 156);

    DARK.dropdownBackgroundColor = nvgRGBA(0, 0, 0, 178); // TODO: 178 may be too much for dark theme

    DARK.nextStageBulletColor = nvgRGB(165, 165, 165);

    DARK.spinnerBarColor = nvgRGBA(131, 131, 131, 102); // TODO: get this right

    DARK.headerRectangleColor = nvgRGB(160, 160, 160);

    DARK.buttonPlainEnabledBackgroundColor  = nvgRGB(1, 255, 201);
    DARK.buttonPlainDisabledBackgroundColor = nvgRGB(83, 87, 86);
    DARK.buttonPlainEnabledTextColor        = nvgRGB(52, 41, 55);
    DARK.buttonPlainDisabledTextColor       = nvgRGB(71, 75, 74);

    DARK.dialogColor                = nvgRGB(70, 70, 70);
    DARK.dialogBackdrop             = nvgRGBA(0, 0, 0, 100);
    DARK.dialogButtonColor          = nvgRGB(3, 251, 199);
    DARK.dialogButtonSeparatorColor = nvgRGB(103, 103, 103);

    return theme;
}

} // namespace brls
