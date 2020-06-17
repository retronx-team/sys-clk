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

#include <borealis/box_layout.hpp>
#include <borealis/image.hpp>
#include <borealis/label.hpp>
#include <borealis/rectangle.hpp>
#include <borealis/scroll_view.hpp>
#include <string>

namespace brls
{

// A list item
// TODO: Use a Label with integrated ticker
class ListItem : public View
{
  private:
    std::string label;
    std::string subLabel;
    std::string value;
    bool valueFaint;

    std::string oldValue;
    bool oldValueFaint;
    float valueAnimation = 0.0f;

    bool checked = false; // check mark on the right

    unsigned textSize;

    bool drawTopSeparator = true;

    Label* descriptionView = nullptr;
    Image* thumbnailView   = nullptr;

    bool reduceDescriptionSpacing = false;

    GenericEvent clickEvent;

    bool indented = false;

    void resetValueAnimation();

  public:
    ListItem(std::string label, std::string description = "", std::string subLabel = "");

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;
    void getHighlightInsets(unsigned* top, unsigned* right, unsigned* bottom, unsigned* left) override;
    virtual bool onClick();
    View* getDefaultFocus() override;

    void setThumbnail(Image* image);
    void setThumbnail(std::string imagePath);
    void setThumbnail(unsigned char* buffer, size_t bufferSize);

    bool hasDescription();
    void setDrawTopSeparator(bool draw);

    bool getReduceDescriptionSpacing();
    void setReduceDescriptionSpacing(bool value);

    void setIndented(bool indented);

    void setTextSize(unsigned textSize);

    void setChecked(bool checked);

    std::string getLabel();

    /**
     * Sets the value of this list item
     * (the text on the right)
     * Set faint to true to have the new value
     * use a darker color (typically "OFF" labels)
     */
    void setValue(std::string value, bool faint = false, bool animate = true);
    std::string getValue();

    GenericEvent* getClickEvent();

    ~ListItem();
};

// Some spacing (to make groups of ListItems)
class ListItemGroupSpacing : public Rectangle
{
  public:
    ListItemGroupSpacing(bool separator = false);
};

// A list item with mutliple choices for its value
// (will open a Dropdown)

// Fired when the user has selected a value
//
// Parameter is either the selected value index
// or -1 if the user cancelled
typedef Event<int> ValueSelectedEvent;

class SelectListItem : public ListItem
{
  public:
    SelectListItem(std::string label, std::vector<std::string> values, unsigned selectedValue = 0);

    void setSelectedValue(unsigned value);

    ValueSelectedEvent* getValueSelectedEvent();

  private:
    std::vector<std::string> values;
    unsigned selectedValue;

    ValueSelectedEvent valueEvent;
};

// A list item with a ON/OFF value
// that can be toggled
// Use the click event to detect when the value
// changes
class ToggleListItem : public ListItem
{
  private:
    bool toggleState;
    std::string onValue, offValue;

    void updateValue();

  public:
    ToggleListItem(std::string label, bool initialValue, std::string description = "", std::string onValue = "On", std::string offValue = "Off");

    virtual bool onClick() override;

    bool getToggleState();
};

// A list item which spawns the swkbd
// to input its value (string)
class InputListItem : public ListItem
{
  protected:
    std::string helpText;
    int maxInputLength;

  public:
    InputListItem(std::string label, std::string initialValue, std::string helpText, std::string description = "", int maxInputLength = 32);

    virtual bool onClick() override;
};

// A list item which spawns the swkbd
// to input its value (integer)
class IntegerInputListItem : public InputListItem
{
  public:
    IntegerInputListItem(std::string label, int initialValue, std::string helpText, std::string description = "", int maxInputLength = 32);

    virtual bool onClick() override;
};

class List; // forward declaration for ListContentView::list

// The content view of lists (used internally)
class ListContentView : public BoxLayout
{
  public:
    ListContentView(List* list, size_t defaultFocus = 0);

  protected:
    void customSpacing(View* current, View* next, int* spacing) override;

  private:
    List* list;
};

// A vertical list of various widgets, with proper margins and spacing
// and a scroll bar
// In practice it's a ScrollView which content view is
// a ListContentView (BoxLayout)
class List : public ScrollView
{
  private:
    ListContentView* layout;

  public:
    List(size_t defaultFocus = 0);
    ~List();

    // Wrapped BoxLayout methods
    void addView(View* view, bool fill = false);
    void setMargins(unsigned top, unsigned right, unsigned bottom, unsigned left);
    void setMarginBottom(unsigned bottom);
    void setSpacing(unsigned spacing);
    unsigned getSpacing();
    virtual void customSpacing(View* current, View* next, int* spacing);
};

} // namespace brls
