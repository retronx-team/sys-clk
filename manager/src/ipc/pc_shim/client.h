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

#include <map>
#include <vector>
#include <numeric>
#include "../client.h"

class SysClkShimServer
{
    public:
        SysClkShimServer();
        void SetContextApplicationId(u64 tid);
        void SetContextHz(SysClkModule module, u32 hz);
        void SetContextRealHz(SysClkModule module, u32 hz);
        void SetContextTemp(SysClkThermalSensor sensor, u32 temp);
        void SetContextProfile(SysClkProfile profile);
        void SetContextEnabled(bool enabled);
        void SetContextOverride(SysClkModule module, u32 hz);
        void CopyContext(SysClkContext* out_context);
        void SetProfile(uint64_t applicationId, SysClkModule module, SysClkProfile profile, u32 mhz);
        u32 GetProfileMHz(uint64_t applicationId, SysClkModule module, SysClkProfile profile);
        void GetProfiles(u64 applicationId, SysClkTitleProfileList* out_profiles);
        void SetProfiles(u64 applicationId, SysClkTitleProfileList* profiles);
        u8 CountProfiles(u64 applicationId);
        u64 GetConfigValue(SysClkConfigValue kval);
        void SetConfigValue(SysClkConfigValue kval, u64 val);
        void GetConfigValues(SysClkConfigValueList* out_configValues);
        void SetConfigValues(SysClkConfigValueList* configValues);
        void AddFreq(SysClkModule module, u32 hz);
        void GetFreqList(SysClkModule module, u32* list, u32 maxCount, u32* outCount);

    protected:
        SysClkContext context;
        std::vector<u32> freqs[SysClkModule_EnumMax];
        std::map<std::tuple<u64, SysClkModule, SysClkProfile>, u32> store;
        u64 configValues[SysClkConfigValue_EnumMax];
};
