// test_argparser.cpp created on 2020-02-02 as part of xcikit project
// https://github.com/rbrich/xcikit
//
// Copyright 2020 Radek Brich
// Licensed under the Apache License, Version 2.0 (see LICENSE file)

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <xci/core/ArgParser.h>

using namespace xci::core;
using namespace xci::core::argparser;


TEST_CASE( "Bool value conversion", "[ArgParser][value_from_cstr]" )
{
    bool v = false;
    {
        INFO("bool supported input");
        CHECK(value_from_cstr("true", v)); CHECK(v);
        CHECK(value_from_cstr("false", v)); CHECK(!v);
        CHECK(value_from_cstr("yes", v)); CHECK(v);
        CHECK(value_from_cstr("no", v)); CHECK(!v);
        CHECK(value_from_cstr("1", v)); CHECK(v);
        CHECK(value_from_cstr("0", v)); CHECK(!v);
        CHECK(value_from_cstr("y", v)); CHECK(v);
        CHECK(value_from_cstr("n", v)); CHECK(!v);
        CHECK(value_from_cstr("T", v)); CHECK(v);
        CHECK(value_from_cstr("F", v)); CHECK(!v);
    }
    {
        INFO("bool unsupported input");
        CHECK(!value_from_cstr("abc", v));
        CHECK(!value_from_cstr("yesyes", v));
        CHECK(!value_from_cstr("nn", v));
        CHECK(!value_from_cstr("X", v));
        CHECK(!value_from_cstr("ON", v));
        CHECK(!value_from_cstr("off", v));
    }
}


TEST_CASE( "Int value conversion", "[ArgParser][value_from_cstr]" )
{
    int v = 0;
    uint8_t u8 = 0;
    {
        INFO("int supported input");
        CHECK(value_from_cstr("1", v)); CHECK(v == 1);
        CHECK(value_from_cstr("0", v)); CHECK(v == 0);
        CHECK(value_from_cstr("-1", v)); CHECK(v == -1);
        CHECK(value_from_cstr("123456", v)); CHECK(v == 123456);
        CHECK(value_from_cstr("0xff", v)); CHECK(v == 0xff);
    }
    {
        INFO("int unsupported input");
        CHECK(!value_from_cstr("abc", v));
        CHECK(!value_from_cstr("1e3", v));
        CHECK(!value_from_cstr("11111111111111111111111111111111111", v));
    }
    {
        INFO("uint8 supported input");
        CHECK(value_from_cstr("0", u8)); CHECK(u8 == 0);
        CHECK(value_from_cstr("255", u8)); CHECK(u8 == 255);
        CHECK(value_from_cstr("077", u8)); CHECK(u8 == 077);
        CHECK(value_from_cstr("0xff", u8)); CHECK(u8 == 0xff);
    }
    {
        INFO("uint8 out of range");
        CHECK(!value_from_cstr("-1", u8));
        CHECK(!value_from_cstr("256", u8));
    }
}


TEST_CASE( "Other value conversion", "[ArgParser][value_from_cstr]" )
{
    {
        const char* v;
        const char* sample = "test";
        INFO("const char* -> original address returned unchanged");
        CHECK(value_from_cstr(sample, v)); CHECK(v == sample);
    }
}


TEST_CASE( "Option description parsing", "[ArgParser][Option]" )
{
    bool flag;
    {
        Option o("-h, --help", "Show help", show_help);
        INFO("basic");
        CHECK(o.has_short('h'));
        CHECK(o.has_long("help"));
        CHECK(!o.has_args());
        CHECK(!o.is_positional());
        CHECK(o.is_print_help());
        CHECK(!o.can_receive_arg());
    }
    {
        Option o("-h", "1 short", flag);
        INFO("1 short");
        CHECK(o.has_short('h'));
        CHECK(!o.has_long("h"));
        CHECK(!o.has_args());
        CHECK(!o.is_positional());
    }
    {
        Option o("-a -b", "2 shorts", flag);
        INFO("2 shorts");
        CHECK(o.has_short('a'));
        CHECK(o.has_short('b'));
        CHECK(!o.has_short('c'));
        CHECK(!o.has_args());
        CHECK(!o.is_positional());
    }
    {
        Option o("--help", "1 long", flag);
        INFO("1 long");
        CHECK(o.has_long("help"));
        CHECK(!o.has_args());
        CHECK(!o.is_positional());
    }
    {
        Option o("--a --b", "2 longs", flag);
        INFO("2 longs");
        CHECK(o.has_long("a"));
        CHECK(o.has_long("b"));
        CHECK(!o.has_args());
        CHECK(!o.is_positional());
    }
    {
        Option o("file", "positional", flag);
        INFO("positional");
        CHECK(!o.is_short());
        CHECK(!o.is_long());
        CHECK(o.is_positional());
        CHECK(o.has_args());
        CHECK(o.required_args() == 1);
    }
    {
        Option o("input...", "all positional", flag);
        INFO("all positional");
        CHECK(!o.is_short());
        CHECK(!o.is_long());
        CHECK(o.is_positional());
        CHECK(o.has_args());
        CHECK(o.required_args() == 1);
        CHECK(o.can_receive_all_args());
    }
    {
        // passthrough the rest of args
        Option o("-- ...", "stop parsing", flag);
        INFO("stop parsing");
        CHECK(!o.is_short());
        CHECK(!o.is_long());
        CHECK(!o.is_positional());
        CHECK(o.is_stop());
        CHECK(o.can_receive_all_args());
    }
}


