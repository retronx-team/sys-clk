/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
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

namespace brls
{

class Style
{
  public:
    // AppletFrame
    struct
    {
        unsigned headerHeightRegular;
        unsigned headerHeightPopup; // PopupFrame
        unsigned footerHeight;

        unsigned imageLeftPadding;
        unsigned imageTopPadding;
        unsigned imageSize;
        unsigned separatorSpacing;

        unsigned titleSize;
        unsigned titleStart;
        unsigned titleOffset;

        unsigned footerTextSize;
        unsigned footerTextSpacing;

        unsigned slideAnimation;
    } AppletFrame;

    // Highlight
    struct
    {
        unsigned strokeWidth;
        float cornerRadius;

        unsigned shadowWidth;
        unsigned shadowOffset;
        unsigned shadowFeather;
        unsigned shadowOpacity;

        unsigned animationDuration;
    } Highlight;

    // Background
    struct
    {
        unsigned sidebarBorderHeight;
    } Background;

    // Sidebar
    struct
    {
        unsigned width;
        unsigned spacing;

        unsigned marginLeft;
        unsigned marginRight;
        unsigned marginTop;
        unsigned marginBottom;

        struct
        {
            unsigned height;
            unsigned textSize;
            unsigned padding;

            unsigned textOffsetX;
            unsigned activeMarkerWidth;

            unsigned highlight;
        } Item;

        struct
        {
            unsigned height;
        } Separator;
    } Sidebar;

    // List
    struct
    {
        unsigned marginLeftRight;
        unsigned marginTopBottom;
        unsigned spacing;

        // Item
        struct
        {
            unsigned height;
            unsigned heightWithSubLabel;
            unsigned valueSize;
            unsigned padding;
            unsigned thumbnailPadding;

            unsigned descriptionIndent;
            unsigned descriptionSpacing;

            unsigned indent;

            unsigned selectRadius;
        } Item;
    } List;

    // Label
    struct
    {
        unsigned regularFontSize;
        unsigned mediumFontSize;
        unsigned smallFontSize;
        unsigned descriptionFontSize;
        unsigned crashFontSize;
        unsigned buttonFontSize;
        unsigned listItemFontSize;
        unsigned notificationFontSize;
        unsigned dialogFontSize;
        unsigned hintFontSize;

        float lineHeight;
        float notificationLineHeight;
    } Label;

    // CrashFrame
    struct
    {
        float labelWidth; // proportional to frame width, from 0 to 1
        unsigned boxStrokeWidth;
        unsigned boxSize;
        unsigned boxSpacing;
        unsigned buttonWidth;
        unsigned buttonHeight;
        unsigned buttonSpacing;
    } CrashFrame;

    // Button
    struct
    {
        float cornerRadius;

        unsigned highlightInset;

        float shadowWidth;
        float shadowFeather;
        float shadowOpacity;
        float shadowOffset;
    } Button;

    // TableRow
    struct
    {
        unsigned headerHeight;
        unsigned headerTextSize;

        unsigned bodyHeight;
        unsigned bodyIndent;
        unsigned bodyTextSize;

        unsigned padding;
    } TableRow;

    // Dropdown
    struct
    {
        unsigned listWidth;
        unsigned listPadding;

        unsigned listItemHeight;
        unsigned listItemTextSize;

        unsigned headerHeight;
        unsigned headerFontSize;
        unsigned headerPadding;
    } Dropdown;

    struct
    {
        unsigned edgePadding;
        unsigned separatorSpacing;
        unsigned footerHeight;
        unsigned imageLeftPadding;
        unsigned imageTopPadding;
        unsigned imageSize;
        unsigned contentWidth;
        unsigned contentHeight;

        unsigned headerTextLeftPadding;
        unsigned headerTextTopPadding;

        unsigned subTitleLeftPadding;
        unsigned subTitleTopPadding;
        unsigned subTitleSpacing;

        unsigned subTitleSeparatorLeftPadding;
        unsigned subTitleSeparatorTopPadding;
        unsigned subTitleSeparatorHeight;

        unsigned headerFontSize;
        unsigned subTitleFontSize;
    } PopupFrame;

    // StagedAppletFrame
    struct
    {
        unsigned progressIndicatorSpacing;
        unsigned progressIndicatorRadiusUnselected;
        unsigned progressIndicatorRadiusSelected;
        unsigned progressIndicatorBorderWidth;
    } StagedAppletFrame;

    // ProgressSpinner
    struct
    {
        float centerGapMultiplier;
        float barWidthMultiplier;
        unsigned animationDuration;
    } ProgressSpinner;

    // ProgressDisplay
    struct
    {
        unsigned percentageLabelWidth;
    } ProgressDisplay;

    // Header
    struct
    {
        unsigned height;
        unsigned padding;

        unsigned rectangleWidth;

        unsigned fontSize;
    } Header;

    // FramerateCounter
    struct
    {
        unsigned width;
        unsigned height;
    } FramerateCounter;

    // ThumbnailSidebar
    struct
    {
        unsigned marginLeftRight;
        unsigned marginTopBottom;

        unsigned buttonHeight;
        unsigned buttonMargin;
    } ThumbnailSidebar;

    // AnimationDuration
    struct
    {
        unsigned show;
        unsigned showSlide;

        unsigned highlight;
        unsigned shake;

        unsigned collapse;

        unsigned progress;

        unsigned notificationTimeout;
    } AnimationDuration;

    // Notification
    struct
    {
        unsigned width;
        unsigned padding;

        unsigned slideAnimation;
    } Notification;

    // Dialog
    struct
    {
        unsigned width;
        unsigned height;

        unsigned paddingTopBottom;
        unsigned paddingLeftRight;

        float cornerRadius;

        unsigned buttonHeight;
        unsigned buttonSeparatorHeight;

        float shadowWidth;
        float shadowFeather;
        float shadowOpacity;
        float shadowOffset;
    } Dialog;

    // As close to HOS as possible
    static Style horizon();

    // TODO: Make a condensed style
};

} // namespace brls
