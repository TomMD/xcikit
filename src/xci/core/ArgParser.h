// ArgParser.h created on 2019-06-04 as part of xcikit project
// https://github.com/rbrich/xcikit
//
// Copyright 2019, 2020 Radek Brich
// Licensed under the Apache License, Version 2.0 (see LICENSE file)

#ifndef XCI_CORE_ARG_PARSER_H
#define XCI_CORE_ARG_PARSER_H

#include <xci/core/error.h>
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <sstream>
#include <initializer_list>
#include <iostream>
#include <strings.h>
#include <cstdlib>
#include <limits>

namespace xci::core::argparser {


class BadOptionDescription: public xci::core::Error {
public:
    explicit BadOptionDescription(const char* detail, const char* desc)
            : Error(std::string(detail) + ": " + desc) {}
};

class BadArgument: public xci::core::Error {
public:
    explicit BadArgument(std::string detail)
            : Error(std::move(detail)) {}
};


// integers
template <class T>
typename std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, bool>
value_from_cstr(const char* s, T& value)
{
    char* end;
    errno = 0;
    long l = strtol(s, &end, 0);
    if (*end != '\0')
        return false;  // not fully parser
    if (errno == ERANGE || l > std::numeric_limits<T>::max() || l < std::numeric_limits<T>::min())
        return false;  // out of range
    value = static_cast<T>(l);
    return true;
}

// bool
template <class T>
typename std::enable_if_t<std::is_same_v<T, bool>, bool>
value_from_cstr(const char* s, T& value) {
    if ((*s == '0' || tolower(*s) == 'f' || tolower(*s) == 'n') && s[1] == '\0')
        value = false;
    else if ((*s == '1' || tolower(*s) == 't' || tolower(*s) == 'y') && s[1] == '\0')
        value = true;
    else if (!strcasecmp(s, "false") || !strcasecmp(s, "no"))
        value = false;
    else if (!strcasecmp(s, "true") || !strcasecmp(s, "yes"))
        value = true;
    else
        return false;
    return true;
}

// const char* (original, unparsed C string)
template <class T>
typename std::enable_if_t<std::is_same_v<T, const char*>, bool>
value_from_cstr(const char* s, T& value) {
    value = s;
    return true;
}


// vector of unparsed args
template <class T>
typename std::enable_if_t<std::is_same_v<T, std::vector<const char*>>, bool>
value_from_cstr(const char* s, T& value) {
    value.push_back(s);
    return true;
}


struct DefaultHelp {};
constexpr DefaultHelp show_help{};

struct Option {
    using Setter = std::function<bool(const char* arg)>;

    /// \param desc     comma-separated short and long names,
    ///                 e.g. "-v, --verbose"
    Option(const char* desc, const char* help, Setter setter, int flags);

    Option(const char* desc, const char* help, DefaultHelp)
            : Option(desc, help, nullptr, FPrintHelp) {}

    Option(const char* desc, const char* help, Setter setter)
            : Option(desc, help, std::move(setter), 0) {}

    Option(const char* desc, const char* help, std::function<void()> flag_setter)
            : Option(desc, help, [&](const char* arg){ flag_setter(); return true; }, 0) {}

    template <class T>
    Option(const char* desc, const char* help, T& value)
            : Option(desc, help, [&value](const char* arg){ return value_from_cstr<T>(arg, value); }, 0) {}

    bool has_short(char arg) const;
    bool has_long(const char* arg) const;
    bool has_args() const { return m_args != 0; }
    bool is_short() const { return m_flags & FShort; }
    bool is_long() const { return m_flags & FLong; }
    bool is_positional() const { return m_flags & FPositional; }
    bool is_print_help() const { return m_flags & FPrintHelp; }
    bool is_stop() const { return m_flags & FStop; }
    bool can_receive_all_args() const { return (m_flags & FDots); }
    bool can_receive_arg() const { return can_receive_all_args() || m_received < m_args; }
    int required_args() const { return m_args; }
    const char* desc() const { return m_desc; }
    const char* help() const { return m_help; }
    std::string usage() const;
    std::string formatted_desc(size_t width) const;

    bool operator() (const char* arg) { ++m_received; return m_setter(arg); }

private:
    struct NamePos {
        // e.g. ", --example 123"
        // - pos = 2 (skip ", "), dashes = 2 ("--"), len = 7 ("example")
        // - end points to " 123"
        int pos;
        int dashes;
        int len;
        int end() const { return pos + dashes + len; }
        operator bool() const { return dashes != 0 || len != 0; }
    };
    NamePos parse_desc(const char* desc) const;

    // 0 = positional, 1 = short, 2 = long
    std::pair<const char*, int> skip_dashes(const char* desc) const;

private:
    const char* m_desc;
    const char* m_help;
    Setter m_setter;
    enum Flags {
        FShort      = (1 << 0),
        FLong       = (1 << 1),
        FPositional = (1 << 2),
        FPrintHelp  = (1 << 3),
        FDots       = (1 << 4),
        FStop       = (1 << 5),
    };
    int m_flags = 0;
    int m_args = 0;
    int m_received = 0;
};


// Declarative parser for command-line arguments.

class ArgParser {
public:
    explicit ArgParser(std::initializer_list<Option> options);

    /// Parse command-line arguments
    ArgParser& parse_args(int argc, const char* argv[]);
    ArgParser& parse_args(int argc, char* argv[]) { return parse_args(argc, (const char**) argv); }

    // ------------------------------------------------------------------------
    // Semi-private methods
    // (normally not needed in client code)

    /// Add option to the parser
    ArgParser& add_option(Option&& opt);

    /// Print short usage information
    void print_usage() const;

    /// Print help text
    void print_help() const;

    /// Validate correctness of entered options (e.g. doubled flags)
    void validate() const;

    /// TODO: Parse environment variables
    /// This is called internally before `parse_args` (env has lower priority)
    void parse_env();

    /// Parse a single argument
    ArgParser& parse_arg(const char* arg);

private:
    std::string m_progname;
    std::vector<Option> m_opts;
    Option* m_curopt = nullptr;
};


} // namespace xci::core::argparser

namespace xci::core { using argparser::ArgParser; }

#endif // include guard
