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

#include <features/features_cpu.h>
#include <stdio.h>

#include <borealis/actions.hpp>
#include <borealis/event.hpp>
#include <borealis/frame_context.hpp>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace brls
{

// The animation to play when
// pushing or popping a view
// (implemented in Application)
enum class ViewAnimation
{
    FADE, // the old view fades away and the new one fades in
    SLIDE_LEFT, // the old view slides out to the left and the new one slides in from the right
    SLIDE_RIGHT // inverted SLIDE_LEFT
};

// Focus direction when navigating
enum class FocusDirection
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// View background
enum class Background
{
    NONE,
    SIDEBAR,
    DEBUG,
    BACKDROP
};

extern NVGcolor transparent;

class View;

typedef Event<View*> GenericEvent;
typedef Event<> VoidEvent;

// Superclass for all the other views
// Lifecycle of a view is :
//   new -> [willAppear -> willDisappear] -> delete
//
// Users have do to the new, the rest of the lifecycle is taken
// care of by the library
//
// willAppear and willDisappear can be called zero or multiple times
// before deletion (in case of a TabLayout for instance)
class View
{
  private:
    Background background = Background::NONE;

    void drawBackground(NVGcontext* vg, FrameContext* ctx, Style* style);
    void drawHighlight(NVGcontext* vg, ThemeValues* theme, float alpha, Style* style, bool background);

    float highlightAlpha = 0.0f;

    bool dirty = true;

    bool highlightShaking = false;
    retro_time_t highlightShakeStart;
    FocusDirection highlightShakeDirection;
    float highlightShakeAmplitude;

    bool fadeIn           = false; // is the fade in animation running?
    bool forceTranslucent = false;

    ThemeValues* themeOverride = nullptr;

    bool hidden = false;

    std::vector<Action> actions;

    /**
     * Parent user data, typically the index of the view
     * in the internal layout structure
     */
    void* parentUserdata = nullptr;

  protected:
    int x = 0;
    int y = 0;

    unsigned width  = 0;
    unsigned height = 0;

    float collapseState = 1.0f;

    bool focused = false;

    View* parent = nullptr;

    GenericEvent focusEvent;

    virtual unsigned getShowAnimationDuration(ViewAnimation animation);

    virtual void getHighlightInsets(unsigned* top, unsigned* right, unsigned* bottom, unsigned* left)
    {
        *top    = 0;
        *right  = 0;
        *bottom = 0;
        *left   = 0;
    }

    virtual void getHighlightMetrics(Style* style, float* cornerRadius)
    {
        *cornerRadius = style->Highlight.cornerRadius;
    }

    virtual bool isHighlightBackgroundEnabled()
    {
        return true;
    }

    // Helper functions to apply this view's alpha to a color
    NVGcolor a(NVGcolor color);
    NVGpaint a(NVGpaint paint);

    NVGcolor RGB(unsigned r, unsigned g, unsigned b)
    {
        return this->a(nvgRGB(r, g, b));
    }

    NVGcolor RGBA(unsigned r, unsigned g, unsigned b, unsigned a)
    {
        return this->a(nvgRGBA(r, g, b, a));
    }

    NVGcolor RGBf(float r, float g, float b)
    {
        return this->a(nvgRGBf(r, g, b));
    }

    NVGcolor RGBAf(float r, float g, float b, float a)
    {
        return this->a(nvgRGBAf(r, g, b, a));
    }

    /**
     * Should the hint alpha be animated when
     * pushing the view?
     */
    virtual bool animateHint()
    {
        return false;
    }

  public:
    void setBoundaries(int x, int y, unsigned width, unsigned height);

    void setBackground(Background background);

    void setWidth(unsigned width);
    void setHeight(unsigned height);

    void shakeHighlight(FocusDirection direction);

    int getX();
    int getY();
    unsigned getWidth();
    unsigned getHeight(bool includeCollapse = true);

    void setForceTranslucent(bool translucent);

    void setParent(View* parent, void* parentUserdata = nullptr);
    View* getParent();
    bool hasParent();

    void* getParentUserData();

    void registerAction(std::string hintText, Key key, ActionListener actionListener, bool hidden = false);
    void updateActionHint(Key key, std::string hintText);
    void setActionAvailable(Key key, bool available);

    std::string describe() const { return typeid(*this).name(); }

    const std::vector<Action>& getActions()
    {
        return this->actions;
    }

    /**
      * Called each frame
      * Do not override it to draw your view,
      * override draw() instead
      */
    virtual void frame(FrameContext* ctx);

    /**
      * Called by frame() to draw
      * the view onscreen
      */
    virtual void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) = 0;

    /**
      * Triggered when the view has been
      * resized and needs to layout its
      * children
      */
    virtual void layout(NVGcontext* vg, Style* style, FontStash* stash)
    {
        // Nothing to do
    }

    /**
      * Called when the view will appear
      * on screen, before or after layout()
      *
      * Can be called if the view has
      * already appeared, so be careful
      */
    virtual void willAppear(bool resetState = false)
    {
        // Nothing to do
    }

    /**
      * Called when the view will disappear
      * from the screen
      *
      * Can be called if the view has
      * already disappeared, so be careful
      */
    virtual void willDisappear(bool resetState = false)
    {
        // Nothing to do
    }

    /**
      * Called when the show() animation (fade in)
      * ends
      */
    virtual void onShowAnimationEnd() {};

    /**
      * Shows the view (fade in animation)
      *
      * Called once when the view is
      * pushed to the view stack
      *
      * Not recursive
      */
    virtual void show(std::function<void(void)> cb, bool animate = true, ViewAnimation animation = ViewAnimation::FADE);

    /**
      * Hides the view in a collapse animation
      */
    void collapse(bool animated = true);

    bool isCollapsed();

    /**
      * Shows the view in a expand animation (opposite
      * of collapse)
      */
    void expand(bool animated = true);

    /**
      * Hides the view (fade out animation)
      *
      * Called if another view is pushed
      * on top of this one
      *
      * Not recursive
      */
    virtual void hide(std::function<void(void)> cb, bool animated = true, ViewAnimation animation = ViewAnimation::FADE);

    bool isHidden();

    /**
      * Calls layout() on next frame
      * unless immediate is true in which case
      * it's called immediately
      */
    void invalidate(bool immediate = false);

    /**
      * Is this view translucent?
      *
      * If you override it please return
      * <value> || View::isTranslucent()
      * to keep the fadeIn transition
      */
    virtual bool isTranslucent()
    {
        return fadeIn || forceTranslucent;
    }

    bool isFocused();

    /**
     * Returns the default view to focus when focusing this view
     * Typically the view itself or one of its children
     *
     * Returning nullptr means that the view is not focusable
     * (and neither are its children)
     *
     * When pressing a key, the flow is :
     *    1. starting from the currently focused view's parent, traverse the tree upwards and
     *       repeatidly call getNextFocus() on every view until we find a next view to focus or meet the end of the tree
     *    2. if a view is found, getNextFocus() will internally call getDefaultFocus() for the selected child
     *    3. give focus to the result, if it exists
     */
    virtual View* getDefaultFocus()
    {
        return nullptr;
    }

    /**
     * Returns the next view to focus given the requested direction
     * and the currently focused view (as parent user data)
     *
     * Returning nullptr means that there is no next view to focus
     * in that direction - getNextFocus will then be called on our
     * parent if any
     */
    virtual View* getNextFocus(FocusDirection direction, void* parentUserdata)
    {
        return nullptr;
    }

    /**
      * Fired when focus is gained
      */
    virtual void onFocusGained();

    /**
      * Fired when focus is lost
      */
    virtual void onFocusLost();

    /**
     * Fired when focus is gained on one of this view's children
     */
    virtual void onChildFocusGained(View* child)
    {
        if (this->hasParent())
            this->getParent()->onChildFocusGained(this);
    }

    /**
     * Fired when focus is gained on one of this view's children
     */
    virtual void onChildFocusLost(View* child)
    {
        if (this->hasParent())
            this->getParent()->onChildFocusLost(this);
    }

    /**
     * Fired when the window size changes
     * Not guaranteed to be called before or after layout()
     */
    virtual void onWindowSizeChanged()
    {
        // Nothing by default
    }

    GenericEvent* getFocusEvent();

    float alpha = 1.0f;

    virtual float getAlpha(bool child = false);

    /**
      * Forces this view and its children to use
      * the specified theme variant
      */
    void overrideThemeVariant(ThemeValues* newTheme);

    virtual ~View();
};

} // namespace brls
