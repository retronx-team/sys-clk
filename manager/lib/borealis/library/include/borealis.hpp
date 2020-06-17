/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam

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

#ifndef BOREALIS_RESOURCES
#error BOREALIS_RESOURCES define missing
#endif
#define BOREALIS_ASSET(_str) BOREALIS_RESOURCES _str

// Library
#include <borealis/applet_frame.hpp>
#include <borealis/application.hpp>
#include <borealis/box_layout.hpp>
#include <borealis/button.hpp>
#include <borealis/crash_frame.hpp>
#include <borealis/dialog.hpp>
#include <borealis/dropdown.hpp>
#include <borealis/event.hpp>
#include <borealis/header.hpp>
#include <borealis/image.hpp>
#include <borealis/label.hpp>
#include <borealis/layer_view.hpp>
#include <borealis/list.hpp>
#include <borealis/logger.hpp>
#include <borealis/material_icon.hpp>
#include <borealis/notification_manager.hpp>
#include <borealis/popup_frame.hpp>
#include <borealis/progress_display.hpp>
#include <borealis/progress_spinner.hpp>
#include <borealis/rectangle.hpp>
#include <borealis/repeating_task.hpp>
#include <borealis/sidebar.hpp>
#include <borealis/staged_applet_frame.hpp>
#include <borealis/style.hpp>
#include <borealis/tab_frame.hpp>
#include <borealis/table.hpp>
#include <borealis/theme.hpp>
#include <borealis/thumbnail_frame.hpp>
#include <borealis/view.hpp>
