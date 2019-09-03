// Module.h created on 2019-06-12, part of XCI toolkit
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

#ifndef XCI_SCRIPT_MODULE_H
#define XCI_SCRIPT_MODULE_H

#include "Value.h"
#include "SymbolTable.h"
#include <cstdint>

namespace xci::script {


/// Module is the translation unit - it contains functions and constants

class Module {
public:
    Module() : m_symtab("<module>") { m_symtab.set_module(this); }

    // Imported modules
    void add_imported_module(Module& module) { m_modules.push_back(&module); }
    Module& get_imported_module(size_t idx) const { return *m_modules[idx]; }
    Index get_imported_module_index(Module* module) const;
    size_t num_imported_modules() const { return m_modules.size(); }

    // Functions
    Index add_function(std::unique_ptr<Function>&& fn);
    Function& get_function(size_t idx) const { return *m_functions[idx]; }
    Function& get_last_function() { return *m_functions.back(); }

    // Static values
    Index add_value(std::unique_ptr<Value>&& value);
    const Value& get_value(Index idx) const { return m_values[idx]; }

    // Type information
    Index add_type(TypeInfo type_info);
    const TypeInfo& get_type(Index idx) const { return m_types[idx]; }

    // Top-level symbol table
    SymbolTable& symtab() { return m_symtab; }

    bool operator==(const Module& rhs) const;

private:
    std::vector<Module*> m_modules;
    std::vector<std::unique_ptr<Function>> m_functions;
    std::vector<TypeInfo> m_types;
    Values m_values;
    SymbolTable m_symtab;
};


} // namespace xci::script

#endif // include guard