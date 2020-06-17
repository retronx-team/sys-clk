/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
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

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <nanovg.h>

#include <borealis/animations.hpp>
#include <borealis/frame_context.hpp>
#include <borealis/hint.hpp>
#include <borealis/label.hpp>
#include <borealis/logger.hpp>
#include <borealis/notification_manager.hpp>
#include <borealis/style.hpp>
#include <borealis/task_manager.hpp>
#include <borealis/theme.hpp>
#include <borealis/view.hpp>
#include <map>
#include <vector>

namespace brls
{

// The top-right framerate counter
class FramerateCounter : public Label
{
  private:
    retro_time_t lastSecond = 0;
    unsigned frames         = 0;

  public:
    FramerateCounter();

    void frame(FrameContext* ctx) override;
};

class Application
{
  public:
    //Init with default style and theme (as close to HOS as possible)
    static bool init(std::string title);

    // Init with given style and theme
    static bool init(std::string title, Style style, Theme theme);

    static bool mainLoop();

    /**
      * Pushes a view on this applications's view stack
      *
      * The view will automatically be resized to take
      * the whole screen and layout() will be called
      *
      * The view will gain focus if applicable
      */
    static void pushView(View* view, ViewAnimation animation = ViewAnimation::FADE);

    /**
      * Pops the last pushed view from the stack
      * and gives focus back where it was before
      */
    static void popView(
        ViewAnimation animation = ViewAnimation::FADE, std::function<void(void)> cb = []() {});

    /**
     * Gives the focus to the given view
     * or clears the focus if given nullptr
     */
    static void giveFocus(View* view);

    static Style* getStyle();
    static void setTheme(Theme theme);
    static ThemeValues* getThemeValues();
    static ThemeValues* getThemeValuesForVariant(ThemeVariant variant);
    static ThemeVariant getThemeVariant();

    static int loadFont(const char* fontName, const char* filePath);
    static int loadFontFromMemory(const char* fontName, void* data, size_t size, bool freeData);
    static int findFont(const char* fontName);

    static FontStash* getFontStash();

    static void notify(std::string text);

    static void onGamepadButtonPressed(char button, bool repeating);

    /**
      * "Crashes" the app (displays a fullscreen CrashFrame)
      */
    static void crash(std::string text);

    static void quit();

    /**
      * Blocks any and all user inputs
      */
    static void blockInputs();

    /**
      * Unblocks inputs after a call to
      * blockInputs()
      */
    static void unblockInputs();

    static NVGcontext* getNVGContext();
    static TaskManager* getTaskManager();
    static NotificationManager* getNotificationManager();

    static void setCommonFooter(std::string footer);
    static std::string* getCommonFooter();

    static void setDisplayFramerate(bool enabled);
    static void toggleFramerateDisplay();

    static void setMaximumFPS(unsigned fps);

    // public so that the glfw callback can access it
    inline static unsigned contentWidth, contentHeight;
    inline static float windowScale;

    static void resizeFramerateCounter();
    static void resizeNotificationManager();

    static GenericEvent* getGlobalFocusChangeEvent();
    static VoidEvent* getGlobalHintsUpdateEvent();

    static View* getCurrentFocus();

    static std::string getTitle();

  private:
    inline static GLFWwindow* window;
    inline static NVGcontext* vg;

    inline static std::string title;

    inline static TaskManager* taskManager;
    inline static NotificationManager* notificationManager;

    inline static FontStash fontStash;

    inline static std::vector<View*> viewStack;
    inline static std::vector<View*> focusStack;

    inline static unsigned windowWidth, windowHeight;

    inline static View* currentFocus;

    inline static Theme currentTheme;
    inline static ThemeVariant currentThemeVariant;

    inline static GLFWgamepadstate oldGamepad;
    inline static GLFWgamepadstate gamepad;

    inline static Style currentStyle;

    inline static unsigned blockInputsTokens = 0; // any value > 0 means inputs are blocked

    inline static std::string commonFooter = "";

    inline static FramerateCounter* framerateCounter = nullptr;

    inline static float frameTime = 0.0f;

    inline static View* repetitionOldFocus = nullptr;

    inline static GenericEvent globalFocusChangeEvent;
    inline static VoidEvent globalHintsUpdateEvent;

    static void navigate(FocusDirection direction);

    static void onWindowSizeChanged();

    static void frame();
    static void clear();
    static void exit();

    /**
     * Handles actions for the currently focused view and
     * the given button
     * Returns true if at least one action has been fired
     */
    static bool handleAction(char button);
};

} // namespace brls
