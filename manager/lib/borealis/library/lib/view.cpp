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

#include <math.h>

#include <algorithm>
#include <borealis/animations.hpp>
#include <borealis/application.hpp>
#include <borealis/view.hpp>

namespace brls
{

NVGcolor transparent = nvgRGBA(0, 0, 0, 0);

static int shakeAnimation(float t, float a) // a = amplitude
{
    // Damped sine wave
    float w = 0.8f; // period
    float c = 0.35f; // damp factor
    return roundf(a * exp(-(c * t)) * sin(w * t));
}

void View::shakeHighlight(FocusDirection direction)
{
    this->highlightShaking        = true;
    this->highlightShakeStart     = cpu_features_get_time_usec() / 1000;
    this->highlightShakeDirection = direction;
    this->highlightShakeAmplitude = std::rand() % 15 + 10;
}

float View::getAlpha(bool child)
{
    return this->alpha * (this->parent ? this->parent->getAlpha(true) : 1.0f);
}

NVGcolor View::a(NVGcolor color)
{
    NVGcolor newColor = color;
    newColor.a *= this->getAlpha();
    return newColor;
}

NVGpaint View::a(NVGpaint paint)
{
    NVGpaint newPaint = paint;
    newPaint.innerColor.a *= this->getAlpha();
    newPaint.outerColor.a *= this->getAlpha();
    return newPaint;
}

// TODO: Only draw views that are onscreen (w/ some margins)
void View::frame(FrameContext* ctx)
{
    Style* style          = Application::getStyle();
    ThemeValues* oldTheme = ctx->theme;

    nvgSave(ctx->vg);

    // Theme override
    if (this->themeOverride)
        ctx->theme = themeOverride;

    // Layout if needed
    if (this->dirty)
    {
        this->invalidate(true);
        this->dirty = false;
    }

    if (this->alpha > 0.0f && this->collapseState != 0.0f)
    {
        // Draw background
        this->drawBackground(ctx->vg, ctx, style);

        // Draw highlight background
        if (this->highlightAlpha > 0.0f && this->isHighlightBackgroundEnabled())
            this->drawHighlight(ctx->vg, ctx->theme, this->highlightAlpha, style, true);

        // Collapse clipping
        if (this->collapseState < 1.0f)
        {
            nvgSave(ctx->vg);
            nvgIntersectScissor(ctx->vg, x, y, this->width, this->height * this->collapseState);
        }

        // Draw the view
        this->draw(ctx->vg, this->x, this->y, this->width, this->height, style, ctx);

        // Draw highlight
        if (this->highlightAlpha > 0.0f)
            this->drawHighlight(ctx->vg, ctx->theme, this->highlightAlpha, style, false);

        //Reset clipping
        if (this->collapseState < 1.0f)
            nvgRestore(ctx->vg);
    }

    // Cleanup
    if (this->themeOverride)
        ctx->theme = oldTheme;

    nvgRestore(ctx->vg);
}

void View::collapse(bool animated)
{
    menu_animation_ctx_tag tag = (uintptr_t) & this->collapseState;
    menu_animation_kill_by_tag(&tag);

    if (animated)
    {
        Style* style = Application::getStyle();

        menu_animation_ctx_entry_t entry;

        entry.cb           = [](void* userdata) {};
        entry.duration     = style->AnimationDuration.collapse;
        entry.easing_enum  = EASING_OUT_QUAD;
        entry.subject      = &this->collapseState;
        entry.tag          = tag;
        entry.target_value = 0.0f;
        entry.tick         = [this](void* userdata) { if (this->hasParent()) this->getParent()->invalidate(); };
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    }
    else
    {
        this->collapseState = 0.0f;
    }
}

bool View::isCollapsed()
{
    return this->collapseState < 1.0f;
}

void View::expand(bool animated)
{
    menu_animation_ctx_tag tag = (uintptr_t) & this->collapseState;
    menu_animation_kill_by_tag(&tag);

    if (animated)
    {
        Style* style = Application::getStyle();

        menu_animation_ctx_entry_t entry;

        entry.cb           = [](void* userdata) {};
        entry.duration     = style->AnimationDuration.collapse;
        entry.easing_enum  = EASING_OUT_QUAD;
        entry.subject      = &this->collapseState;
        entry.tag          = tag;
        entry.target_value = 1.0f;
        entry.tick         = [this](void* userdata) { if (this->hasParent()) this->getParent()->invalidate(); };
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    }
    else
    {
        this->collapseState = 1.0f;
    }
}

// TODO: Slight glow all around
void View::drawHighlight(NVGcontext* vg, ThemeValues* theme, float alpha, Style* style, bool background)
{
    nvgSave(vg);
    nvgResetScissor(vg);

    unsigned insetTop, insetRight, insetBottom, insetLeft;
    this->getHighlightInsets(&insetTop, &insetRight, &insetBottom, &insetLeft);

    float cornerRadius;
    this->getHighlightMetrics(style, &cornerRadius);

    unsigned x      = this->x - insetLeft - style->Highlight.strokeWidth / 2;
    unsigned y      = this->y - insetTop - style->Highlight.strokeWidth / 2;
    unsigned width  = this->width + insetLeft + insetRight + style->Highlight.strokeWidth;
    unsigned height = this->height + insetTop + insetBottom + style->Highlight.strokeWidth;

    // Shake animation
    if (this->highlightShaking)
    {
        retro_time_t curTime = cpu_features_get_time_usec() / 1000;
        retro_time_t t       = (curTime - highlightShakeStart) / 10;

        if (t >= style->AnimationDuration.shake)
        {
            this->highlightShaking = false;
        }
        else
        {
            switch (this->highlightShakeDirection)
            {
                case FocusDirection::RIGHT:
                    x += shakeAnimation(t, this->highlightShakeAmplitude);
                    break;
                case FocusDirection::LEFT:
                    x -= shakeAnimation(t, this->highlightShakeAmplitude);
                    break;
                case FocusDirection::DOWN:
                    y += shakeAnimation(t, this->highlightShakeAmplitude);
                    break;
                case FocusDirection::UP:
                    y -= shakeAnimation(t, this->highlightShakeAmplitude);
                    break;
                default:
                    break;
            }
        }
    }

    // Draw
    if (background)
    {
        // Background
        nvgFillColor(vg, RGBAf(theme->highlightBackgroundColor.r, theme->highlightBackgroundColor.g, theme->highlightBackgroundColor.b, this->highlightAlpha));
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgFill(vg);
    }
    else
    {
        // Shadow
        NVGpaint shadowPaint = nvgBoxGradient(vg,
            x, y + style->Highlight.shadowWidth,
            width, height,
            cornerRadius * 2, style->Highlight.shadowFeather,
            RGBA(0, 0, 0, style->Highlight.shadowOpacity * alpha), transparent);

        nvgBeginPath(vg);
        nvgRect(vg, x - style->Highlight.shadowOffset, y - style->Highlight.shadowOffset,
            width + style->Highlight.shadowOffset * 2, height + style->Highlight.shadowOffset * 3);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgPathWinding(vg, NVG_HOLE);
        nvgFillPaint(vg, shadowPaint);
        nvgFill(vg);

        // Border
        float gradientX, gradientY, color;
        menu_animation_get_highlight(&gradientX, &gradientY, &color);

        NVGcolor pulsationColor = RGBAf((color * theme->highlightColor1.r) + (1 - color) * theme->highlightColor2.r,
            (color * theme->highlightColor1.g) + (1 - color) * theme->highlightColor2.g,
            (color * theme->highlightColor1.b) + (1 - color) * theme->highlightColor2.b,
            alpha);

        NVGcolor borderColor = theme->highlightColor2;
        borderColor.a        = 0.5f * alpha * this->getAlpha();

        NVGpaint border1Paint = nvgRadialGradient(vg,
            x + gradientX * width, y + gradientY * height,
            style->Highlight.strokeWidth * 10, style->Highlight.strokeWidth * 40,
            borderColor, transparent);

        NVGpaint border2Paint = nvgRadialGradient(vg,
            x + (1 - gradientX) * width, y + (1 - gradientY) * height,
            style->Highlight.strokeWidth * 10, style->Highlight.strokeWidth * 40,
            borderColor, transparent);

        nvgBeginPath(vg);
        nvgStrokeColor(vg, pulsationColor);
        nvgStrokeWidth(vg, style->Highlight.strokeWidth);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgStrokePaint(vg, border1Paint);
        nvgStrokeWidth(vg, style->Highlight.strokeWidth);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgStrokePaint(vg, border2Paint);
        nvgStrokeWidth(vg, style->Highlight.strokeWidth);
        nvgRoundedRect(vg, x, y, width, height, cornerRadius);
        nvgStroke(vg);
    }

    nvgRestore(vg);
}

void View::setBackground(Background background)
{
    this->background = background;
}

void View::drawBackground(NVGcontext* vg, FrameContext* ctx, Style* style)
{
    switch (this->background)
    {
        case Background::SIDEBAR:
        {
            unsigned backdropHeight = style->Background.sidebarBorderHeight;

            // Solid color
            nvgBeginPath(vg);
            nvgFillColor(vg, a(ctx->theme->sidebarColor));
            nvgRect(vg, this->x, this->y + backdropHeight, this->width, this->height - backdropHeight * 2);
            nvgFill(vg);

            //Borders gradient
            // Top
            NVGpaint topGradient = nvgLinearGradient(vg, this->x, this->y + backdropHeight, this->x, this->y, a(ctx->theme->sidebarColor), transparent);
            nvgBeginPath(vg);
            nvgFillPaint(vg, topGradient);
            nvgRect(vg, this->x, this->y, this->width, backdropHeight);
            nvgFill(vg);

            // Bottom
            NVGpaint bottomGradient = nvgLinearGradient(vg, this->x, this->y + this->height - backdropHeight, this->x, this->y + this->height, a(ctx->theme->sidebarColor), transparent);
            nvgBeginPath(vg);
            nvgFillPaint(vg, bottomGradient);
            nvgRect(vg, this->x, this->y + this->height - backdropHeight, this->width, backdropHeight);
            nvgFill(vg);
            break;
        }
        case Background::DEBUG:
        {
            nvgFillColor(vg, RGB(255, 0, 0));
            nvgBeginPath(vg);
            nvgRect(vg, this->x, this->y, this->width, this->height);
            nvgFill(vg);
            break;
        }
        case Background::BACKDROP:
        {
            nvgFillColor(vg, a(ctx->theme->backdropColor));
            nvgBeginPath(vg);
            nvgRect(vg, this->x, this->y, this->width, this->height);
            nvgFill(vg);
        }
        case Background::NONE:
            break;
    }
}

void View::registerAction(std::string hintText, Key key, ActionListener actionListener, bool hidden)
{
    if (auto it = std::find(this->actions.begin(), this->actions.end(), key); it != this->actions.end())
        *it = { key, hintText, true, hidden, actionListener };
    else
        this->actions.push_back({ key, hintText, true, hidden, actionListener });
}

void View::updateActionHint(Key key, std::string hintText)
{
    if (auto it = std::find(this->actions.begin(), this->actions.end(), key); it != this->actions.end())
        it->hintText = hintText;

    Application::getGlobalHintsUpdateEvent()->fire();
}

void View::setActionAvailable(Key key, bool available)
{
    if (auto it = std::find(this->actions.begin(), this->actions.end(), key); it != this->actions.end())
        it->available = available;
}

void View::setBoundaries(int x, int y, unsigned width, unsigned height)
{
    this->x      = x;
    this->y      = y;
    this->width  = width;
    this->height = height;
}

void View::setParent(View* parent, void* parentUserdata)
{
    this->parent         = parent;
    this->parentUserdata = parentUserdata;
}

void* View::getParentUserData()
{
    return this->parentUserdata;
}

bool View::isFocused()
{
    return this->focused;
}

View* View::getParent()
{
    return this->parent;
}

bool View::hasParent()
{
    return this->parent;
}

void View::setWidth(unsigned width)
{
    this->width = width;
}

void View::setHeight(unsigned height)
{
    this->height = height;
}

int View::getX()
{
    return this->x;
}

int View::getY()
{
    return this->y;
}

unsigned View::getHeight(bool includeCollapse)
{
    return this->height * (includeCollapse ? this->collapseState : 1.0f);
}

unsigned View::getWidth()
{
    return this->width;
}

void View::onFocusGained()
{
    this->focused = true;

    Style* style = Application::getStyle();

    menu_animation_ctx_tag tag = (uintptr_t)this->highlightAlpha;

    menu_animation_ctx_entry_t entry;
    entry.cb           = [](void* userdata) {};
    entry.duration     = style->AnimationDuration.highlight;
    entry.easing_enum  = EASING_OUT_QUAD;
    entry.subject      = &this->highlightAlpha;
    entry.tag          = tag;
    entry.target_value = 1.0f;
    entry.tick         = [](void* userdata) {};
    entry.userdata     = nullptr;

    menu_animation_push(&entry);

    this->focusEvent.fire(this);

    if (this->hasParent())
        this->getParent()->onChildFocusGained(this);
}

GenericEvent* View::getFocusEvent()
{
    return &this->focusEvent;
}

/**
 * Fired when focus is lost
 */
void View::onFocusLost()
{
    this->focused = false;

    Style* style = Application::getStyle();

    menu_animation_ctx_tag tag = (uintptr_t)this->highlightAlpha;

    menu_animation_ctx_entry_t entry;
    entry.cb           = [](void* userdata) {};
    entry.duration     = style->AnimationDuration.highlight;
    entry.easing_enum  = EASING_OUT_QUAD;
    entry.subject      = &this->highlightAlpha;
    entry.tag          = tag;
    entry.target_value = 0.0f;
    entry.tick         = [](void* userdata) {};
    entry.userdata     = nullptr;

    menu_animation_push(&entry);

    if (this->hasParent())
        this->getParent()->onChildFocusLost(this);
}

void View::setForceTranslucent(bool translucent)
{
    this->forceTranslucent = translucent;
}

unsigned View::getShowAnimationDuration(ViewAnimation animation)
{
    Style* style = Application::getStyle();

    if (animation == ViewAnimation::SLIDE_LEFT || animation == ViewAnimation::SLIDE_RIGHT)
        return style->AnimationDuration.showSlide;

    return style->AnimationDuration.show;
}

void View::show(std::function<void(void)> cb, bool animate, ViewAnimation animation)
{
    brls::Logger::debug("Showing %s with animation %d", this->describe().c_str(), animation);

    this->hidden = false;

    menu_animation_ctx_tag tag = (uintptr_t) & this->alpha;
    menu_animation_kill_by_tag(&tag);

    this->fadeIn = true;

    if (animate)
    {
        this->alpha = 0.0f;

        menu_animation_ctx_entry_t entry;
        entry.cb = [this, cb](void* userdata) {
            this->fadeIn = false;
            this->onShowAnimationEnd();
            cb();
        };
        entry.duration     = this->getShowAnimationDuration(animation);
        entry.easing_enum  = EASING_OUT_QUAD;
        entry.subject      = &this->alpha;
        entry.tag          = tag;
        entry.target_value = 1.0f;
        entry.tick         = [](void* userdata) {};
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    }
    else
    {
        this->alpha  = 1.0f;
        this->fadeIn = false;
        this->onShowAnimationEnd();
        cb();
    }
}

void View::hide(std::function<void(void)> cb, bool animated, ViewAnimation animation)
{
    brls::Logger::debug("Hiding %s with animation %d", this->describe().c_str(), animation);

    this->hidden = true;
    this->fadeIn = false;

    menu_animation_ctx_tag tag = (uintptr_t) & this->alpha;
    menu_animation_kill_by_tag(&tag);

    if (animated)
    {
        this->alpha = 1.0f;

        menu_animation_ctx_entry_t entry;
        entry.cb           = [cb](void* userdata) { cb(); };
        entry.duration     = this->getShowAnimationDuration(animation);
        entry.easing_enum  = EASING_OUT_QUAD;
        entry.subject      = &this->alpha;
        entry.tag          = tag;
        entry.target_value = 0.0f;
        entry.tick         = [](void* userdata) {};
        entry.userdata     = nullptr;

        menu_animation_push(&entry);
    }
    else
    {
        this->alpha = 0.0f;
        cb();
    }
}

bool View::isHidden()
{
    return this->hidden;
}

void View::overrideThemeVariant(ThemeValues* theme)
{
    this->themeOverride = theme;
}

View::~View()
{
    menu_animation_ctx_tag alphaTag = (uintptr_t) & this->alpha;
    menu_animation_kill_by_tag(&alphaTag);

    menu_animation_ctx_tag highlightTag = (uintptr_t) & this->highlightAlpha;
    menu_animation_kill_by_tag(&highlightTag);

    menu_animation_ctx_tag collapseTag = (uintptr_t) & this->collapseState;
    menu_animation_kill_by_tag(&collapseTag);

    // Parent userdata
    if (this->parentUserdata)
    {
        free(this->parentUserdata);
        this->parentUserdata = nullptr;
    }

    // Focus sanity check
    if (Application::getCurrentFocus() == this)
        Application::giveFocus(nullptr);
}

void View::invalidate(bool immediate)
{
    if (immediate)
        this->layout(Application::getNVGContext(), Application::getStyle(), Application::getFontStash());
    else
        this->dirty = true;
}

} // namespace brls
