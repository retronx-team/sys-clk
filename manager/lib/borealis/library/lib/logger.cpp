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

#include <stdarg.h>
#include <stdio.h>

#include <borealis/logger.hpp>

namespace brls
{

static LogLevel g_logLevel = LogLevel::INFO;

void Logger::setLogLevel(LogLevel newLogLevel)
{
    g_logLevel = newLogLevel;
}

void Logger::log(LogLevel logLevel, const char* prefix, const char* color, const char* format, va_list ap)
{
    if (g_logLevel < logLevel)
        return;

    printf("\033%s[%s]\033[0m ", color, prefix);
    vprintf(format, ap);
    printf("\n");

#ifdef __MINGW32__
    fflush(0);
#endif
}

void Logger::error(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    Logger::log(LogLevel::ERROR, "ERROR", "[0;31m", format, ap);
    va_end(ap);
}

void Logger::info(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    Logger::log(LogLevel::INFO, "INFO", "[0;34m", format, ap);
    va_end(ap);
}

void Logger::debug(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    Logger::log(LogLevel::DEBUG, "DEBUG", "[0;32m", format, ap);
    va_end(ap);
}

} // namespace brls
