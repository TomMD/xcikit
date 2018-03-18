// Layout.h created on 2018-03-10, part of XCI toolkit
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

#ifndef XCI_TEXT_LAYOUT_H
#define XCI_TEXT_LAYOUT_H

#include "layout/Page.h"
#include "layout/Element.h"
#include "Font.h"
#include "Style.h"
#include "../graphics/View.h"
#include "../graphics/Color.h"
#include "../util/geometry.h"

#include <string>
#include <utility>
#include <vector>

namespace xci {
namespace text {
namespace layout {


// Group of words, allowing mass editing and providing a bounding box
class Span
{
public:
    static constexpr ElementIndex invalid_index = ~0u;

    explicit Span(ElementIndex begin) : m_begin(begin) {}

    void set_end(ElementIndex end) { m_end = end; }

    ElementIndex begin_index() const { return m_begin; }

    ElementIndex end_index() const { return m_end; }

    bool is_empty() const { return m_begin == m_end; }

    bool is_open() const
    {
        return m_begin != invalid_index && m_end == invalid_index;
    }

    // Restyle all words in span
    //void set_color(const graphics::Color& color);

    // Retrieve adjusted (global) bounding box of the span.
    util::Rect_f get_bounds() const;

    // add padding to bounds
    void add_padding(float radius);

private:
    friend class Page;

    ElementIndex m_begin;
    ElementIndex m_end = invalid_index;  // points after last work in STL fashion
    util::Rect_f m_bounds;
};


// Layout allows to record a stream of elements (text and control)
// and then apply this stream of elements to generate precise positions
// bounding boxes according to current View, and draw them into the View.
class Layout
{
public:
    Layout() : m_page(*this) {}

    // Clear all contents.
    void clear();

    // ------------------------------------------------------------------------
    // Control elements
    //
    // The following methods add control elements into the stream.
    // The new state will affect text elements added after this.

    // Set page width. This drives the line breaking.
    // Default: 0 (same as INF - no line breaking)
    void set_page_width(float width);

    // Set text alignment
    void set_alignment(Alignment alignment);

    // Horizontal tab stops. Following Tab elements will add horizontal space
    // up to next tab stop.
    void add_tab_stop(float x);
    void reset_tab_stops();

    // Set font and text style.
    // Also affects spacing (which depends on font metrics).
    void set_font(Font* font);
    void set_font_size(float size);
    void set_color(const graphics::Color &color);

    // ------------------------------------------------------------------------
    // Text elements

    // Word should be actual word. Punctuation can be attached to it
    // or pushed separately as another "word". No whitespace should be contained
    // in the word, unless it is meant to behave as hard, unbreakable space.
    void add_word(const std::string& string);

    // Add a space after last word. Does nothing if current line is empty.
    void add_space();

    // Put horizontal tab onto line. It takes all space up to next tabstop.
    void add_tab();

    // Finish current line, apply alignment and move to new one.
    // Does nothing if current line is empty.
    void finish_line();

    // ------------------------------------------------------------------------
    // Spans allow to name part of the text and change its attributes later

    friend struct Span;

    // Begin and end the span.
    // Returns false on error:
    // - Trying to begin a span of same name twice.
    // - Trying to end not-started span.
    bool begin_span(const std::string& name);

    bool end_span(const std::string& name);

    // Returns NULL if the span does not exist.
    Span* get_span(const std::string& name);

    // ------------------------------------------------------------------------
    // Typeset and draw

    // Typeset the element stream for the target, ie. compute element
    // positions and sizes.
    // Should be called on every change of framebuffer size
    // and after addition of new elements.
    void typeset(const graphics::View& target);

    // Draw whole layout to target
    void draw(graphics::View& target, const util::Vec2f& pos) const;

    // ------------------------------------------------------------------------
    // Debug

#ifndef NDEBUG
    bool d_show_bounds = false;
#endif

private:
    friend class Page;
    Page m_page;
    std::vector<std::unique_ptr<Element>> m_elements;
    std::map<std::string, Span> m_spans;
};


} // namespace layout

// Export Layout into xci::text namespace
using layout::Layout;

}} // namespace xci::text

#endif // XCI_TEXT_LAYOUT_H