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

#include <borealis/table.hpp>

namespace brls
{

TableRow* Table::addRow(TableRowType type, std::string label, std::string value)
{
    TableRow* row = new TableRow(type, label, value);
    this->rows.push_back(row);
    return row;
}

void Table::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    ThemeValues* theme = ctx->theme;
    unsigned yAdvance  = 0;

    for (size_t i = 0; i < this->rows.size(); i++)
    {
        bool even     = i % 2 == 0;
        TableRow* row = this->rows[i];

        // Get appearance
        unsigned indent   = 0;
        unsigned height   = 0;
        unsigned fontSize = 0;
        NVGcolor backgroundColor;
        NVGcolor textColor;

        switch (row->getType())
        {
            case TableRowType::HEADER:
                indent    = 0;
                height    = style->TableRow.headerHeight;
                textColor = theme->textColor;
                fontSize  = style->TableRow.headerTextSize;
                break;
            case TableRowType::BODY:
                indent    = style->TableRow.bodyIndent;
                height    = style->TableRow.bodyHeight;
                textColor = theme->tableBodyTextColor;
                fontSize  = style->TableRow.bodyTextSize;
                break;
        }

        backgroundColor = even ? theme->tableEvenBackgroundColor : transparent;

        // Background
        nvgFillColor(vg, a(backgroundColor));
        nvgBeginPath(vg);
        nvgRect(vg, x + indent, y + yAdvance, width - indent, height);
        nvgFill(vg);

        // Text
        nvgFillColor(vg, a(textColor));
        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgFontSize(vg, fontSize);

        // Label
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgBeginPath(vg);
        nvgText(vg, x + indent + style->TableRow.padding, y + yAdvance + height / 2, row->getLabel()->c_str(), nullptr);

        // Value
        nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgBeginPath(vg);
        nvgText(vg, x + width - style->TableRow.padding, y + yAdvance + height / 2, row->getValue()->c_str(), nullptr);

        yAdvance += height;
    }
}

void Table::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    unsigned height = 0;

    for (TableRow* row : this->rows)
    {
        switch (row->getType())
        {
            case TableRowType::HEADER:
                height += style->TableRow.headerHeight;
                break;
            case TableRowType::BODY:
                height += style->TableRow.bodyHeight;
                break;
        }
    }

    this->setHeight(height);
}

Table::~Table()
{
    for (TableRow* row : this->rows)
    {
        delete row;
    }
}

TableRow::TableRow(TableRowType type, std::string label, std::string value)
    : type(type)
    , label(label)
    , value(value)
{
}

std::string* TableRow::getLabel()
{
    return &this->label;
}

std::string* TableRow::getValue()
{
    return &this->value;
}

void TableRow::setValue(std::string value)
{
    this->value = value;
}

TableRowType TableRow::getType()
{
    return this->type;
}

} // namespace brls
