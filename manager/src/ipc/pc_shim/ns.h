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
#include <string>
#include "../types.h"
#include "types.h"
#include "nacp.h"

typedef enum
{
    NsApplicationControlSource_Storage = 1
} NsApplicationControlSource;

typedef struct
{
    u64 application_id;
} NsApplicationRecord;

typedef struct
{
    NacpStruct nacp;
    NsApplicationIcon icon;
} NsApplicationControlData;

Result nsInitialize();
Result nsListApplicationRecord(NsApplicationRecord* out_records, s32 size, s32 offset, s32* out_count);
Result nsGetApplicationControlData(NsApplicationControlSource flag, u64 titleID, NsApplicationControlData* out_data, size_t size, u64* out_size);
void nsExit();

class NsShimStore
{
    public:
        NsShimStore();
        void AddRecord(u64 titleID, std::string name, std::string iconPath);
        bool ListRecord(NsApplicationRecord* out_records, s32 size, s32 offset, s32* out_count);
        bool GetControlData(NsApplicationControlData* out_data, size_t size, u64 titleID, size_t* out_size);

    protected:
        std::map<u64, std::pair<std::string, std::string>> store;
};
