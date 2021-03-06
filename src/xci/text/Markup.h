// Markup.h created on 2018-03-10, part of XCI toolkit
// Copyright 2018, 2019 Radek Brich
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

#ifndef XCI_TEXT_MARKUP_H
#define XCI_TEXT_MARKUP_H

#include "Layout.h"

namespace xci::text {


class Markup {
public:
    explicit Markup(Layout &layout) : m_layout(layout) {}
    explicit Markup(Layout &layout, const std::string &s)
        : m_layout(layout) { parse(s); }

    bool parse(const std::string &s);

    Layout& get_layout() { return m_layout; };

private:
    Layout& m_layout;
};


} // namespace xci::text

#endif // XCI_TEXT_MARKUP_H
