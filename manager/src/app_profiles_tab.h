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

#include "ipc/ipc.h"

typedef struct
{
    uint64_t tid;
    NsApplicationName name;
    NsApplicationIcon icon;
    uint8_t profileCount;

    brls::ListItem* listItem;
} Title;

class AppProfilesTab : public brls::List
{
    private:
        bool showEmptyProfiles = true;

        std::vector<Title*> titles;

        std::vector<brls::ListItem*> items;
        std::vector<brls::ListItem*> emptyProfilesItems;
        std::vector<brls::ListItem*> profilesItems;

        brls::Label *emptyListLabel;
        brls::ToggleListItem *filterListItem;

        void updateEmptyListLabel(bool animate = true);

        Title* editingTitle = nullptr;

        void refreshFilter();

    public:
        AppProfilesTab();
        ~AppProfilesTab();

        void willAppear(bool resetState) override;
};
