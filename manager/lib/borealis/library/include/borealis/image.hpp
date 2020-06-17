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

#pragma once

#include <borealis/frame_context.hpp>
#include <borealis/view.hpp>

namespace brls
{

enum class ImageScaleType
{
    NO_RESIZE = 0, // Nothing is resized
    FIT, // The image is shrinked to fit the view boundaries
    CROP, // The image is not resized but is cropped if bigger than the view
    SCALE, // The image is stretched to match the view boundaries
    VIEW_RESIZE // The view is resized to match the image
};

// An image
class Image : public View
{
  public:
    Image(std::string imagePath);
    Image(unsigned char* buffer, size_t bufferSize);
    ~Image();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

    void setImage(unsigned char* buffer, size_t bufferSize);
    void setImage(std::string imagePath);

    void setScaleType(ImageScaleType imageScaleType);
    void setOpacity(float opacity);

    void setCornerRadius(float radius)
    {
        this->cornerRadius = radius;
    }

  private:
    std::string imagePath;
    unsigned char* imageBuffer = nullptr;
    size_t imageBufferSize     = 0;

    int texture = -1;
    NVGpaint imgPaint;

    ImageScaleType imageScaleType = ImageScaleType::FIT;

    float cornerRadius = 0;

    int imageX = 0, imageY = 0;
    int imageWidth = 0, imageHeight = 0;
    int origViewWidth = 0, origViewHeight = 0;

    void reloadTexture();
};

} // namespace brls
