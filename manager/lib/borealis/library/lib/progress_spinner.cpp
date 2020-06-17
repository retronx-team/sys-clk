/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  Billy Laws
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

#include <borealis/animations.hpp>
#include <borealis/application.hpp>
#include <borealis/progress_spinner.hpp>

namespace brls
{

ProgressSpinner::ProgressSpinner() {}

void ProgressSpinner::restartAnimation()
{
    Style* style = Application::getStyle();

    menu_animation_ctx_tag tag = (uintptr_t) & this->animationValue;
    menu_animation_ctx_entry_t entry;

    this->animationValue = 0.0f;

    menu_animation_kill_by_tag(&tag);

    entry.cb           = [this](void* userdata) { this->restartAnimation(); };
    entry.duration     = style->AnimationDuration.progress;
    entry.easing_enum  = EASING_LINEAR;
    entry.subject      = &this->animationValue;
    entry.tag          = tag;
    entry.target_value = 8.0f;
    entry.tick         = [](void* userdata) {};
    entry.userdata     = nullptr;

    menu_animation_push(&entry);
}

void ProgressSpinner::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    this->height = std::min(this->width, this->height);
    this->width  = this->height;
}

void ProgressSpinner::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    NVGcolor barColor = a(ctx->theme->spinnerBarColor);

    // Each bar of the spinner
    for (int i = 0 + animationValue; i < 8 + animationValue; i++)
    {
        barColor.a = fmax((i - animationValue) / 8.0f, a(ctx->theme->spinnerBarColor).a);
        nvgSave(vg);
        nvgTranslate(vg, x + width / 2, y + height / 2);
        nvgRotate(vg, nvgDegToRad(i * 45)); // Internal angle of octagon
        nvgBeginPath(vg);
        nvgMoveTo(vg, height * style->ProgressSpinner.centerGapMultiplier, 0);
        nvgLineTo(vg, height / 2 - height * style->ProgressSpinner.centerGapMultiplier, 0);
        nvgStrokeColor(vg, barColor);
        nvgStrokeWidth(vg, height * style->ProgressSpinner.barWidthMultiplier);
        nvgLineCap(vg, NVG_SQUARE);
        nvgStroke(vg);
        nvgRestore(vg);
    }
}

void ProgressSpinner::willAppear(bool resetState)
{
    this->restartAnimation();
}

void ProgressSpinner::willDisappear(bool resetState)
{
    menu_animation_ctx_tag tag = (uintptr_t) & this->animationValue;

    this->animationValue = 0.0f;

    menu_animation_kill_by_tag(&tag);
}

} // namespace brls
