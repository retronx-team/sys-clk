/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "main_gui.h"

#include "fatal_gui.h"
#include "app_profile_gui.h"
#include "global_override_gui.h"

void MainGui::listUI()
{
    this->enabledToggle = new tsl::elm::ToggleListItem("Enable", false);
    enabledToggle->setStateChangedListener([this](bool state) {
        Result rc = sysclkIpcSetEnabled(state);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("sysclkIpcSetEnabled", rc);
        }

        this->lastContextUpdate = armGetSystemTick();
        this->context->enabled = state;
    });
    this->listElement->addItem(this->enabledToggle);

    tsl::elm::ListItem* appProfileItem = new tsl::elm::ListItem("Edit app profile");
    appProfileItem->setClickListener([this](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A && this->context)
        {
            AppProfileGui::changeTo(this->context->applicationId);
            return true;
        }

        return false;
    });
    this->listElement->addItem(appProfileItem);

    this->listElement->addItem(new tsl::elm::CategoryHeader("Advanced"));

    tsl::elm::ListItem* globalOverrideItem = new tsl::elm::ListItem("Temporary overrides");
    globalOverrideItem->setClickListener([this](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A)
        {
            tsl::changeTo<GlobalOverrideGui>();
            return true;
        }

        return false;
    });
    this->listElement->addItem(globalOverrideItem);
}

void MainGui::refresh()
{
    BaseMenuGui::refresh();

    if(this->context)
    {
        this->enabledToggle->setState(this->context->enabled);
    }
}
