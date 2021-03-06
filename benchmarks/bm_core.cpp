// bm_core.cpp created on 2018-08-21, part of XCI toolkit
// Copyright 2018 Radek Brich
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

#include <benchmark/benchmark.h>
#include <xci/core/string.h>

using namespace xci::core;


static void bm_utf8_codepoint(benchmark::State& state) {
    std::string input = "人";
    for (auto _ : state)
        utf8_codepoint(input.data());
}
BENCHMARK(bm_utf8_codepoint);


BENCHMARK_MAIN();
