// Code.h created on 2019-05-23, part of XCI toolkit
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

#ifndef XCI_SCRIPT_CODE_H
#define XCI_SCRIPT_CODE_H

#include <vector>
#include <ostream>
#include <cstdint>
#include <cstdlib>
#include <cassert>

namespace xci::script {


enum class Opcode: uint8_t {
    // --------------------------------------------------------------
    // No args

    Noop,

    LogicalNot,
    LogicalOr,
    LogicalAnd,

    Equal_8,
    Equal_32,
    Equal_64,
    NotEqual_8,
    NotEqual_32,
    NotEqual_64,
    LessEqual_8,
    LessEqual_32,
    LessEqual_64,
    GreaterEqual_8,
    GreaterEqual_32,
    GreaterEqual_64,
    LessThan_8,
    LessThan_32,
    LessThan_64,
    GreaterThan_8,
    GreaterThan_32,
    GreaterThan_64,

    BitwiseNot_8,
    BitwiseNot_32,
    BitwiseNot_64,
    BitwiseOr_8,
    BitwiseOr_32,
    BitwiseOr_64,
    BitwiseAnd_8,
    BitwiseAnd_32,
    BitwiseAnd_64,
    BitwiseXor_8,
    BitwiseXor_32,
    BitwiseXor_64,
    ShiftLeft_8,
    ShiftLeft_32,
    ShiftLeft_64,
    ShiftRight_8,
    ShiftRight_32,
    ShiftRight_64,

    Neg_8,
    Neg_32,
    Neg_64,
    Add_8,
    Add_32,
    Add_64,
    Sub_8,
    Sub_32,
    Sub_64,
    Mul_8,
    Mul_32,
    Mul_64,
    Div_8,
    Div_32,
    Div_64,
    Mod_8,
    Mod_32,
    Mod_64,
    Exp_8,
    Exp_32,
    Exp_64,

    // Control flow
    Invoke,                 // pop from stack, invoke the value
    Execute,                // pop function object from stack, execute it

    // --------------------------------------------------------------
    // The following have one 1-byte arg

    LoadStatic,             // arg => idx of static in module, push on stack
    LoadModule,             // arg => idx of imported module, push it as value on stack
    LoadFunction,           // arg => idx of function in module, push on stack

    Call0,                  // arg = idx of function in current module, call it, pop args from stack, push result back
    Call1,                  // arg = idx of function in builtin module, call it, pop args from stack, push result back

    MakeClosure,            // arg = idx of function in current module, pull nonlocals from stack, wrap them into closure, push closure back

    IncRef,                 // arg = offset from top, (uint32*) at the offset is dereferenced and incremented
    DecRef,                 // arg = offset from top, (uint32*) at the offset is dereferenced and decremented

    Jump,                   // arg => relative jump (+N instructions) - unconditional
    JumpIfNot,              // pop cond from stack, arg => relative jump (+N instructions) if cond is false

    // --------------------------------------------------------------
    // The following have two 1-byte args

    Call,                   // arg1 = idx of imported module, arg2 = idx of function in the module, call it, pop args from stack, push result back

    CopyVariable,           // arg1 => offset from base (0 = base - size = first local), arg2 => size, copy bytes from stack and push back on top
    CopyArgument,           // arg1 => offset from base (0 = base = first arg), arg2 => size, copy bytes from stack and push back on top

    Drop,                   // drop <arg2> bytes from stack, skipping top <arg1> bytes

    // --------------------------------------------------------------
    // Auxiliary aliases

    ZeroArgFirst = Noop,
    ZeroArgLast = Execute,
    OneArgFirst = LoadStatic,
    OneArgLast = JumpIfNot,
    TwoArgFirst = Call,
    TwoArgLast = Drop,
};

// Allow basic arithmetic on OpCode
inline Opcode operator+(Opcode lhs, int rhs) { return Opcode(int(lhs) + rhs); }

std::ostream& operator<<(std::ostream& os, Opcode v);


class Code {
public:
    using OpIdx = size_t;
    void add_opcode(Opcode opcode, uint8_t arg) { add_opcode(opcode); add_arg(arg); }
    void add_opcode(Opcode opcode, uint8_t arg1, uint8_t arg2) { add_opcode(opcode); add_arg(arg1); add_arg(arg2); }
    void add_opcode(Opcode opcode) { m_ops.push_back(static_cast<uint8_t>(opcode)); }
    void add_arg(uint8_t arg) { m_ops.push_back(arg); }
    void set_arg(OpIdx pos, uint8_t arg) { m_ops[pos] = arg; }
    OpIdx this_instruction_address() const { return m_ops.size() - 1; }
    OpIdx next_instruction_address() const { return m_ops.size(); }

    using const_iterator = std::vector<uint8_t>::const_iterator;
    const_iterator begin() const { return m_ops.begin(); }
    const_iterator end() const { return m_ops.end(); }

    bool operator==(const Code& rhs) const { return m_ops == rhs.m_ops; }

private:
    std::vector<uint8_t> m_ops;
};



} // namespace xci::script

#endif // include guard
