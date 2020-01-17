// Function.h created on 2019-05-30, part of XCI toolkit
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

#ifndef XCI_SCRIPT_FUNCTION_H
#define XCI_SCRIPT_FUNCTION_H

#include "Code.h"
#include "ast/AST.h"
#include "SymbolTable.h"
#include "TypeInfo.h"
#include "NativeDelegate.h"
#include <map>
#include <string>
#include <variant>

namespace xci::script {

class Module;
class Stack;

// Scope of names and values
//
// Calling convention:
// - function args (in reversed order, first on top)
// - nonlocals (in reversed order, first on top)
// - base pointer points above args and nonlocals (first local var)
// - local variables


// Function bytecode:
// - caller:
//      - pushes args on stack in reversed order (first arg on top)
// - callee (the function itself):
//      - local definition are pushed on stack (last on top)
//      - operations referencing local defs or params use
//        COPY opcode to retrieve value from stack and push it on top

class Function {
public:
    explicit Function(Module& module, SymbolTable& symtab);

    const std::string& name() const { return m_symtab.name(); }

    // module containing this function
    Module& module() const { return m_module; }

    // symbol table with names used in function scope
    SymbolTable& symtab() const { return m_symtab; }

    // parameters
    void add_parameter(std::string name, TypeInfo&& type_info);
    bool has_parameters() const { return !m_signature->params.empty(); }
    const TypeInfo& parameter(Index idx) const { return m_signature->params[idx]; }
    const std::vector<TypeInfo>& parameters() const { return m_signature->params; }
    size_t raw_size_of_parameters() const;
    size_t parameter_offset(Index idx) const;

    // function signature
    void set_signature(const std::shared_ptr<Signature>& newsig) { m_signature = newsig; }
    std::shared_ptr<Signature> signature_ptr() const { return m_signature; }
    Signature& signature() { return *m_signature; }
    const Signature& signature() const { return *m_signature; }

    // effective return type
    TypeInfo effective_return_type() const { return m_signature->return_type.effective_type(); }

    // compiled function body
    Code& code() { return std::get<NormalBody>(m_body).code; }
    const Code& code() const { return std::get<NormalBody>(m_body).code; }

    // Special intrinsics function cannot contain any compiled code and is always inlined.
    // This counter helps to check no other code was generated.
    void add_intrinsic(uint8_t code) { std::get<NormalBody>(m_body).intrinsics++;
        std::get<NormalBody>(m_body).code.add(code); }
    size_t intrinsics() const { return std::get<NormalBody>(m_body).intrinsics; }
    bool has_intrinsics() const { return std::get<NormalBody>(m_body).intrinsics > 0; }

    // Generic function: AST of function body
    const ast::Block& ast() const { return std::get<GenericBody>(m_body).ast; }
    ast::Block& ast() { return std::get<GenericBody>(m_body).ast; }
    void copy_ast(const ast::Block& body) { m_body = GenericBody{body.copy()}; }

    // non-locals
    void add_nonlocal(TypeInfo&& type_info);
    bool has_nonlocals() const { return !m_signature->nonlocals.empty(); }
    const std::vector<TypeInfo>& nonlocals() const { return m_signature->nonlocals; }
    size_t raw_size_of_nonlocals() const;  // size of all nonlocals in bytes
    std::pair<size_t, TypeInfo> nonlocal_offset_and_type(Index idx) const;

    // partial call (bound args)
    void add_partial(TypeInfo&& type_info);
    const std::vector<TypeInfo>& partial() const { return m_signature->partial; }
    size_t raw_size_of_partial() const;

    // whole closure = nonlocals + partial
    size_t raw_size_of_closure() const { return raw_size_of_nonlocals() + raw_size_of_partial(); }
    size_t closure_size() const { return nonlocals().size() + partial().size(); }
    std::vector<TypeInfo> closure() const;

    // true if this function should be generic (i.e. signature contains a type variable)
    bool detect_generic() const;

    // Kind of function body

    // function has code and will be called or inlined
    struct NormalBody {
        bool operator==(const NormalBody& rhs) const;

        // Compiled function body
        Code code;
        // Counter for instructions from intrinsics
        size_t intrinsics = 0;
        // If function should be inlined at call site
        bool is_fragment = false;
    };

    // function is a template, signature contains type variables
    struct GenericBody {
        bool operator==(const GenericBody& rhs) const;

        // AST of function body (only for generic function)
        ast::Block ast;
    };

    // function wraps native function (C++ binding)
    struct NativeBody {
        bool operator==(const NativeBody& rhs) const;

        NativeDelegate native;
    };

    void set_normal() { m_body = NormalBody{}; }
    void set_fragment() { m_body = NormalBody{{}, 0, true}; }

    void set_native(NativeDelegate native) { m_body = NativeBody{native}; }
    void call_native(Stack& stack) const { std::get<NativeBody>(m_body).native(stack); }

    bool is_undefined() const { return std::holds_alternative<std::monostate>(m_body); }
    bool is_normal() const { return std::holds_alternative<NormalBody>(m_body); }
    bool is_fragment() const { return std::holds_alternative<NormalBody>(m_body) && std::get<NormalBody>(m_body).is_fragment; }
    bool is_generic() const { return std::holds_alternative<GenericBody>(m_body); }
    bool is_native() const { return std::holds_alternative<NativeBody>(m_body); }

    enum class Kind {
        Undefined,
        Normal,
        Generic,
        Native,
    };
    Kind kind() const { return Kind(m_body.index()); }

    bool operator==(const Function& rhs) const;

private:
    Module& m_module;
    SymbolTable& m_symtab;
    // Function signature
    std::shared_ptr<Signature> m_signature;
    // Function body (depending on kind of function)
    std::variant<std::monostate, NormalBody, GenericBody, NativeBody> m_body;
};


} // namespace xci::script

#endif // include guard
