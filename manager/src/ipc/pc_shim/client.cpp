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

#include "client.h"

#include <string.h>
#include <stdlib.h>

static SysClkShimServer* g_server = NULL;

bool sysclkIpcRunning()
{
    return true;
}

Result sysclkIpcInitialize()
{
    if(!g_server)
    {
        g_server = new SysClkShimServer();

        g_server->SetContextApplicationId(0x0100000000001000ULL);
        g_server->SetContextHz(SysClkModule_CPU, 1020000000);
        g_server->SetContextHz(SysClkModule_GPU, 307200000);
        g_server->SetContextHz(SysClkModule_MEM, 1065600000);
        g_server->SetContextTemp(SysClkThermalSensor_PCB, 45700);
        g_server->SetContextTemp(SysClkThermalSensor_SOC, 48200);
        g_server->SetContextEnabled(true);

        g_server->SetProfile(0x010000000000F002, SysClkModule_CPU, SysClkProfile_Docked, 1224);
        g_server->SetProfile(0x010000000000F002, SysClkModule_GPU, SysClkProfile_Docked, 921);
        g_server->SetProfile(0x010000000000F002, SysClkModule_MEM, SysClkProfile_Docked, 1600);

        g_server->SetProfile(0x010000000000F002, SysClkModule_CPU, SysClkProfile_Handheld, 1224);
        g_server->SetProfile(0x010000000000F002, SysClkModule_MEM, SysClkProfile_Handheld, 1600);

        g_server->SetConfigValue(SysClkConfigValue_CsvWriteIntervalMs, 5000);

        g_server->AddFreq(SysClkModule_MEM, 665600000);
        g_server->AddFreq(SysClkModule_MEM, 800000000);
        g_server->AddFreq(SysClkModule_MEM, 1065600000);
        g_server->AddFreq(SysClkModule_MEM, 1331200000);
        g_server->AddFreq(SysClkModule_MEM, 1600000000);

        g_server->AddFreq(SysClkModule_CPU, 612000000);
        g_server->AddFreq(SysClkModule_CPU, 714000000);
        g_server->AddFreq(SysClkModule_CPU, 816000000);
        g_server->AddFreq(SysClkModule_CPU, 918000000);
        g_server->AddFreq(SysClkModule_CPU, 1020000000);
        g_server->AddFreq(SysClkModule_CPU, 1122000000);
        g_server->AddFreq(SysClkModule_CPU, 1224000000);
        g_server->AddFreq(SysClkModule_CPU, 1326000000);
        g_server->AddFreq(SysClkModule_CPU, 1428000000);
        g_server->AddFreq(SysClkModule_CPU, 1581000000);
        g_server->AddFreq(SysClkModule_CPU, 1683000000);
        g_server->AddFreq(SysClkModule_CPU, 1785000000);

        g_server->AddFreq(SysClkModule_GPU, 76800000);
        g_server->AddFreq(SysClkModule_GPU, 153600000);
        g_server->AddFreq(SysClkModule_GPU, 230400000);
        g_server->AddFreq(SysClkModule_GPU, 307200000);
        g_server->AddFreq(SysClkModule_GPU, 384000000);
        g_server->AddFreq(SysClkModule_GPU, 460800000);
        g_server->AddFreq(SysClkModule_GPU, 537600000);
        g_server->AddFreq(SysClkModule_GPU, 614400000);
        g_server->AddFreq(SysClkModule_GPU, 691200000);
        g_server->AddFreq(SysClkModule_GPU, 768000000);
        g_server->AddFreq(SysClkModule_GPU, 844800000);
        g_server->AddFreq(SysClkModule_GPU, 921600000);
    }

    return 0;
}

void sysclkIpcExit()
{
    if(g_server)
        delete g_server;
}

Result sysclkIpcGetAPIVersion(u32* out_ver)
{
    *out_ver = SYSCLK_IPC_API_VERSION;
    return 0;
}

Result sysclkIpcGetVersionString(char* out, size_t len)
{
    strncpy(out, "- shim server -", len-1);
    return 0;
}

Result sysclkIpcGetCurrentContext(SysClkContext* out_context)
{
    g_server->CopyContext(out_context);
    return 0;
}

