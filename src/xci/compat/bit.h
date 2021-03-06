// bit.h created on 2018-11-11 as part of xcikit project
// https://github.com/rbrich/xcikit
//
// Copyright 2018, 2020 Radek Brich
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
//
// ------------------------------------------------------------------
// Bitwise operations - <bit> header compatibility + goodies
//
// References:
// - https://en.cppreference.com/w/cpp/header/bit
// - http://graphics.stanford.edu/~seander/bithacks.html
// ------------------------------------------------------------------

#ifndef XCI_COMPAT_BIT_H
#define XCI_COMPAT_BIT_H

#include <type_traits>
#include <cstring>
#include <cstdint>

namespace xci {


/// C++20 bit_cast emulation.
///
/// Example:
///
///     double f = 3.14;
///     auto u = std::bit_cast<std::uint64_t>(f);

template <class To, class From>
typename std::enable_if<
        (sizeof(To) == sizeof(From)) &&
        std::is_trivially_copyable<From>::value &&
        std::is_trivial<To>::value,
        To>::type
bit_cast(const From &src) noexcept
{
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}


/// Similar to C++20 bit_cast, but read bits from void*, char*, byte* etc.
/// Does not check type sizes. Useful to emulate file reading from memory buffer.
///
/// Example:
///
///     vector<byte> buf;
///     auto ptr = buf.data();
///     auto a = bit_read<int32_t>(ptr);
///     auto b = bit_read<uint16_t>(ptr + 4);

template <class To, class From>
typename std::enable_if<
        std::is_trivially_copyable<From>::value &&
        std::is_trivial<To>::value &&
        !std::is_pointer<From>::value &&
        !std::is_pointer<To>::value &&
        sizeof(From) == 1,
        To>::type
bit_read(const From* src) noexcept
{
    To dst;
    std::memcpy(&dst, src, sizeof(To));
    return dst;
}


// C++20 bit operations
//
// Reference:
// - https://en.cppreference.com/w/cpp/header/bit
// - https://en.wikipedia.org/wiki/Find_first_set

/// Count zero bits from left (MSB)
/// (similar to C++20 std::countl_zero)
constexpr int count_leading_zeros(uint64_t x) noexcept {
    return x == 0 ? 64 : __builtin_clzll(x);
}

/// Count zero bits from right (LSB)
/// (similar to C++20 std::countr_zero)
constexpr int count_trailing_zeros(uint64_t x) noexcept {
    return x == 0 ? 64 : __builtin_ctzll(x);
}


} // namespace

#endif // include guard
