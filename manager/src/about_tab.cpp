/*
    sys-clk manager, a sys-clk frontend homebrew
    Copyright (C) 2019-2020  natinusala
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

#include "about_tab.h"

#include "logo.h"
#include "ipc/ipc.h"
#include "ipc/client.h"

#include <borealis.hpp>

AboutTab::AboutTab()
{
    this->addView(new Logo(LogoStyle::ABOUT));

    // Subtitle
    brls::Label *subTitle = new brls::Label(
        brls::LabelStyle::REGULAR,
        "Nintendo Switch overclocking / underclocking system module and frontend app by the RetroNX Team", 
        true
    );
    subTitle->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(subTitle);

    // Copyright
    brls::Label *copyright = new brls::Label(
        brls::LabelStyle::DESCRIPTION,
        "System module licensed under the Beerware license\n" \
        "Frontend app licensed under GPL-3.0\n" \
        "\u00A9 2019 - 2020 natinusala, p-sam, m4xw",
        true
    );
    copyright->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->addView(copyright);

    // Links
    this->addView(new brls::Header("Links and Resources"));
    brls::Label *links = new brls::Label(
        brls::LabelStyle::SMALL,
        "\uE016  User guide and code source can be found on our GitHub repository\n" \
        "\uE016  The sys-clk manager is powered by Borealis, an hardware accelerated UI library\n" \
        "\uE016  Join the RetroNX Discord server for support, to request features or just hang out!",
        true
    );
    this->addView(links);
}
