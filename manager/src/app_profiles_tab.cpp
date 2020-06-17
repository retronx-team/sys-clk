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

#include "app_profiles_tab.h"

#include "app_profile_frame.h"

#include "ipc/client.h"
#include "ipc/ipc.h"

#include <cstring>

#include "utils.h"

#define PROFILE_BADGE "\uE3E0"

AppProfilesTab::AppProfilesTab()
{
    // Filter toggle
    this->filterListItem = new brls::ToggleListItem("Show applications with no profile", this->showEmptyProfiles, "", "Yes", "No");
    filterListItem->getClickEvent()->subscribe([this](View* v)
    {
        this->refreshFilter();
    });
    this->addView(filterListItem);

    // Spacing
    this->addView(new brls::ListItemGroupSpacing());

    // Applications list
    NsApplicationRecord record;
    uint64_t tid;
    NsApplicationControlData controlData;
    NacpLanguageEntry* langEntry = NULL;

    Result rc;
    size_t i            = 0;
    int recordCount     = 0;
    size_t controlSize  = 0;

    while (true)
    {
        // Record
        rc = nsListApplicationRecord(&record, sizeof(record), i, &recordCount);
        if (R_FAILED(rc))
        {
            errorResult("nsListApplicationRecord", rc);
            break;
        }

        if(recordCount <= 0)
            break;

        tid = record.application_id;

        // Control data
        rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, &controlData, sizeof(controlData), &controlSize);
        if (R_FAILED(rc))
        {
            errorResult("nsGetApplicationControlData", rc);
            break;
        }

        // Language entry
        rc = nacpGetLanguageEntry(&controlData.nacp, &langEntry);
        if (R_FAILED(rc))
        {
            errorResult("nacpGetLanguageEntry", rc);
            break;
        }

        // Name
        if (!langEntry->name)
        {
            i++;
            continue;
        }

        Title* title = (Title*) malloc(sizeof(Title));
        title->tid = tid;

        memset(title->name, 0, sizeof(title->name));
        strncpy(title->name, langEntry->name, sizeof(title->name)-1);

        memcpy(title->icon, controlData.icon, sizeof(title->icon));

        // Profile
        rc = sysclkIpcGetProfileCount(tid, &title->profileCount);
        if (R_FAILED(rc))
        {
            errorResult("sysclkIpcGetProfileCount", rc);
            free(title);
            break;
        }

        // Add the ListItem
        brls::ListItem *listItem = new brls::ListItem(formatListItemTitle(std::string(title->name)), "", formatTid(title->tid));

        title->listItem = listItem;

        this->items.push_back(listItem);

        if (title->profileCount > 0)
        {
            listItem->setValue(PROFILE_BADGE);
            this->profilesItems.push_back(listItem);
        }
        else
        {
            this->emptyProfilesItems.push_back(listItem);

            if (!this->showEmptyProfiles)
                listItem->collapse(false);
        }

        listItem->setThumbnail(title->icon, sizeof(title->icon));

        listItem->getClickEvent()->subscribe([this, title](View* view) {
            this->editingTitle = title;
            AppProfileFrame* profileFrame = new AppProfileFrame(title);
            brls::Application::pushView(profileFrame, brls::ViewAnimation::SLIDE_LEFT);
        });

        this->addView(listItem);
        this->titles.push_back(title);

        i++;
    }

    // Empty list message
    this->emptyListLabel = new brls::Label(brls::LabelStyle::REGULAR, "", true);
    this->addView(emptyListLabel);
    this->updateEmptyListLabel(false);
}

void AppProfilesTab::updateEmptyListLabel(bool animate)
{
    if (this->items.empty())
    {
        this->emptyListLabel->setText("\uE140  You don't have any application installed on your Nintendo Switch.");
        this->emptyListLabel->show([](){}, animate);
    }
    else if (!this->showEmptyProfiles && this->profilesItems.empty())
    {
        this->emptyListLabel->setText("\uE140  You don't have any application with a defined profile at the moment.");
        this->emptyListLabel->show([](){}, animate);
    }
    else
    {
        this->emptyListLabel->hide([](){}, animate);
    }
}

AppProfilesTab::~AppProfilesTab()
{
    for (Title* title : this->titles)
        free(title);

    this->titles.clear();
}


void AppProfilesTab::willAppear(bool resetState)
{
    if (this->editingTitle != nullptr)
    {
        bool hadProfiles = this->editingTitle->profileCount > 0;

        Result rc = sysclkIpcGetProfileCount(this->editingTitle->tid, &this->editingTitle->profileCount);

        bool hasProfiles = this->editingTitle->profileCount > 0;

        if (R_FAILED(rc))
        {
            errorResult("sysclkIpcGetProfileCount", rc);
            this->editingTitle = nullptr;
            return;
        }

        // Update the profile badge
        if (hasProfiles)
            this->editingTitle->listItem->setValue(PROFILE_BADGE);
        else
            this->editingTitle->listItem->setValue("");

        // Update lists

        // Remove from emptyProfilesItems if it didn't have a profile
        // but has one now
        // Add to profilesItems
        if (!hadProfiles && hasProfiles)
        {
            this->emptyProfilesItems.erase(std::remove(this->emptyProfilesItems.begin(), this->emptyProfilesItems.end(), this->editingTitle->listItem), this->emptyProfilesItems.end());
            this->profilesItems.push_back(this->editingTitle->listItem);
        }

        // Add to emptyProfilesItems if it had a profile but doesn't
        // has one now
        // Remove from profilesItems
        if (hadProfiles && !hasProfiles)
        {
            this->emptyProfilesItems.push_back(this->editingTitle->listItem);
            this->profilesItems.erase(std::remove(this->profilesItems.begin(), this->profilesItems.end(), this->editingTitle->listItem), this->profilesItems.end());
        }

        // Refresh the filter
        this->refreshFilter();

        // Cleanup
        this->editingTitle = nullptr;
    }
}

void AppProfilesTab::refreshFilter()
{
    this->showEmptyProfiles = this->filterListItem->getToggleState();

    for (brls::ListItem *listItem : this->emptyProfilesItems)
    {
        if (this->showEmptyProfiles)
            listItem->expand();
        else
            listItem->collapse();

        this->updateEmptyListLabel();
    }
}
