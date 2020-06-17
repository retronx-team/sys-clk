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

#include <features/features_cpu.h>

namespace brls
{

// A task that is repeated at a given interval
// by the UI thread
class RepeatingTask
{
  private:
    retro_time_t interval;

    retro_time_t lastRun = 0;

    bool running       = false;
    bool stopRequested = false;

  public:
    RepeatingTask(retro_time_t interval);
    virtual ~RepeatingTask();

    /**
      * Actual code to run by the task
      * Must call RepeatingTask::run() !
      */
    virtual void run(retro_time_t currentTime);

    /**
      * Fired when the task starts
      */
    virtual void onStart() {};

    /**
      * Fired when the task stops
      */
    virtual void onStop() {};

    /**
      * Starts the task
      */
    void start();

    /**
      * Fires the task immediately and delays the
      * next run
      */
    void fireNow();

    /**
      * Pauses the task without deleting it
      */
    void pause();

    /**
      * Stops and deletes the task
      */
    void stop();

    retro_time_t getInterval();
    retro_time_t getLastRun();

    bool isRunning();
    bool isStopRequested();
};

} // namespace brls
