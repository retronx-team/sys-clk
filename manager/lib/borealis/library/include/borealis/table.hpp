/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
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

#include <borealis/view.hpp>
#include <string>
#include <vector>

namespace brls
{

// The type of a Table Row
enum class TableRowType
{
    HEADER = 0,
    BODY
};

// A Table row
class TableRow
{
  private:
    TableRowType type;

    std::string label;
    std::string value;

  public:
    TableRow(TableRowType type, std::string label, std::string value = "");

    std::string* getLabel();
    std::string* getValue();
    TableRowType getType();

    void setValue(std::string value);
};

// A simple, static two-columns table, as seen in
// the Settings app (Internet connection details)
// All rows must be added before adding the view
// to a layout (it's static)
class Table : public View
{
  private:
    std::vector<TableRow*> rows;

  public:
    ~Table();

    void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx) override;
    void layout(NVGcontext* vg, Style* style, FontStash* stash) override;

    TableRow* addRow(TableRowType type, std::string label, std::string value = "");
};

} // namespace brls