Result sysclkIpcGetProfileCount(u64 tid, u8* out_count)
{
    *out_count = g_server->CountProfiles(tid);
    return 0;
}

Result sysclkIpcGetProfiles(u64 tid, SysClkTitleProfileList* out_profiles)
{
    g_server->GetProfiles(tid, out_profiles);
    return 0;
}

Result sysclkIpcSetProfiles(u64 tid, SysClkTitleProfileList* profiles)
{
    g_server->SetProfiles(tid, profiles);
    return 0;
}

Result sysclkIpcSetEnabled(bool enabled)
{
    g_server->SetContextEnabled(enabled);

    if(enabled)
    {
        g_server->SetContextTemp(SysClkThermalSensor_PCB, 45700);
        g_server->SetContextTemp(SysClkThermalSensor_SOC, 48200);
    }
    else
    {
        g_server->SetContextTemp(SysClkThermalSensor_PCB, 34200);
        g_server->SetContextTemp(SysClkThermalSensor_SOC, 42800);
    }

    return 0;
}

Result sysclkIpcExitCmd()
{
    return 0;
}

Result sysclkIpcSetOverride(SysClkModule module, u32 hz)
{
    g_server->SetContextOverride(module, hz);
    return 0;
}

Result sysclkIpcGetConfigValues(SysClkConfigValueList* out_configValues)
{
    g_server->GetConfigValues(out_configValues);
    return 0;
}

Result sysclkIpcSetConfigValues(SysClkConfigValueList* configValues)
{
    g_server->SetConfigValues(configValues);
    return 0;
}


Result sysclkIpcGetFreqList(SysClkModule module, u32* list, u32 maxCount, u32* outCount)
{
    g_server->GetFreqList(module, list, maxCount, outCount);
    return 0;
}

SysClkShimServer::SysClkShimServer()
{
    this->store = std::map<std::tuple<u64, SysClkModule, SysClkProfile>, u32>();
    this->SetContextApplicationId(0);
    this->SetContextHz(SysClkModule_CPU, 0);
    this->SetContextHz(SysClkModule_GPU, 0);
    this->SetContextHz(SysClkModule_MEM, 0);
    this->SetContextRealHz(SysClkModule_CPU, 0);
    this->SetContextRealHz(SysClkModule_GPU, 0);
    this->SetContextRealHz(SysClkModule_MEM, 0);
    this->SetContextOverride(SysClkModule_CPU, 0);
    this->SetContextOverride(SysClkModule_GPU, 0);
    this->SetContextOverride(SysClkModule_MEM, 0);
    this->SetContextTemp(SysClkThermalSensor_PCB, 0);
    this->SetContextTemp(SysClkThermalSensor_SOC, 0);
    this->SetContextProfile(SysClkProfile_Handheld);
    this->SetContextEnabled(false);

    for(int kval = 0; kval < SysClkConfigValue_EnumMax; kval++)
    {
        this->configValues[kval] = sysclkDefaultConfigValue((SysClkConfigValue)kval);
    }
}

void SysClkShimServer::SetContextApplicationId(u64 tid)
{
    this->context.applicationId = tid;
}

void SysClkShimServer::SetContextHz(SysClkModule module, u32 hz)
{
    if(module < SysClkModule_EnumMax)
    {
        this->context.freqs[module] = hz;
    }
}

void SysClkShimServer::SetContextRealHz(SysClkModule module, u32 hz)
{
    if(module < SysClkModule_EnumMax)
    {
        this->context.realFreqs[module] = hz;
    }
}

void SysClkShimServer::SetContextTemp(SysClkThermalSensor sensor, u32 temp)
{
    if(sensor < SysClkThermalSensor_EnumMax)
    {
        this->context.temps[sensor] = temp;
    }
}

void SysClkShimServer::CopyContext(SysClkContext* out_context)
{
    memcpy(out_context, &this->context, sizeof(SysClkContext));
}

void SysClkShimServer::SetContextProfile(SysClkProfile profile)
{
    if(profile < SysClkProfile_EnumMax)
    {
        this->context.profile = profile;
    }
}

void SysClkShimServer::SetContextEnabled(bool enabled)
{
    this->context.enabled = enabled;
}

