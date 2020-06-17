/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "base_gui.h"

#include "../elements/base_frame.h"
#include "logo_rgba_bin.h"

#define LOGO_WIDTH 110
#define LOGO_HEIGHT 39
#define LOGO_X 18
#define LOGO_Y 21

#define LOGO_LABEL_X (LOGO_X + LOGO_WIDTH + 6)
#define LOGO_LABEL_Y 50
#define LOGO_LABEL_FONT_SIZE 28

#define VERSION_X (LOGO_LABEL_X + 110)
#define VERSION_Y LOGO_LABEL_Y
#define VERSION_FONT_SIZE 15

void BaseGui::preDraw(tsl::gfx::Renderer* renderer)
{
    renderer->drawBitmap(LOGO_X, LOGO_Y, LOGO_WIDTH, LOGO_HEIGHT, logo_rgba_bin);
    renderer->drawString("overlay", false, LOGO_LABEL_X, LOGO_LABEL_Y, LOGO_LABEL_FONT_SIZE, TEXT_COLOR);
    renderer->drawString(TARGET_VERSION, false, VERSION_X, VERSION_Y, VERSION_FONT_SIZE, DESC_COLOR);
}

tsl::elm::Element* BaseGui::createUI()
{
    BaseFrame* rootFrame = new BaseFrame(this);
    rootFrame->setContent(this->baseUI());
    return rootFrame;
}

void BaseGui::update()
{
    this->refresh();
}
