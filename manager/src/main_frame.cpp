/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019-2020  natinusala
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

#include "main_frame.h"

#include "status_tab.h"
#include "advanced_settings_tab.h"
#include "app_profiles_tab.h"
#include "cheat_sheet_tab.h"
#include "about_tab.h"
#include "logo.h"

#include "ipc/client.h"

MainFrame::MainFrame() : TabFrame()
{
    // Start refresh task
    this->refreshTask = new RefreshTask();
    this->refreshTask->start();

    // Load UI
    this->setIcon(new Logo(LogoStyle::HEADER));

    AppProfilesTab *tab = new AppProfilesTab();

    this->addTab("Status", new StatusTab(this->refreshTask));
    this->addTab("Application Profiles", tab);
    this->addTab("Advanced Settings", new AdvancedSettingsTab());

    this->addSeparator();

    this->addTab("Cheat Sheet", new CheatSheetTab());
    this->addTab("About", new AboutTab());
}

MainFrame::~MainFrame()
{
    this->refreshTask->stop();
}