void SysClkShimServer::SetProfile(uint64_t applicationId, SysClkModule module, SysClkProfile profile, u32 mhz)
{
    std::tuple<u64, SysClkModule, SysClkProfile> key = std::make_tuple(applicationId, module, profile);

    if(mhz > 0)
    {
        this->store[key] = mhz;
    }
    else
    {
        this->store.erase(key);
    }
}

u32 SysClkShimServer::GetProfileMHz(uint64_t applicationId, SysClkModule module, SysClkProfile profile)
{
    std::tuple<u64, SysClkModule, SysClkProfile> key = std::make_tuple(applicationId, module, profile);
    std::map<std::tuple<u64, SysClkModule, SysClkProfile>, u32>::iterator it = this->store.find(key);

    if(it != this->store.end())
    {
        return it->second;
    }
    return 0;
}

void SysClkShimServer::GetProfiles(u64 applicationId, SysClkTitleProfileList* out_profiles)
{
    for(unsigned int profile = 0; profile < SysClkProfile_EnumMax; profile++)
    {
        for(unsigned int module = 0; module < SysClkModule_EnumMax; module++)
        {
            out_profiles->mhzMap[profile][module] = this->GetProfileMHz(applicationId, (SysClkModule)module, (SysClkProfile)profile);
        }
    }
}

void SysClkShimServer::SetProfiles(u64 applicationId, SysClkTitleProfileList* profiles)
{
    for(unsigned int profile = 0; profile < SysClkProfile_EnumMax; profile++)
    {
        for(unsigned int module = 0; module < SysClkModule_EnumMax; module++)
        {
            this->SetProfile(applicationId, (SysClkModule)module, (SysClkProfile)profile, profiles->mhzMap[profile][module]);
        }
    }
}

u8 SysClkShimServer::CountProfiles(u64 applicationId)
{
    return std::accumulate(
        std::begin(this->store),
        std::end(this->store),
        0,
        [applicationId] (u8 value, const std::map<std::tuple<u64, SysClkModule, SysClkProfile>, u32>::value_type& p)
        {
            if(std::get<0>(p.first) == applicationId)
            {
                value++;
            }
            return value;
        }
    );
}

void SysClkShimServer::SetContextOverride(SysClkModule module, u32 hz)
{
    if(SYSCLK_ENUM_VALID(SysClkModule, module))
    {
        this->context.overrideFreqs[module] = hz;
    }
}

u64 SysClkShimServer::GetConfigValue(SysClkConfigValue kval)
{
    if(SYSCLK_ENUM_VALID(SysClkConfigValue, kval))
    {
        return this->configValues[kval];
    }
    return 0;
}

void SysClkShimServer::SetConfigValue(SysClkConfigValue kval, u64 val)
{
    if(SYSCLK_ENUM_VALID(SysClkConfigValue, kval))
    {
        if(sysclkValidConfigValue(kval, this->configValues[kval]))
        {
            this->configValues[kval] = val;
        }
        else
        {
            this->configValues[kval] = sysclkDefaultConfigValue(kval);
        }
    }
}

void SysClkShimServer::GetConfigValues(SysClkConfigValueList* out_configValues)
{
    for(unsigned int kval = 0; kval < SysClkConfigValue_EnumMax; kval++)
    {
        out_configValues->values[kval] = this->GetConfigValue((SysClkConfigValue)kval);
    }
}

void SysClkShimServer::SetConfigValues(SysClkConfigValueList* configValues)
{
    for(unsigned int kval = 0; kval < SysClkConfigValue_EnumMax; kval++)
    {
        this->SetConfigValue((SysClkConfigValue)kval, configValues->values[kval]);
    }
}

void SysClkShimServer::AddFreq(SysClkModule module, u32 hz)
{
    if(SYSCLK_ENUM_VALID(SysClkModule, module))
    {
        this->freqs[module].push_back(hz);
    }
}


void SysClkShimServer::GetFreqList(SysClkModule module, u32* list, u32 maxCount, u32* outCount)
{
    u32 count = 0;

    if(SYSCLK_ENUM_VALID(SysClkModule, module))
    {
        std::vector<u32>::iterator iter = this->freqs[module].begin();

        while(iter < this->freqs[module].end() && *outCount < maxCount)
        {
            *list = *iter;
            list++;
            iter++;
            count++;
        }
    }

    if(outCount)
    {
        *outCount = count;
    }
}