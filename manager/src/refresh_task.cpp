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

#include "refresh_task.h"

#include "utils.h"

#define REFRESH_INTERVAL 400

RefreshTask::RefreshTask() : RepeatingTask(REFRESH_INTERVAL)
{

}

void RefreshTask::onStart()
{
    Result rc = sysclkIpcGetCurrentContext(&this->oldContext);
    if (R_FAILED(rc))
    {
        brls::Logger::error("Unable to get context");
        errorResult("sysclkIpcGetCurrentContext", rc);
    }
}

void RefreshTask::run(retro_time_t currentTime)
{
    RepeatingTask::run(currentTime);

    // Get new context
    SysClkContext context;
    if (R_SUCCEEDED(sysclkIpcGetCurrentContext(&context)))
    {
        // CPU Freq
        if (context.freqs[SysClkModule_CPU] != this->oldContext.freqs[SysClkModule_CPU])
            this->freqUpdateEvent.fire(SysClkModule_CPU, context.freqs[SysClkModule_CPU]);

        // GPU Freq
        if (context.freqs[SysClkModule_GPU] != this->oldContext.freqs[SysClkModule_GPU])
            this->freqUpdateEvent.fire(SysClkModule_GPU, context.freqs[SysClkModule_GPU]);

        // MEM Freq
        if (context.freqs[SysClkModule_MEM] != this->oldContext.freqs[SysClkModule_MEM])
            this->freqUpdateEvent.fire(SysClkModule_MEM, context.freqs[SysClkModule_MEM]);

        // Real CPU Freq
        if (context.realFreqs[SysClkModule_CPU] != this->oldContext.realFreqs[SysClkModule_CPU])
            this->realFreqUpdateEvent.fire(SysClkModule_CPU, context.realFreqs[SysClkModule_CPU]);

        // Real GPU Freq
        if (context.realFreqs[SysClkModule_GPU] != this->oldContext.realFreqs[SysClkModule_GPU])
            this->realFreqUpdateEvent.fire(SysClkModule_GPU, context.realFreqs[SysClkModule_GPU]);

        // Real MEM Freq
        if (context.realFreqs[SysClkModule_MEM] != this->oldContext.realFreqs[SysClkModule_MEM])
            this->realFreqUpdateEvent.fire(SysClkModule_MEM, context.realFreqs[SysClkModule_MEM]);

        // Application ID
        if (context.applicationId != this->oldContext.applicationId)
            this->appIdUpdateEvent.fire(context.applicationId);

        // Profile
        if (context.profile != this->oldContext.profile)
            this->profileUpdateEvent.fire(context.profile);

        // Only notify temp changes every other tick
        if (this->shouldNotifyTempChange)
        {
            // PCB Temp
            if (context.temps[SysClkThermalSensor_PCB] != this->oldContext.temps[SysClkThermalSensor_PCB])
                this->tempUpdateEvent.fire(SysClkThermalSensor_PCB, context.temps[SysClkThermalSensor_PCB]);

            //SoC Temp
            if (context.temps[SysClkThermalSensor_SOC] != this->oldContext.temps[SysClkThermalSensor_SOC])
                this->tempUpdateEvent.fire(SysClkThermalSensor_SOC, context.temps[SysClkThermalSensor_SOC]);

            //Skin Temp
            if (context.temps[SysClkThermalSensor_Skin] != this->oldContext.temps[SysClkThermalSensor_Skin])
                this->tempUpdateEvent.fire(SysClkThermalSensor_Skin, context.temps[SysClkThermalSensor_Skin]);
        }

        this->shouldNotifyTempChange = !this->shouldNotifyTempChange;
        this->oldContext = context;
    }
    else
    {
        brls::Logger::error("Unable to refresh context");
    }
}
