/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv
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
#include <borealis/image.hpp>
#include <cstring>

namespace brls
{

Image::Image(std::string imagePath)
{
    this->setImage(imagePath);
    this->setOpacity(1.0F);
}

Image::Image(unsigned char* buffer, size_t bufferSize)
{
    this->setImage(buffer, bufferSize);
    this->setOpacity(1.0F);
}

Image::~Image()
{
    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;

    if (this->texture != -1)
        nvgDeleteImage(Application::getNVGContext(), this->texture);
}

void Image::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    nvgSave(vg);

    if (this->texture != -1)
    {
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x + this->imageX, y + this->imageY, this->imageWidth, this->imageHeight, this->cornerRadius);
        nvgFillPaint(vg, a(this->imgPaint));
        nvgFill(vg);
    }

    nvgRestore(vg);
}

void Image::reloadTexture()
{
    NVGcontext* vg = Application::getNVGContext();

    if (this->texture != -1)
        nvgDeleteImage(vg, this->texture);

    if (!this->imagePath.empty())
        this->texture = nvgCreateImage(vg, this->imagePath.c_str(), 0);
    else if (this->imageBuffer != nullptr)
        this->texture = nvgCreateImageMem(vg, 0, this->imageBuffer, this->imageBufferSize);
}

void Image::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    if (this->origViewWidth == 0 || this->origViewHeight == 0)
    {
        this->origViewWidth  = this->getWidth();
        this->origViewHeight = this->getHeight();
    }

    nvgImageSize(vg, this->texture, &this->imageWidth, &this->imageHeight);

    this->setWidth(this->origViewWidth);
    this->setHeight(this->origViewHeight);

    this->imageX = 0;
    this->imageY = 0;

    float viewAspectRatio  = static_cast<float>(this->getWidth()) / static_cast<float>(this->getHeight());
    float imageAspectRatio = static_cast<float>(this->imageWidth) / static_cast<float>(this->imageHeight);

    switch (imageScaleType)
    {
        case ImageScaleType::NO_RESIZE:
            this->imageX = (this->origViewWidth - this->imageWidth) / 2.0F;
            this->imageY = (this->origViewHeight - this->imageHeight) / 2.0F;
            break;
        case ImageScaleType::FIT:
            if (viewAspectRatio >= imageAspectRatio)
            {
                this->imageHeight = this->getHeight();
                this->imageWidth  = this->imageHeight * imageAspectRatio;
                this->imageX      = (this->origViewWidth - this->imageWidth) / 2.0F;
            }
            else
            {
                this->imageWidth  = this->getWidth();
                this->imageHeight = this->imageWidth * imageAspectRatio;
                this->imageY      = (this->origViewHeight - this->imageHeight) / 2.0F;
            }
            break;
        case ImageScaleType::CROP:
            if (viewAspectRatio < imageAspectRatio)
            {
                this->imageHeight = this->getHeight();
                this->imageWidth  = this->imageHeight * imageAspectRatio;
                this->imageX      = (this->origViewWidth - this->imageWidth) / 2.0F;
            }
            else
            {
                this->imageWidth  = this->getWidth();
                this->imageHeight = this->imageWidth * imageAspectRatio;
                this->imageY      = (this->origViewHeight - this->imageHeight) / 2.0F;
            }
            break;
        case ImageScaleType::SCALE:
            this->imageWidth  = this->getWidth();
            this->imageHeight = this->getHeight();
            break;
        case ImageScaleType::VIEW_RESIZE:
            this->setWidth(this->imageWidth);
            this->setHeight(this->imageHeight);
            break;
    }

    this->imgPaint = nvgImagePattern(vg, getX() + this->imageX, getY() + this->imageY, this->imageWidth, this->imageHeight, 0, this->texture, this->alpha);
}

void Image::setImage(unsigned char* buffer, size_t bufferSize)
{
    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;

    this->imagePath = "";

    this->imageBuffer = new unsigned char[bufferSize];
    std::memcpy(this->imageBuffer, buffer, bufferSize);
    this->imageBufferSize = bufferSize;

    this->reloadTexture();
    this->invalidate();
}

void Image::setImage(std::string imagePath)
{
    this->imagePath = imagePath;

    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;

    this->imageBuffer = nullptr;

    this->reloadTexture();

    this->invalidate();
}

void Image::setOpacity(float opacity)
{
    this->alpha = opacity;
    this->invalidate();
}

void Image::setScaleType(ImageScaleType imageScaleType)
{
    this->imageScaleType = imageScaleType;
    this->invalidate();
}

} // namespace brls
