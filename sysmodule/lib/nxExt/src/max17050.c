/*
 * Fuel gauge driver for Nintendo Switch's Maxim 17050
 *
 * Copyright (c) 2011 Samsung Electronics
 * MyungJoo Ham <myungjoo.ham@samsung.com>
 * Copyright (c) 2018 CTCaer
 * Copyright (c) 2022 p-sam
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "nxExt/max17050.h"
#include "nxExt/i2c.h"

#define MAX17050_WAIT_NS 1000000000UL

#define MAX17050_VCELL      0x09
#define MAX17050_Current    0x0A
#define MAX17050_AvgCurrent 0x0B
#define MAX17050_AvgVCELL   0x19

#define MAX17050_BOARD_CGAIN 2
#define MAX17050_BOARD_SNS_RESISTOR_UOHM 5000

static I2cSession g_i2c_session;
static u64 g_update_ticks = 0;
static s32 g_power_now = 0;
static s32 g_power_avg = 0;

static Result _max17050_get_power(s32 *out_mw_now, s32 *out_mw_avg)
{
    s64 ma, mv;
    u16 values[3] = {0};

    Result rc = i2csessionExtRegReceive(&g_i2c_session, MAX17050_VCELL, values, sizeof(values));

    if (R_SUCCEEDED(rc))
    {
        ma = (s16)values[1];
        ma = ma * 1562500 / (MAX17050_BOARD_SNS_RESISTOR_UOHM * MAX17050_BOARD_CGAIN);

        mv = (int)(values[0] >> 3) * 625 / 1000;

        *out_mw_now = ma * mv / 1000000;
    }

    if (R_SUCCEEDED(rc))
    {
        rc = i2csessionExtRegReceive(&g_i2c_session, MAX17050_AvgVCELL, values, sizeof(u16));
    }

    if (R_SUCCEEDED(rc))
    {
        ma = (s16)values[2];
        ma = ma * 1562500 / (MAX17050_BOARD_SNS_RESISTOR_UOHM * MAX17050_BOARD_CGAIN);

        mv = (int)(values[0] >> 3) * 625 / 1000;

        *out_mw_avg = ma * mv / 1000000;
    }

    return rc;
}

static void _max17050_update()
{
    u64 ticks = armGetSystemTick();
    if(armTicksToNs(ticks - g_update_ticks) <= MAX17050_WAIT_NS)
    {
        return;
    }

    g_update_ticks = ticks;

    if(!serviceIsActive(&g_i2c_session.s))
    {
        return;
    }

    _max17050_get_power(&g_power_now, &g_power_avg);
}

Result max17050Initialize(void)
{
    Result rc = i2cInitialize();

    if(R_SUCCEEDED(rc))
    {
        rc = i2cOpenSession(&g_i2c_session, I2cDevice_Max17050);
    }

    return rc;
}

void max17050Exit(void)
{
    i2csessionClose(&g_i2c_session);
    i2cExit();
}

s32 max17050PowerNow(void)
{
    _max17050_update();
    return g_power_now;
}

s32 max17050PowerAvg(void)
{
    _max17050_update();
    return g_power_avg;
}
