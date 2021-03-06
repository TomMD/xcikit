// Element.h created on 2018-03-18, part of XCI toolkit
// Copyright 2018 Radek Brich
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef XCI_TEXT_LAYOUT_PAGE_ELEMENT_H
#define XCI_TEXT_LAYOUT_PAGE_ELEMENT_H

#include "Page.h"

#include <string>

namespace xci::text::layout {


class Element {
public:
    virtual ~Element() = default;
    virtual void apply(Page& page) = 0;
};


// ------------------------------------------------------------------------
// Control elements - change page attributes


class SetPageWidth: public Element {
public:
    explicit SetPageWidth(ViewportUnits width) : m_width(width) {}
    void apply(Page& page) override {
        page.set_width(m_width);
    }

private:
    float m_width;
};


class SetAlignment: public Element {
public:
    explicit SetAlignment(Alignment alignment) : m_alignment(alignment) {}
    void apply(Page& page) override {
        page.set_alignment(m_alignment);
    }

private:
    Alignment m_alignment;
};


class AddTabStop: public Element {
public:
    explicit AddTabStop(ViewportUnits tab_stop) : m_tab_stop(tab_stop) {}
    void apply(Page& page) override {
        page.add_tab_stop(m_tab_stop);
    }

private:
    float m_tab_stop;
};


class ResetTabStops: public Element {
public:
    void apply(Page& page) override {
        page.reset_tab_stops();
    }
};


class SetOffset: public Element {
public:
    explicit SetOffset(const ViewportSize& offset) : m_offset(offset) {}
    void apply(Page& page) override {
        page.set_pen_offset(m_offset);
    }

private:
    const ViewportSize m_offset;
};


class SetFont: public Element {
public:
    explicit SetFont(Font* font) : m_font(font) {}
    void apply(Page& page) override {
        page.set_font(m_font);
    }

private:
    Font* m_font;
};


class SetFontSize: public Element {
public:
    explicit SetFontSize(ViewportUnits size) : m_size(size) {}
    void apply(Page& page) override {
        page.set_font_size(m_size);
    }

private:
    float m_size;
};


class SetColor: public Element {
public:
    explicit SetColor(graphics::Color color) : m_color(color) {}
    void apply(Page& page) override {
        page.set_color(m_color);
    }

private:
    graphics::Color m_color;
};


// ------------------------------------------------------------------------
// Text elements


// Single word, consisting of letters (glyphs), font and style.
class AddWord: public Element {
public:
    explicit AddWord(std::string string) : m_string(std::move(string)) {}
    void apply(Page& page) override {
        page.add_word(m_string);
    }

private:
    std::string m_string;
};


class AddSpace: public Element {
public:
    void apply(Page& page) override {
        page.add_space();
    }
};


class AddTab: public Element {
public:
    void apply(Page& page) override {
        page.add_tab();
    }
};


class FinishLine: public Element {
public:
    void apply(Page& page) override {
        page.finish_line();
    }
};


class BeginSpan: public Element {
public:
    explicit BeginSpan(std::string name) : m_name(std::move(name)) {}
    void apply(Page& page) override {
        page.begin_span(m_name);
    }

private:
    std::string m_name;
};


class EndSpan: public Element {
public:
    explicit EndSpan(std::string name) : m_name(std::move(name)) {}
    void apply(Page& page) override {
        page.end_span(m_name);
    }

private:
    std::string m_name;
};


} // namespace xci::text::layout

#endif // XCI_TEXT_LAYOUT_PAGE_ELEMENT_H
