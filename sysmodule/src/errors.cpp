/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "errors.h"
#include <cstdarg>
#include <cstring>

void Errors::ThrowException(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const char *msg = Errors::FormatMessage(format, args);
    va_end(args);

    throw std::runtime_error(msg);
}

const char *Errors::FormatMessage(const char *format, va_list args)
{
    size_t len = vsnprintf(NULL, 0, format, args) * sizeof(char);
    char *buf = (char *)malloc(len + 1);
    if (buf == NULL)
    {
        return format;
    }

    vsnprintf(buf, len + 1, format, args);

    return buf;
}
