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

#include "ns.h"
#include "types.h"

#include <string.h>
#include <stdlib.h>

#define GAME_ICON_PATH "src/ipc/pc_shim/game.jpg"

static NsShimStore* g_store = NULL;

Result nsInitialize()
{
    if(!g_store)
    {
        g_store = new NsShimStore();
        g_store->AddRecord(0x010000000000F001ULL, "Secret Maryo Chronicles", GAME_ICON_PATH);
        g_store->AddRecord(0x010000000000F002ULL, "Zelda: Mystery of Solarus", GAME_ICON_PATH);
        g_store->AddRecord(0x010000000000F003ULL, "AM2R", GAME_ICON_PATH);
        g_store->AddRecord(0x010000000000F004ULL, "Mother 4", GAME_ICON_PATH);
        g_store->AddRecord(0x01007EF00011E000ULL, "Wrath of the Wild", GAME_ICON_PATH);
    }
    return 0;
}

void nsExit()
{
    if(g_store)
        delete g_store;
}

Result nsListApplicationRecord(NsApplicationRecord* out_records, s32 size, s32 offset, s32* out_count)
{
    return g_store->ListRecord(out_records, size, offset, out_count) ? 0 : 0xBAD;
}

Result nsGetApplicationControlData(NsApplicationControlSource flag, u64 titleID, NsApplicationControlData* out_data, size_t size, u64* out_size)
{
    return g_store->GetControlData(out_data, size, titleID, out_size) ? 0 : 0xBAD;
}

NsShimStore::NsShimStore()
{
    this->store = std::map<u64, std::pair<std::string, std::string>>();
}

void NsShimStore::AddRecord(u64 titleID, std::string name, std::string iconPath)
{
    this->store[titleID] = std::make_pair(name, iconPath);
}

bool NsShimStore::ListRecord(NsApplicationRecord* out_records, s32 size, s32 offset, s32* out_count) 
{
    if(!out_records || !out_count)
    {
        return false;
    }

    size_t left = size;
    size_t count = 0;
    std::map<u64, std::pair<std::string, std::string>>::iterator it = this->store.begin();
    std::advance(it, offset);
    while(left >= sizeof(NsApplicationRecord) && it != this->store.end())
    {
        out_records->application_id = it->first;
        left -= sizeof(NsApplicationRecord);
        out_records++;
        count++;
        std::advance(it, 1);
    }

    *out_count = count;

    return true;
}

bool NsShimStore::GetControlData(NsApplicationControlData* out_data, size_t size, u64 tid, size_t* out_size)
{
    if(!out_data || !out_size || size != sizeof(NsApplicationControlData))
    {
        return false;
    }

    std::map<u64, std::pair<std::string, std::string>>::iterator it = this->store.find(tid);
    if(it == this->store.end())
    {
        return false;
    }

    memset(out_data, 0, size);
    strncpy(out_data->nacp.entry.name, it->second.first.c_str(), sizeof(out_data->nacp.entry.name)-1);
    FILE* f = fopen(it->second.second.c_str(), "rb");
    if(!f)
    {
        printf("[NsShimStore] Could not open file '%s'\n", it->second.second.c_str());
        return false;
    }
    *out_size = fread(out_data->icon, sizeof(out_data->icon), 1, f);
    fclose(f);
    return true;
}
