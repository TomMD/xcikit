// Interpreter.cpp created on 2019-06-21, part of XCI toolkit
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

#include "Interpreter.h"

namespace xci::script {


void Interpreter::add_module(const std::string& name, std::string_view content)
{
    // parse
    ast::Module ast;
    m_parser.parse(content, ast);

    // compile
    m_compiler.add_module(name, ast);
}


std::unique_ptr<Value> Interpreter::eval(std::string_view input, const InvokeCallback& cb)
{
    // parse
    ast::Module ast;
    m_parser.parse(input, ast);

    // compile
    auto& symtab = m_compiler.main_module().symtab().add_child("<input>");
    auto func = std::make_unique<Function>(m_compiler.main_module(), symtab);
    //Pointer<Function> func = m_compiler.module().add_function(std::move(fn));
    m_compiler.compile(*func, ast);

    // execute
    m_machine.call(*func, cb);

    // get result from stack
    return m_machine.stack().pull(func->signature().return_type);
}


} // namespace xci::script
