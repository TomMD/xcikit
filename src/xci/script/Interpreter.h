// Interpreter.h created on 2019-06-21, part of XCI toolkit
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

#ifndef XCI_SCRIPT_INTERPRETER_H
#define XCI_SCRIPT_INTERPRETER_H

#include "Parser.h"
#include "Compiler.h"
#include "Machine.h"
#include "Builtin.h"
#include <string_view>

namespace xci::script {


/// High level interpreter

class Interpreter {
public:
    Interpreter() : Interpreter(0) {}
    explicit Interpreter(uint32_t flags);

    // `flags` are Compiler::Flags
    void configure(uint32_t flags) { m_compiler.set_flags(flags); }

    std::unique_ptr<Module> build_module(const std::string& name, std::string_view content);
    void add_imported_module(Module& module) { m_main.add_imported_module(module); }

    using InvokeCallback = Machine::InvokeCallback;
    std::unique_ptr<Value> eval(std::string_view input, const InvokeCallback& cb = [](const Value&){});

    // low-level component access
    Parser& parser() { return m_parser; }
    Compiler& compiler() { return m_compiler; }
    Machine& machine() { return m_machine; }
    Module& main_module() { return m_main; }

private:
    Parser m_parser;
    Compiler m_compiler;
    Machine m_machine;
    Module m_main;
};


} // namespace xci::script

#endif // include guard
