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

#include <features/features_cpu.h>

#include <borealis/task_manager.hpp>

namespace brls
{

void TaskManager::frame()
{
    // Repeating tasks
    retro_time_t currentTime = cpu_features_get_time_usec() / 1000;
    for (auto i = this->repeatingTasks.begin(); i != this->repeatingTasks.end(); i++)
    {
        RepeatingTask* task = *i;

        // Stop the task if needed
        if (task->isStopRequested())
        {
            this->stopRepeatingTask(task);
            this->repeatingTasks.erase(i--);
        }
        // Fire it
        else if (task->isRunning() && currentTime - task->getLastRun() > task->getInterval())
        {
            task->run(currentTime);
        }
    }
}

void TaskManager::registerRepeatingTask(RepeatingTask* task)
{
    this->repeatingTasks.push_back(task);
}

void TaskManager::stopRepeatingTask(RepeatingTask* task)
{
    task->onStop();
    delete task;
}

TaskManager::~TaskManager()
{
    // Stop all repeating tasks
    for (RepeatingTask* task : this->repeatingTasks)
        this->stopRepeatingTask(task);

    this->repeatingTasks.clear();
}

} // namespace brls
