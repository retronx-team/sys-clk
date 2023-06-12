/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "freq_choice_gui.h"

#include "../format.h"
#include "fatal_gui.h"

FreqChoiceGui::FreqChoiceGui(std::uint32_t selectedHz, std::uint32_t* hzList, std::uint32_t hzCount, FreqChoiceListener listener)
{
    this->selectedHz = selectedHz;
    this->hzList = hzList;
    this->hzCount = hzCount;
    this->listener = listener;
}

tsl::elm::ListItem* FreqChoiceGui::createFreqListItem(std::uint32_t hz, bool selected)
{
    tsl::elm::ListItem* listItem = new tsl::elm::ListItem(formatListFreqHz(hz));
    listItem->setValue(selected ? "\uE14B" : "");

    listItem->setClickListener([this, hz](u64 keys) {
        if((keys & HidNpadButton_A) == HidNpadButton_A && this->listener)
        {
            if(this->listener(hz))
            {
                tsl::goBack();
            }
            return true;
        }

        return false;
    });

    return listItem;
}

void FreqChoiceGui::listUI()
{
    this->listElement->addItem(this->createFreqListItem(0, this->selectedHz == 0));
    for(std::uint32_t i = 0; i < this->hzCount; i++) {
        std::uint32_t hz = this->hzList[i];
        this->listElement->addItem(this->createFreqListItem(hz, (hz / 1000000) == (this->selectedHz / 1000000)));
    }
}
