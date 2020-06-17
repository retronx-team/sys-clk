/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

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
#include <borealis/thumbnail_frame.hpp>

namespace brls
{

ThumbnailFrame::ThumbnailFrame()
    : AppletFrame(true, false)
{
    // Create the ThumbnailSidebar
    this->sidebar = new ThumbnailSidebar();

    // Setup content view
    this->boxLayout = new BoxLayout(BoxLayoutOrientation::HORIZONTAL);
    AppletFrame::setContentView(this->boxLayout);
}

void ThumbnailFrame::setContentView(View* view)
{
    this->thumbnailContentView = view;

    // Clear the layout
    this->boxLayout->clear();

    // Add the views
    this->boxLayout->addView(view);
    this->boxLayout->addView(this->sidebar);

    // Invalidate
    this->invalidate();
}

void ThumbnailFrame::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    // Resize content view
    if (this->thumbnailContentView)
    {
        unsigned sidebarWidth = this->sidebar->getWidth();
        this->thumbnailContentView->setWidth(this->getWidth() - sidebarWidth - this->leftPadding - this->rightPadding);
    }

    // Layout the rest
    AppletFrame::layout(vg, style, stash);
}

ThumbnailSidebar* ThumbnailFrame::getSidebar()
{
    return this->sidebar;
}

ThumbnailFrame::~ThumbnailFrame()
{
    // If content view wasn't set, free the sidebar manually
    if (!this->thumbnailContentView)
        delete this->sidebar;
}

ThumbnailSidebar::ThumbnailSidebar()
{
    Style* style = Application::getStyle();

    this->setBackground(Background::SIDEBAR);
    this->setWidth(style->Sidebar.width);

    this->button = (new Button(ButtonStyle::PLAIN))->setLabel("Save");
    this->button->setParent(this);
}

Button* ThumbnailSidebar::getButton()
{
    return this->button;
}

void ThumbnailSidebar::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    if (this->image)
        this->image->frame(ctx);

    if (this->title)
        this->title->frame(ctx);

    if (this->subTitle)
        this->subTitle->frame(ctx);

    this->button->frame(ctx);
}

void ThumbnailSidebar::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    unsigned sidebarWidth = getWidth() - style->AppletFrame.separatorSpacing;
    unsigned yAdvance     = getY() + style->ThumbnailSidebar.marginTopBottom;
    unsigned titleX       = getX() + style->ThumbnailSidebar.marginLeftRight / 2;
    unsigned titleWidth   = sidebarWidth - style->ThumbnailSidebar.marginLeftRight;

    // Image
    if (this->image)
    {
        unsigned imageX     = getX() + style->ThumbnailSidebar.marginLeftRight;
        unsigned imageWidth = sidebarWidth - style->ThumbnailSidebar.marginLeftRight * 2;

        this->image->setBoundaries(
            imageX,
            yAdvance,
            imageWidth,
            imageWidth);

        yAdvance += this->image->getHeight() + style->ThumbnailSidebar.marginTopBottom;
    }

    // Title
    if (this->title)
    {
        this->title->setBoundaries(
            titleX,
            yAdvance,
            titleWidth,
            0 // height is dynamic
        );

        // Call layout directly to update height
        this->title->invalidate(true);

        yAdvance += this->title->getHeight() + style->ThumbnailSidebar.marginTopBottom / 2;
    }

    // Subtitle
    if (this->subTitle)
    {
        this->subTitle->setBoundaries(
            titleX,
            yAdvance,
            titleWidth,
            0 // height doesn't matter
        );

        this->subTitle->invalidate();
    }

    //Button
    unsigned buttonWidth  = sidebarWidth - style->ThumbnailSidebar.buttonMargin * 2;
    unsigned buttonHeight = style->ThumbnailSidebar.buttonHeight;

    this->button->setBoundaries(
        getX() + style->ThumbnailSidebar.buttonMargin,
        getY() + getHeight() - style->ThumbnailSidebar.marginTopBottom - buttonHeight,
        buttonWidth,
        buttonHeight);
}

View* ThumbnailSidebar::getDefaultFocus()
{
    return this->button->getDefaultFocus();
}

void ThumbnailSidebar::setThumbnail(std::string imagePath)
{
    if (this->image)
    {
        this->image->setImage(imagePath);
    }
    else
    {
        this->image = new Image(imagePath);
        this->image->setParent(this);
        this->invalidate();
    }
}

void ThumbnailSidebar::setThumbnail(unsigned char* buffer, size_t bufferSize)
{
    if (this->image)
    {
        this->image->setImage(buffer, bufferSize);
    }
    else
    {
        this->image = new Image(buffer, bufferSize);
        this->image->setParent(this);
        this->invalidate();
    }
}

void ThumbnailSidebar::setSubtitle(std::string subTitle)
{
    if (!this->subTitle)
    {
        this->subTitle = new Label(LabelStyle::DESCRIPTION, "");
        this->subTitle->setParent(this);
    }

    this->subTitle->setText(subTitle);

    this->invalidate();
}

// TODO: Add ellipsis if the title exceeds three lines
void ThumbnailSidebar::setTitle(std::string title)
{
    if (!this->title)
    {
        this->title = new Label(LabelStyle::REGULAR, "", true);
        this->title->setParent(this);
    }

    this->title->setText(title);

    this->invalidate();
}

ThumbnailSidebar::~ThumbnailSidebar()
{
    if (this->image)
        delete this->image;

    if (this->title)
        delete this->title;

    if (this->subTitle)
        delete this->subTitle;

    delete this->button;
}

} // namespace brls
