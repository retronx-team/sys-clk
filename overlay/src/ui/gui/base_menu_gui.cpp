/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "base_menu_gui.h"

#include "fatal_gui.h"

BaseMenuGui::BaseMenuGui()
{
    this->context = nullptr;
    this->lastContextUpdate = 0;
    this->listElement = nullptr;
}

BaseMenuGui::~BaseMenuGui()
{
    if(this->context)
    {
        delete this->context;
    }
}

void BaseMenuGui::preDraw(tsl::gfx::Renderer* renderer)
{
    BaseGui::preDraw(renderer);
    if(this->context)
    {
        char buf[32];
        std::uint32_t y = 85;

        renderer->drawString("App ID: ", false, 20, y, SMALL_TEXT_SIZE, DESC_COLOR);
        snprintf(buf, sizeof(buf), "%016lX", context->applicationId);
        renderer->drawString(buf, false, 81, y, SMALL_TEXT_SIZE, VALUE_COLOR);

        renderer->drawString("Profile: ", false, 246, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString(sysclkFormatProfile(context->profile, true), false, 302, y, SMALL_TEXT_SIZE, VALUE_COLOR);

        y += 30;

        static struct
        {
            SysClkModule m;
            std::uint32_t x;
        } freqOffsets[SysClkModule_EnumMax] = {
            { SysClkModule_CPU, 61 },
            { SysClkModule_GPU, 204 },
            { SysClkModule_MEM, 342 },
        };

        for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
        {
            std::uint32_t hz = this->context->freqs[freqOffsets[i].m];
            snprintf(buf, sizeof(buf), "%u.%u MHz", hz / 1000000, hz / 100000 - hz / 1000000 * 10);
            renderer->drawString(buf, false, freqOffsets[i].x, y, SMALL_TEXT_SIZE, VALUE_COLOR);
        }
        renderer->drawString("CPU:", false, 20, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("GPU:", false, 162, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("MEM:", false, 295, y, SMALL_TEXT_SIZE, DESC_COLOR);

        y += 25;

        for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
        {
            std::uint32_t hz = this->context->realFreqs[freqOffsets[i].m];
            snprintf(buf, sizeof(buf), "%u.%u MHz", hz / 1000000, hz / 100000 - hz / 1000000 * 10);
            renderer->drawString(buf, false, freqOffsets[i].x, y, SMALL_TEXT_SIZE, VALUE_COLOR);
        }

        y += 25;

        static struct
        {
            SysClkThermalSensor s;
            std::uint32_t x;
        } tempOffsets[SysClkModule_EnumMax] = {
            { SysClkThermalSensor_SOC, 61 },
            { SysClkThermalSensor_PCB, 204 },
            { SysClkThermalSensor_Skin, 342 },
        };

        renderer->drawString("SOC:", false, 20, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("PCB:", false, 166, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("Skin:", false, 303, y, SMALL_TEXT_SIZE, DESC_COLOR);
        for(unsigned int i = 0; i < SysClkModule_EnumMax; i++)
        {
            std::uint32_t millis = this->context->temps[tempOffsets[i].s];
            snprintf(buf, sizeof(buf), "%u.%u °C", millis / 1000, (millis - millis / 1000 * 1000) / 100);
            renderer->drawString(buf, false, tempOffsets[i].x, y, SMALL_TEXT_SIZE, VALUE_COLOR);
        }

        y += 30;

        static struct
        {
            SysClkPowerSensor s;
            std::uint32_t x;
        } powerOffsets[SysClkPowerSensor_EnumMax] = {
            { SysClkPowerSensor_Now, 204 },
            { SysClkPowerSensor_Avg, 342 },
        };

        renderer->drawString("Battery Power", false, 20, y, SMALL_TEXT_SIZE, DESC_COLOR);

        renderer->drawString("Now:", false, 160, y, SMALL_TEXT_SIZE, DESC_COLOR);
        renderer->drawString("Avg:", false, 304, y, SMALL_TEXT_SIZE, DESC_COLOR);
        for(unsigned int i = 0; i < SysClkPowerSensor_EnumMax; i++)
        {
            std::uint32_t mw = this->context->power[powerOffsets[i].s];
            snprintf(buf, sizeof(buf), "%d mW", mw);
            renderer->drawString(buf, false, powerOffsets[i].x, y, SMALL_TEXT_SIZE, VALUE_COLOR);
        }
    }
}

void BaseMenuGui::refresh()
{
    std::uint64_t ticks = armGetSystemTick();

    if(armTicksToNs(ticks - this->lastContextUpdate) > 500000000UL)
    {
        this->lastContextUpdate = ticks;
        if(!this->context)
        {
            this->context = new SysClkContext;
        }

        Result rc = sysclkIpcGetCurrentContext(this->context);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("sysclkIpcGetCurrentContext", rc);
            return;
        }
    }
}

tsl::elm::Element* BaseMenuGui::baseUI()
{
    tsl::elm::List* list = new tsl::elm::List();
    this->listElement = list;
    this->listUI();

    return list;
}
