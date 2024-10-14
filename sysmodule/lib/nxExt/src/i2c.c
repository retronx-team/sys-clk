/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "nxExt/i2c.h"

#define I2C_CMD_SND 0
#define I2C_CMD_RCV 1

Result i2csessionExtRegReceive(I2cSession* s, u8 in, void* out, u8 out_size)
{
    u8 cmdlist[5] = {
        I2C_CMD_SND | (I2cTransactionOption_Start << 6),
        sizeof(in),
        in,

        I2C_CMD_RCV | (I2cTransactionOption_All << 6),
        out_size
    };

    return i2csessionExecuteCommandList(s, out, out_size, cmdlist, sizeof(cmdlist));
}
