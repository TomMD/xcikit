// Machine.cpp created on 2019-05-18, part of XCI toolkit
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

#include "Machine.h"
#include "Builtin.h"
#include "Value.h"
#include "Error.h"

#include <fmt/core.h>
#include <range/v3/view/reverse.hpp>
#include <cassert>
#include <functional>

namespace xci::script {

using ranges::cpp20::views::reverse;
using std::move;
using fmt::format;


void Machine::call(const Function& function, const InvokeCallback& cb)
{
    const Function* cur_fun = &function;
    auto it = function.code().begin();
    auto base = m_stack.size();
    auto call_fun = [this, &cur_fun, &it, &base](const Function& fn) {
        if (fn.is_native()) {
            fn.call_native(m_stack);
            return;
        }
        m_stack.push_frame(cur_fun, it - cur_fun->code().begin());
        cur_fun = &fn;
        it = cur_fun->code().begin();
        base = m_stack.frame().base;
        if (m_call_enter_cb)
            m_call_enter_cb(*cur_fun);
    };

    // Run function code
    m_stack.push_frame(nullptr, 0);
    if (m_call_enter_cb)
        m_call_enter_cb(*cur_fun);
    for (;;) {
        if (it == cur_fun->code().end()) {
            // return from function
            if (m_bytecode_trace_cb)
                m_bytecode_trace_cb(*cur_fun, cur_fun->code().end());

            if (m_call_exit_cb)
                m_call_exit_cb(*cur_fun);

            // no more stack frames?
            if (m_stack.frame().function == nullptr) {
                m_stack.pop_frame();
                assert(m_stack.size() == function.effective_return_type().size());
                break;
            }

            // return into previous call location
            cur_fun = m_stack.frame().function;
            it = cur_fun->code().begin() + m_stack.frame().instruction;
            m_stack.pop_frame();
            base = m_stack.frame().base;
            continue;
        }

        if (m_bytecode_trace_cb)
            m_bytecode_trace_cb(*cur_fun, it);


        auto opcode = static_cast<Opcode>(*it++);
        switch (opcode) {

            case Opcode::LogicalOr:
            case Opcode::LogicalAnd: {
                auto fn = builtin::logical_op_function(opcode);
                if (!fn)
                    throw NotImplemented(format("logical operator {}", opcode));
                auto lhs = m_stack.pull<value::Bool>();
                auto rhs = m_stack.pull<value::Bool>();
                m_stack.push(fn(lhs, rhs));
                break;
            }

            case Opcode::Equal_8:
            case Opcode::NotEqual_8:
            case Opcode::LessEqual_8:
            case Opcode::GreaterEqual_8:
            case Opcode::LessThan_8:
            case Opcode::GreaterThan_8: {
                auto fn = builtin::comparison_op_function<value::Byte>(opcode);
                if (!fn)
                    throw NotImplemented(format("comparison operator {}", opcode));
                auto lhs = m_stack.pull<value::Byte>();
                auto rhs = m_stack.pull<value::Byte>();
                m_stack.push(fn(lhs, rhs));
                break;
            }

            case Opcode::Equal_32:
            case Opcode::NotEqual_32:
            case Opcode::LessEqual_32:
            case Opcode::GreaterEqual_32:
            case Opcode::LessThan_32:
            case Opcode::GreaterThan_32: {
                auto fn = builtin::comparison_op_function<value::Int32>(opcode);
                if (!fn)
                    throw NotImplemented(format("comparison operator {}", opcode));
                auto lhs = m_stack.pull<value::Int32>();
                auto rhs = m_stack.pull<value::Int32>();
                m_stack.push(fn(lhs, rhs));
                break;
            }

            case Opcode::Equal_64:
            case Opcode::NotEqual_64:
            case Opcode::LessEqual_64:
            case Opcode::GreaterEqual_64:
            case Opcode::LessThan_64:
            case Opcode::GreaterThan_64: {
                auto fn = builtin::comparison_op_function<value::Int64>(opcode);
                if (!fn)
                    throw NotImplemented(format("comparison operator {}", opcode));
                auto lhs = m_stack.pull<value::Int64>();
                auto rhs = m_stack.pull<value::Int64>();
                m_stack.push(fn(lhs, rhs));
                break;
            }

            case Opcode::BitwiseOr_8:
            case Opcode::BitwiseAnd_8:
            case Opcode::BitwiseXor_8:
            case Opcode::ShiftLeft_8:
            case Opcode::ShiftRight_8:
            case Opcode::Add_8:
            case Opcode::Sub_8:
            case Opcode::Mul_8:
            case Opcode::Div_8:
            case Opcode::Mod_8:
            case Opcode::Exp_8: {
                auto fn = builtin::binary_op_function<value::Byte>(opcode);
                if (!fn)
                    throw NotImplemented(format("binary operator {}", opcode));
                auto lhs = m_stack.pull<value::Byte>();
                auto rhs = m_stack.pull<value::Byte>();
                m_stack.push(fn(lhs, rhs));
                break;
            }

            case Opcode::BitwiseOr_32:
            case Opcode::BitwiseAnd_32:
            case Opcode::BitwiseXor_32:
            case Opcode::ShiftLeft_32:
            case Opcode::ShiftRight_32:
            case Opcode::Add_32:
            case Opcode::Sub_32:
            case Opcode::Mul_32:
            case Opcode::Div_32:
            case Opcode::Mod_32:
            case Opcode::Exp_32: {
                auto fn = builtin::binary_op_function<value::Int32>(opcode);
                if (!fn)
                    throw NotImplemented(format("binary operator {}", opcode));
                auto lhs = m_stack.pull<value::Int32>();
                auto rhs = m_stack.pull<value::Int32>();
                m_stack.push(fn(lhs, rhs));
                break;
            }

            case Opcode::BitwiseOr_64:
            case Opcode::BitwiseAnd_64:
            case Opcode::BitwiseXor_64:
            case Opcode::ShiftLeft_64:
            case Opcode::ShiftRight_64:
            case Opcode::Add_64:
            case Opcode::Sub_64:
            case Opcode::Mul_64:
            case Opcode::Div_64:
            case Opcode::Mod_64:
            case Opcode::Exp_64: {
                auto fn = builtin::binary_op_function<value::Int64>(opcode);
                if (!fn)
                    throw NotImplemented(format("binary operator {}", opcode));
                auto lhs = m_stack.pull<value::Int64>();
                auto rhs = m_stack.pull<value::Int64>();
                m_stack.push(fn(lhs, rhs));
                break;
            }

            case Opcode::LogicalNot: {
                auto fn = builtin::logical_not_function();
                assert(fn);
                auto rhs = m_stack.pull<value::Bool>();
                m_stack.push(fn(rhs));
                break;
            }

            case Opcode::BitwiseNot_8:
            case Opcode::Neg_8: {
                auto fn = builtin::unary_op_function<value::Byte>(opcode);
                if (!fn)
                    throw NotImplemented(format("unary operator {}", opcode));
                auto rhs = m_stack.pull<value::Byte>();
                m_stack.push(fn(rhs));
                break;
            }

            case Opcode::BitwiseNot_32:
            case Opcode::Neg_32: {
                auto fn = builtin::unary_op_function<value::Int32>(opcode);
                if (!fn)
                    throw NotImplemented(format("unary operator {}", opcode));
                auto rhs = m_stack.pull<value::Int32>();
                m_stack.push(fn(rhs));
                break;
            }

            case Opcode::BitwiseNot_64:
            case Opcode::Neg_64: {
                auto fn = builtin::unary_op_function<value::Int64>(opcode);
                if (!fn)
                    throw NotImplemented(format("unary operator {}", opcode));
                auto rhs = m_stack.pull<value::Int64>();
                m_stack.push(fn(rhs));
                break;
            }

            case Opcode::Subscript_32: {
                auto lhs = m_stack.pull<value::Int32List>();
                auto rhs = m_stack.pull<value::Int32>();
                auto idx = rhs.value();
                auto len = lhs.length();
                lhs.decref();
                if (idx < 0)
                    idx += (int) len;
                if (idx < 0 || (size_t) idx >= len)
                    throw IndexOutOfBounds(idx, len);
                m_stack.push(*lhs.get(idx));
                break;
            }

            case Opcode::Invoke: {
                const auto type_index = *it++;
                const auto& type_info = cur_fun->module().get_type(type_index);
                cb(*m_stack.pull(type_info));
                break;
            }

            case Opcode::Execute: {
                auto o = m_stack.pull<value::Closure>();
                auto closure = o.closure();
                for (const auto& nl : reverse(closure.values())) {
                    m_stack.push(*nl);
                }
                call_fun(o.function());
                o.decref();
                break;
            }

            case Opcode::LoadStatic: {
                auto arg = *it++;
                const auto& o = cur_fun->module().get_value(arg);
                m_stack.push(o);
                o.incref();
                break;
            }

            case Opcode::LoadFunction: {
                auto arg = *it++;
                auto& fn = cur_fun->module().get_function(arg);
                m_stack.push(value::Closure(fn));
                break;
            }

            case Opcode::SetBase: {
                auto level = *it++;
                base = m_stack.frame(m_stack.n_frames() - 1 - level).base;
                break;
            }

            case Opcode::Copy: {
                const auto addr = *it++ + m_stack.to_rel(base); // arg1 + base
                const auto size = *it++; // arg2
                m_stack.copy(addr, size);
                break;
            }

            case Opcode::Drop: {
                const auto addr = *it++;
                const auto size = *it++;
                m_stack.drop(addr, size);
                break;
            }

            case Opcode::Call0:
            case Opcode::Call1:
            case Opcode::Call: {
                // get the function's module
                Module* module;
                if (opcode == Opcode::Call0) {
                    module = &cur_fun->module();
                } else {
                    size_t idx;
                    if (opcode == Opcode::Call1) {
                        idx = 0;
                    } else {
                        // read arg1
                        idx = *it++;
                    }
                    module = &cur_fun->module().get_imported_module(idx);
                }
                // call function from the module
                auto arg = *it++;
                auto& fn = module->get_function(arg);
                call_fun(fn);
                break;
            }

            case Opcode::MakeList: {
                const auto num_elems = *it++;
                const auto size_of_elem = *it++;
                const size_t total_size = num_elems * size_of_elem;
                // move list contents from stack to heap
                HeapSlot slot{total_size};
                std::memcpy(slot.data(), m_stack.data(), total_size);
                m_stack.drop(0, total_size);
                // push list handle back to stack
                m_stack.push(value::List{TypeInfo{Type::Int32}, num_elems, move(slot)});
                break;
            }

            case Opcode::MakeClosure: {
                auto arg1 = *it++;
                // get function
                auto& fn = cur_fun->module().get_function(arg1);
                // pull nonlocals + partial args
                Values closure;
                closure.reserve(fn.nonlocals().size() + fn.partial().size());
                for (const auto & ti : fn.nonlocals()) {
                    closure.add(m_stack.pull(ti));
                }
                for (const auto & ti : fn.partial()) {
                    closure.add(m_stack.pull(ti));
                }
                // push closure
                m_stack.push(value::Closure{fn, move(closure)});
                break;
            }

            case Opcode::IncRef: {
                auto arg = *it++;
                HeapSlot slot {static_cast<byte*>(m_stack.get_ptr(arg))};
                slot.incref();
                break;
            }

            case Opcode::DecRef: {
                auto arg = *it++;
                HeapSlot slot {static_cast<byte*>(m_stack.get_ptr(arg))};
                slot.decref();
                // needed for stack dump:
                if (slot.refcount() == 0)
                    m_stack.clear_ptr(arg);
                break;
            }

            case Opcode::Jump: {
                auto arg = *it++;
                it += arg;
                break;
            }

            case Opcode::JumpIfNot: {
                auto arg = *it++;
                auto cond = m_stack.pull<value::Bool>();
                if (!cond.value()) {
                    it += arg;
                }
                break;
            }

            default:
                throw NotImplemented(format("opcode {}", opcode));
        }
    }
}


} // namespace xci::script
