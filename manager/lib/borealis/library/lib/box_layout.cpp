/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
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

#include <math.h>
#include <stdio.h>

#include <borealis/animations.hpp>
#include <borealis/application.hpp>
#include <borealis/box_layout.hpp>
#include <borealis/logger.hpp>
#include <iterator>

namespace brls
{

BoxLayout::BoxLayout(BoxLayoutOrientation orientation, size_t defaultFocus)
    : orientation(orientation)
    , originalDefaultFocus(defaultFocus)
    , defaultFocusedIndex(defaultFocus)
{
}

void BoxLayout::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    // Draw children
    for (BoxLayoutChild* child : this->children)
        child->view->frame(ctx);
}

void BoxLayout::setGravity(BoxLayoutGravity gravity)
{
    this->gravity = gravity;
    this->invalidate();
}

void BoxLayout::setSpacing(unsigned spacing)
{
    this->spacing = spacing;
    this->invalidate();
}

unsigned BoxLayout::getSpacing()
{
    return this->spacing;
}

void BoxLayout::setMargins(unsigned top, unsigned right, unsigned bottom, unsigned left)
{
    this->marginBottom = bottom;
    this->marginLeft   = left;
    this->marginRight  = right;
    this->marginTop    = top;
    this->invalidate();
}

void BoxLayout::setMarginBottom(unsigned bottom)
{
    this->marginBottom = bottom;
    this->invalidate();
}

size_t BoxLayout::getViewsCount()
{
    return this->children.size();
}

View* BoxLayout::getDefaultFocus()
{
    // Focus default focus first
    if (this->defaultFocusedIndex < this->children.size())
    {
        View* newFocus = this->children[this->defaultFocusedIndex]->view->getDefaultFocus();

        if (newFocus)
            return newFocus;
    }

    // Fallback to finding the first focusable view
    for (size_t i = 0; i < this->children.size(); i++)
    {
        View* newFocus = this->children[i]->view->getDefaultFocus();

        if (newFocus)
            return newFocus;
    }

    return nullptr;
}

View* BoxLayout::getNextFocus(FocusDirection direction, void* parentUserData)
{
    // Return nullptr immediately if focus direction mismatches the layout direction
    if ((this->orientation == BoxLayoutOrientation::HORIZONTAL && direction != FocusDirection::LEFT && direction != FocusDirection::RIGHT) || (this->orientation == BoxLayoutOrientation::VERTICAL && direction != FocusDirection::UP && direction != FocusDirection::DOWN))
    {
        return nullptr;
    }

    // Traverse the children
    size_t offset = 1; // which way are we going in the children list

    if ((this->orientation == BoxLayoutOrientation::HORIZONTAL && direction == FocusDirection::LEFT) || (this->orientation == BoxLayoutOrientation::VERTICAL && direction == FocusDirection::UP))
    {
        offset = -1;
    }

    size_t currentFocusIndex = *((size_t*)parentUserData) + offset;
    View* currentFocus       = nullptr;

    while (!currentFocus && currentFocusIndex >= 0 && currentFocusIndex < this->children.size())
    {
        currentFocus = this->children[currentFocusIndex]->view->getDefaultFocus();
        currentFocusIndex += offset;
    }

    return currentFocus;
}

void BoxLayout::removeView(int index, bool free)
{
    BoxLayoutChild* toRemove = this->children[index];
    toRemove->view->willDisappear(true);
    if (free)
        delete toRemove->view;
    delete toRemove;
    this->children.erase(this->children.begin() + index);
}

void BoxLayout::clear(bool free)
{
    while (!this->children.empty())
        this->removeView(0, free);
}

