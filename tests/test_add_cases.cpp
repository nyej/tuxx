/*

Copyright (c) 2023 Jason Nye

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

// WARNING: this is a test driver for tuxx itself. It is NOT an example of
//          how to use tuxx.

#define TESTING_DEFINE_MAIN

#include "testing.hpp"

#include <tuxx.hpp>

using namespace std;
using namespace nyej::tuxx;

int const first_test_case_line = __LINE__;
test_case(first_test_case) {}
int const second_test_case_line = __LINE__;
test_case(second_test_case) {}

struct my_fixture {};

int const third_test_case_line = __LINE__;
test_case_f(my_fixture, third_test_case_f) {}

int const fourth_test_case_line = __LINE__;
test_case_f(my_fixture, fourth_test_case_f) {}

struct my_arg {
    string s;
    int i{};
    my_arg(
        string str,
        int val
    ) : s{std::move(str)}, i{val} {}
};


template <typename Ch> inline
basic_ostream<Ch>& operator<<(
    basic_ostream<Ch>& os,
    my_arg const& arg
) {
    // NOTE: a newline is added here to verify it gets replaced by a space.
    return os << "my_arg{s: '" << arg.s << "',\ni: " << arg.i << '}';
}

static my_arg args[] = {my_arg{"a", 1}, my_arg{"b", 2}};

int const fifth_test_case_line = __LINE__;
test_case_args_arr(fifth_test_case_arr, args) {}

int const sixth_test_case_line = __LINE__;
test_case_args_arr_f(my_fixture, sixth_test_case_arr_f, args) {}

int const seventh_test_case_line = __LINE__;
test_case_args(seventh_test_case_args, my_arg, my_arg{"x", 10}, my_arg{"y", 11}) {}

int const eighth_test_case_line = __LINE__;
test_case_args_f(my_fixture, eighth_test_case_args_f, my_arg, my_arg{"x", 10}, my_arg{"y", 11}) {}

void testing_main(vector<string_type> const&) {
    char const file_name_check[] = "test_add_cases";

    auto const& tests = detail::tests__();

    testing_assert(tests.size() == 12);

    testing_assert(tests[0].name == TESTING_STR_LIT("first_test_case"));
    testing_assert(tests[0].arg.empty());
    testing_assert(!!strstr(tests[0].file, file_name_check));
    testing_assert(tests[0].line == first_test_case_line + 1);

    testing_assert(tests[1].name == TESTING_STR_LIT("second_test_case"));
    testing_assert(tests[1].arg.empty());
    testing_assert(!!strstr(tests[1].file, file_name_check));
    testing_assert(tests[1].line == second_test_case_line + 1);

    testing_assert(tests[2].name == TESTING_STR_LIT("my_fixture.third_test_case_f"));
    testing_assert(tests[2].arg.empty());
    testing_assert(!!strstr(tests[2].file, file_name_check));
    testing_assert(tests[2].line == third_test_case_line + 1);

    testing_assert(tests[3].name == TESTING_STR_LIT("my_fixture.fourth_test_case_f"));
    testing_assert(tests[3].arg.empty());
    testing_assert(!!strstr(tests[3].file, file_name_check));
    testing_assert(tests[3].line == fourth_test_case_line + 1);

    testing_assert(tests[4].name == TESTING_STR_LIT("fifth_test_case_arr"));
    testing_assert(tests[4].arg == TESTING_STR_LIT("my_arg{s: 'a', i: 1}"));
    testing_assert(!!strstr(tests[4].file, file_name_check));
    testing_assert(tests[4].line == fifth_test_case_line + 1);

    testing_assert(tests[5].name == TESTING_STR_LIT("fifth_test_case_arr"));
    testing_assert(tests[5].arg == TESTING_STR_LIT("my_arg{s: 'b', i: 2}"));
    testing_assert(!!strstr(tests[5].file, file_name_check));
    testing_assert(tests[5].line == fifth_test_case_line + 1);

    testing_assert(tests[6].name == TESTING_STR_LIT("my_fixture.sixth_test_case_arr_f"));
    testing_assert(tests[6].arg == TESTING_STR_LIT("my_arg{s: 'a', i: 1}"));
    testing_assert(!!strstr(tests[6].file, file_name_check));
    testing_assert(tests[6].line == sixth_test_case_line + 1);

    testing_assert(tests[7].name == TESTING_STR_LIT("my_fixture.sixth_test_case_arr_f"));
    testing_assert(tests[7].arg == TESTING_STR_LIT("my_arg{s: 'b', i: 2}"));
    testing_assert(!!strstr(tests[7].file, file_name_check));
    testing_assert(tests[7].line == sixth_test_case_line + 1);

    testing_assert(tests[8].name == TESTING_STR_LIT("seventh_test_case_args"));
    testing_assert(tests[8].arg == TESTING_STR_LIT("my_arg{s: 'x', i: 10}"));
    testing_assert(!!strstr(tests[8].file, file_name_check));
    testing_assert(tests[8].line == seventh_test_case_line + 1);

    testing_assert(tests[9].name == TESTING_STR_LIT("seventh_test_case_args"));
    testing_assert(tests[9].arg == TESTING_STR_LIT("my_arg{s: 'y', i: 11}"));
    testing_assert(!!strstr(tests[9].file, file_name_check));
    testing_assert(tests[9].line == seventh_test_case_line + 1);

    testing_assert(tests[10].name == TESTING_STR_LIT("my_fixture.eighth_test_case_args_f"));
    testing_assert(tests[10].arg == TESTING_STR_LIT("my_arg{s: 'x', i: 10}"));
    testing_assert(!!strstr(tests[10].file, file_name_check));
    testing_assert(tests[10].line == eighth_test_case_line + 1);

    testing_assert(tests[11].name == TESTING_STR_LIT("my_fixture.eighth_test_case_args_f"));
    testing_assert(tests[11].arg == TESTING_STR_LIT("my_arg{s: 'y', i: 11}"));
    testing_assert(!!strstr(tests[11].file, file_name_check));
    testing_assert(tests[11].line == eighth_test_case_line + 1);
}
