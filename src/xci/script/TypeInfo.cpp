// TypeInfo.cpp created on 2019-06-09, part of XCI toolkit
// Copyright 2019 Radek Brich
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

#include "TypeInfo.h"
#include "Error.h"
#include <xci/compat/macros.h>
#include <numeric>

using namespace std;

namespace xci::script {


size_t TypeInfo::size() const
{
    switch (type()) {
        case Type::Unknown:     return 0;
        case Type::Void:        return 1;
        case Type::Bool:        return 1;
        case Type::Byte:        return 1;
        case Type::Char:        return 4;
        case Type::Int32:       return 4;
        case Type::Int64:       return 8;
        case Type::Float32:     return 4;
        case Type::Float64:     return 8;
        case Type::String:      return sizeof(byte*) + sizeof(size_t);
        case Type::List:        return sizeof(byte*) + sizeof(size_t);
        case Type::Tuple:
            return accumulate(m_subtypes.begin(), m_subtypes.end(), 0,
                              [](size_t init, const TypeInfo& ti)
                              { return init + ti.size(); });
        case Type::Function:    return sizeof(byte*) + sizeof(void*);
        case Type::Module:      return 0;  // TODO
    }
    return 0;
}


void TypeInfo::foreach_heap_slot(std::function<void(size_t offset)> cb) const
{
    switch (type()) {
        case Type::String:      cb(0); break;
        case Type::Function:    cb(0); break;
        case Type::List:        cb(0); break;
        case Type::Tuple: {
            size_t pos = 0;
            for (const auto& ti : m_subtypes) {
                ti.foreach_heap_slot([&cb, pos](size_t offset) {
                    cb(pos + offset);
                });
                pos += ti.size();
            }
            break;
        }
        default: break;
    }
}


const TypeInfo& TypeInfo::elem_type() const
{
    assert(m_type == Type::List);
    return m_subtypes[0];
}


bool TypeInfo::operator==(const TypeInfo& rhs) const
{
    if (m_type == Type::Unknown || rhs.type() == Type::Unknown)
        return true;  // unknown type matches all other types
    if (m_type != rhs.type())
        return false;
    if (m_type == Type::Function)
        return *m_signature == *rhs.m_signature;
    if (m_type == Type::Tuple || m_type == Type::List)
        return m_subtypes == rhs.m_subtypes;
    return true;
}


std::ostream& operator<<(std::ostream& os, const TypeInfo& v)
{
    switch (v.type()) {
        case Type::Unknown:     return os << "?";
        case Type::Void:        return os << "Void";
        case Type::Bool:        return os << "Bool";
        case Type::Byte:        return os << "Byte";
        case Type::Char:        return os << "Char";
        case Type::Int32:       return os << "Int32";
        case Type::Int64:       return os << "Int64";
        case Type::Float32:     return os << "Float32";
        case Type::Float64:     return os << "Float64";
        case Type::String:      return os << "String";
        case Type::List:
            return os << "[" << v.subtypes()[0] << "]";
        case Type::Tuple: {
            os << "(";
            for (const auto& ti : v.subtypes()) {
                os << ti;
                if (&ti != &v.subtypes().back())
                    os << ", ";
            }
            return os << ")";
        }
        case Type::Function:    return os << v.signature();
        case Type::Module:      return os << "Module";
    }
    UNREACHABLE;
}


void Signature::resolve_return_type(const TypeInfo& t)
{
    assert(!t.is_generic());
    if (return_type.is_generic())
        return_type = t;
    if (return_type != t)
        throw UnexpectedReturnType(return_type, t);
}


std::ostream& operator<<(std::ostream& os, const Signature& v)
{
    if (!v.params.empty()) {
        os << "| ";
        for (auto& param : v.params) {
            os << param << " ";
        }
        os << "| -> ";
    }
    return os << v.return_type;
}


} // namespace xci::script