void BoxLayout::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    // Vertical orientation
    if (this->orientation == BoxLayoutOrientation::VERTICAL)
    {
        unsigned entriesHeight = 0;
        int yAdvance           = this->y + this->marginTop;

        for (size_t i = 0; i < this->children.size(); i++)
        {
            BoxLayoutChild* child = this->children[i];
            unsigned childHeight  = child->view->getHeight();

            if (child->fill)
                child->view->setBoundaries(this->x + this->marginLeft,
                    yAdvance,
                    this->width - this->marginLeft - this->marginRight,
                    this->y + this->height - yAdvance - this->marginBottom);
            else
                child->view->setBoundaries(this->x + this->marginLeft,
                    yAdvance,
                    this->width - this->marginLeft - this->marginRight,
                    child->view->getHeight(false));

            child->view->invalidate(true); // call layout directly in case height is updated
            childHeight = child->view->getHeight();

            int spacing = (int)this->spacing;
            View* next  = (this->children.size() > 1 && i <= this->children.size() - 2) ? this->children[i + 1]->view : nullptr;

            this->customSpacing(child->view, next, &spacing);

            if (child->view->isCollapsed())
                spacing = 0;

            if (!child->view->isHidden())
                entriesHeight += spacing + childHeight;

            yAdvance += spacing + childHeight;
        }

        // TODO: apply gravity

        // Update height if needed
        if (this->resize)
            this->setHeight(entriesHeight - spacing + this->marginTop + this->marginBottom);
    }
    // Horizontal orientation
    else if (this->orientation == BoxLayoutOrientation::HORIZONTAL)
    {
        // Layout
        int xAdvance = this->x + this->marginLeft;
        for (size_t i = 0; i < this->children.size(); i++)
        {
            BoxLayoutChild* child = this->children[i];
            unsigned childWidth   = child->view->getWidth();

            if (child->fill)
                child->view->setBoundaries(xAdvance,
                    this->y + this->marginTop,
                    this->x + this->width - xAdvance - this->marginRight,
                    this->height - this->marginTop - this->marginBottom);
            else
                child->view->setBoundaries(xAdvance,
                    this->y + this->marginTop,
                    childWidth,
                    this->height - this->marginTop - this->marginBottom);

            child->view->invalidate(true); // call layout directly in case width is updated
            childWidth = child->view->getWidth();

            int spacing = (int)this->spacing;

            View* next = (this->children.size() > 1 && i <= this->children.size() - 2) ? this->children[i + 1]->view : nullptr;

            this->customSpacing(child->view, next, &spacing);

            if (child->view->isCollapsed())
                spacing = 0;

            xAdvance += spacing + childWidth;
        }

        // Apply gravity
        // TODO: more efficient gravity implementation?
        if (!this->children.empty())
        {
            switch (this->gravity)
            {
                case BoxLayoutGravity::RIGHT:
                {
                    // Take the remaining empty space between the last view's
                    // right boundary and ours and push all views by this amount
                    View* lastView = this->children[this->children.size() - 1]->view;

                    unsigned lastViewRight = lastView->getX() + lastView->getWidth();
                    unsigned ourRight      = this->getX() + this->getWidth();

                    if (lastViewRight <= ourRight)
                    {
                        unsigned difference = ourRight - lastViewRight;

                        for (BoxLayoutChild* child : this->children)
                        {
                            View* view = child->view;
                            view->setBoundaries(
                                view->getX() + difference,
                                view->getY(),
                                view->getWidth(),
                                view->getHeight());
                            view->invalidate();
                        }
                    }

                    break;
                }
                default:
                    break;
            }
        }

        // TODO: update width if needed (introduce entriesWidth)
    }
}

void BoxLayout::setResize(bool resize)
{
    this->resize = resize;
    this->invalidate();
}

void BoxLayout::addView(View* view, bool fill, bool resetState)
{
    BoxLayoutChild* child = new BoxLayoutChild();
    child->view           = view;
    child->fill           = fill;

    this->children.push_back(child);

    size_t position = this->children.size() - 1;

    size_t* userdata = (size_t*)malloc(sizeof(size_t));
    *userdata        = position;

    view->setParent(this, userdata);

    view->willAppear(resetState);
    this->invalidate();
}

View* BoxLayout::getChild(size_t index)
{
    return this->children[index]->view;
}

bool BoxLayout::isEmpty()
{
    return this->children.size() == 0;
}

bool BoxLayout::isChildFocused()
{
    return this->childFocused;
}

void BoxLayout::onChildFocusGained(View* child)
{
    this->childFocused = true;

    // Remember focus if needed
    if (this->rememberFocus)
    {
        size_t index              = *((size_t*)child->getParentUserData());
        this->defaultFocusedIndex = index;
    }

    View::onChildFocusGained(child);
}

void BoxLayout::onChildFocusLost(View* child)
{
    this->childFocused = false;

    View::onChildFocusLost(child);
}

BoxLayout::~BoxLayout()
{
    for (BoxLayoutChild* child : this->children)
    {
        child->view->willDisappear(true);
        delete child->view;
        delete child;
    }

    this->children.clear();
}

void BoxLayout::willAppear(bool resetState)
{
    for (BoxLayoutChild* child : this->children)
        child->view->willAppear(resetState);
}

void BoxLayout::willDisappear(bool resetState)
{
    for (BoxLayoutChild* child : this->children)
        child->view->willDisappear(resetState);

    // Reset default focus to original one if needed
    if (this->rememberFocus)
        this->defaultFocusedIndex = this->originalDefaultFocus;
}

void BoxLayout::onWindowSizeChanged()
{
    for (BoxLayoutChild* child : this->children)
        child->view->onWindowSizeChanged();
}

void BoxLayout::setRememberFocus(bool remember)
{
    this->rememberFocus = remember;
}

} // namespace brls
