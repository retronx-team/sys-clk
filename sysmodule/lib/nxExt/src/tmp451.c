/*
 * SOC/PCB Temperature driver for Nintendo Switch's TI TMP451
 *
 * Copyright (c) 2018-2024 CTCaer
 * Copyright (c) 2024 p-sam
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nxExt/tmp451.h"
#include "nxExt/i2c.h"

#define TMP451_WAIT_NS 1000000000UL

#define TMP451_PCB_TEMP_REG    0x00
#define TMP451_SOC_TEMP_REG    0x01

#define TMP451_SOC_TMP_DEC_REG 0x10
#define TMP451_PCB_TMP_DEC_REG 0x15

static I2cSession g_i2c_session;
static u64 g_update_ticks = 0;
static s32 g_temp_pcb = 0;
static s32 g_temp_soc = 0;

static Result _tmp451_get_temp(u8 reg, u8 dec_reg, s32* out)
{
    u8 val = 0;
    Result rc = i2csessionExtRegReceive(&g_i2c_session, reg, &val, sizeof(val));

    if(R_SUCCEEDED(rc))
    {
        *out = (s32)val * 1000;
        rc = i2csessionExtRegReceive(&g_i2c_session, dec_reg, &val, sizeof(val));
    }

    if(R_SUCCEEDED(rc))
    {
        *out += ((s32)(val >> 4) * 625) / 10;
    }

    return rc;
}

static void _tmp451_update()
{
    u64 ticks = armGetSystemTick();
    if(armTicksToNs(ticks - g_update_ticks) <= TMP451_WAIT_NS)
    {
        return;
    }

    g_update_ticks = ticks;

    if(!serviceIsActive(&g_i2c_session.s))
    {
        return;
    }

    _tmp451_get_temp(TMP451_PCB_TEMP_REG, TMP451_PCB_TMP_DEC_REG, &g_temp_pcb);
    _tmp451_get_temp(TMP451_SOC_TEMP_REG, TMP451_SOC_TMP_DEC_REG, &g_temp_soc);
}

Result tmp451Initialize(void)
{
    Result rc = i2cInitialize();

    if(R_SUCCEEDED(rc))
    {
        rc = i2cOpenSession(&g_i2c_session, I2cDevice_Tmp451);
    }

    return rc;
}

void tmp451Exit(void)
{
    i2csessionClose(&g_i2c_session);
    i2cExit();
}

s32 tmp451TempPcb(void)
{
    _tmp451_update();
    return g_temp_pcb;
}

s32 tmp451TempSoc(void)
{
    _tmp451_update();
    return g_temp_soc;
}