TEST_CASE( "Invalid option descriptions", "[ArgParser][Option]" )
{
    CHECK_THROWS_AS(Option("---help", "Too many dashes", show_help), BadOptionDescription);
    CHECK_THROWS_AS(Option("-help", "Too long short option", show_help), BadOptionDescription);
    CHECK_THROWS_AS(Option("-", "Missing short name", show_help), BadOptionDescription);
    CHECK_THROWS_AS(Option("file.", "Not enough dots", show_help), BadOptionDescription);
    CHECK_THROWS_AS(Option("file..", "Not enough dots", show_help), BadOptionDescription);
    CHECK_THROWS_AS(Option("file....", "To many dots", show_help), BadOptionDescription);
    CHECK_THROWS_AS(Option("FILE -f", "Swapped nonsense", show_help), BadOptionDescription);
}


TEST_CASE( "Parse arg", "[ArgParser][parse_arg]" )
{
    {
        bool verbose = false;
        bool warn = false;
        int optimize = 0;
        std::vector<const char*> files;
        ArgParser ap {
            Option("-v, --verbose", "Enable verbosity", verbose),
            Option("-w, --warn", "Warn me", warn),
            Option("-O, --optimize LEVEL", "Optimization level", optimize),
        };

        CHECK_THROWS_AS(ap.parse_arg("-x"), BadArgument);
        CHECK_THROWS_AS(ap.parse_arg("---v"), BadArgument);
        CHECK_THROWS_AS(ap.parse_arg("--v"), BadArgument);
        CHECK_THROWS_AS(ap.parse_arg("-verbose"), BadArgument);
        CHECK_THROWS_AS(ap.parse_arg("-vx"), BadArgument);
        CHECK_THROWS_AS(ap.parse_arg("file"), BadArgument);
    }

    {
        bool verbose = false;
        bool warn = false;
        int optimize = 0;
        std::vector<const char*> files;
        ArgParser ap {
                Option("-v, --verbose", "Enable verbosity", verbose),
                Option("-w, --warn", "Warn me", warn),
                Option("-O, --optimize LEVEL", "Optimization level", optimize),
                Option("FILE...", "Input files", files),
        };
        ap.parse_arg("-vwO3");
        ap.parse_arg("file1");
        ap.parse_arg("file2");
        CHECK(verbose);
        CHECK(warn);
        CHECK(optimize == 3);
        CHECK(files == std::vector<const char*>{"file1", "file2"});

        INFO("option given again");
        CHECK_THROWS_AS(ap.parse_arg("--optimize").parse_arg("4"), BadArgument);
    }
}


TEST_CASE( "Option argument", "[ArgParser][parse_arg]" )
{
    char t;
    ArgParser ap {
        Option("-t VALUE", "Test choices: a,b", [&t](const char* arg){
            t = arg[0];
            return (t == 'a' || t == 'b') && arg[1] == 0;
        }),
    };

    SECTION("a") {
        ap.parse_arg("-ta");
        CHECK(t == 'a');
    }

    SECTION("b") {
        ap.parse_arg("-t").parse_arg("b");
        CHECK(t == 'b');
    }

    SECTION("c") {
        CHECK_THROWS_AS(ap.parse_arg("-tc"), BadArgument);
    }
}
