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

#include <borealis/application.hpp>
#include <borealis/repeating_task.hpp>

namespace brls
{

RepeatingTask::RepeatingTask(retro_time_t interval)
    : interval(interval)
{
    Application::getTaskManager()->registerRepeatingTask(this);
}

void RepeatingTask::run(retro_time_t currentTime)
{
    this->lastRun = currentTime;
}

void RepeatingTask::start()
{
    this->onStart();
    this->running = true;
}

void RepeatingTask::pause()
{
    this->running = false;
}

void RepeatingTask::stop()
{
    this->pause();
    this->stopRequested = true;
}

void RepeatingTask::fireNow()
{
    if (!this->isRunning())
        return;

    retro_time_t currentTime = cpu_features_get_time_usec() / 1000;
    this->run(currentTime);
}

retro_time_t RepeatingTask::getInterval()
{
    return this->interval;
}

retro_time_t RepeatingTask::getLastRun()
{
    return this->lastRun;
}

bool RepeatingTask::isRunning()
{
    return this->running;
}

bool RepeatingTask::isStopRequested()
{
    return this->stopRequested;
}

RepeatingTask::~RepeatingTask()
{
    // Nothing to do here, only the Task Manager is supposed to free tasks
}

} // namespace brls
