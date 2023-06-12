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

#include "app_profile_frame.h"

#include <borealis.hpp>

#include "utils.h"

#include "ipc/client.h"

#include <cstring>

AppProfileFrame::AppProfileFrame(Title* title) : ThumbnailFrame(), title(title)
{
    this->setTitle("Edit application profile");
    this->setIcon(new brls::MaterialIcon("\uE315"));

    // Get the freqs
    Result rc = sysclkIpcGetProfiles(title->tid, &this->profiles);

    if (R_FAILED(rc))
        errorResult("sysclkIpcGetProfiles", rc);

    // Setup the right sidebar
    this->getSidebar()->setThumbnail(title->icon, sizeof(title->icon));
    this->getSidebar()->setTitle(std::string(title->name));
    this->getSidebar()->setSubtitle(formatTid(title->tid));
    this->getSidebar()->getButton()->setState(brls::ButtonState::DISABLED);

    this->getSidebar()->getButton()->getClickEvent()->subscribe([this, title](brls::View* view)
    {
        SysClkTitleProfileList profiles;
        memcpy(&profiles, &this->profiles, sizeof(SysClkTitleProfileList));

        for (int p = 0; p < SysClkProfile_EnumMax; p++)
        {
            for (int m = 0; m < SysClkModule_EnumMax; m++)
                profiles.mhzMap[p][m] /= 1000000;
        }

        Result rc = sysclkIpcSetProfiles(title->tid, &profiles);

        if (R_SUCCEEDED(rc))
        {
            // TODO: set the tick mark color to blue/green once borealis has rich text support
            brls::Application::notify("\uE14B Profile saved");
            brls::Application::popView(brls::ViewAnimation::SLIDE_RIGHT);
        }
        else
        {
            errorResult("sysclkIpcSetProfiles", rc);
            brls::Application::notify("An error occured while saving the profile - see logs for more details");
        }
    });

    // Setup the list
    brls::List* list = new brls::List();

    this->addFreqs(list, SysClkProfile_Docked);
    this->addFreqs(list, SysClkProfile_Handheld);

    this->addFreqs(list, SysClkProfile_HandheldCharging);
    this->addFreqs(list, SysClkProfile_HandheldChargingOfficial);
    this->addFreqs(list, SysClkProfile_HandheldChargingUSB);

    this->setContentView(list);
}

void AppProfileFrame::addFreqs(brls::List* list, SysClkProfile profile)
{
    // Get the freqs
    list->addView(new brls::Header(std::string(sysclkFormatProfile(profile, true))));

    // CPU
    brls::SelectListItem* cpuListItem = createFreqListItem(SysClkModule_CPU, this->profiles.mhzMap[profile][SysClkModule_CPU]);

    this->profiles.mhzMap[profile][SysClkModule_CPU] *= 1000000;

    cpuListItem->getValueSelectedEvent()->subscribe([this, profile](int result) {
        this->onProfileChanged();
        this->profiles.mhzMap[profile][SysClkModule_CPU] = result == 0 ? result : g_freq_table_hz[SysClkModule_CPU][result];

        brls::Logger::debug("Caching freq for module %d and profile %d to %" PRIu32, SysClkModule_CPU, profile, this->profiles.mhzMap[profile][SysClkModule_CPU]);
    });
    list->addView(cpuListItem);

    // GPU
    brls::SelectListItem* gpuListItem = createFreqListItem(SysClkModule_GPU, this->profiles.mhzMap[profile][SysClkModule_GPU]);

    this->profiles.mhzMap[profile][SysClkModule_GPU] *= 1000000;

    gpuListItem->getValueSelectedEvent()->subscribe([this, profile](int result) {
        this->onProfileChanged();
        this->profiles.mhzMap[profile][SysClkModule_GPU] = result == 0 ? result : g_freq_table_hz[SysClkModule_GPU][result];

        brls::Logger::debug("Caching freq for module %d and profile %d to %" PRIu32, SysClkModule_GPU, profile, this->profiles.mhzMap[profile][SysClkModule_GPU]);
    });
    list->addView(gpuListItem);

    // MEM
    brls::SelectListItem* memListItem = createFreqListItem(SysClkModule_MEM, this->profiles.mhzMap[profile][SysClkModule_MEM]);

    this->profiles.mhzMap[profile][SysClkModule_MEM] *= 1000000;

    memListItem->getValueSelectedEvent()->subscribe([this, profile](int result) {
        this->onProfileChanged();
        this->profiles.mhzMap[profile][SysClkModule_MEM] = result == 0 ? result : g_freq_table_hz[SysClkModule_MEM][result];

        brls::Logger::debug("Caching freq for module %d and profile %d to %" PRIu32, SysClkModule_MEM, profile, this->profiles.mhzMap[profile][SysClkModule_MEM]);
    });
    list->addView(memListItem);
}

void AppProfileFrame::onProfileChanged()
{
    this->getSidebar()->getButton()->setState(brls::ButtonState::ENABLED);
    this->updateActionHint(brls::Key::B, "Cancel");
}

bool AppProfileFrame::onCancel()
{
    if (this->hasProfileChanged())
    {
        brls::Dialog* dialog = new brls::Dialog("You have unsaved changes to this profile!\nAre you sure you want to discard them?");

        dialog->addButton("No", [dialog](brls::View* view){
            dialog->close();
        });

        dialog->addButton("Yes", [dialog](brls::View* view){
            dialog->close([](){
                brls::Application::popView(brls::ViewAnimation::SLIDE_RIGHT);
            });
        });

        dialog->open();
    }
    else
    {
        brls::Application::popView(brls::ViewAnimation::SLIDE_RIGHT);
    }
    return true;
}

bool AppProfileFrame::hasProfileChanged()
{
    return this->getSidebar()->getButton()->getState() == brls::ButtonState::ENABLED;
}
