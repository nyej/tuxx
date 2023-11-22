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

#include "testing.hpp"

#include <tuxx.hpp>

using namespace std;
using namespace nyej::tuxx;

// We're not #define'ing TUXX_DEFINE_TEST_MAIN here so we need to add an
// implementation for this
std::vector<test_case_instance>& nyej::tuxx::detail::tests__() {
    static std::vector<test_case_instance> tests;
    return tests;
}

test_case(first_test_case) {}
test_case(second_test_case) {}

struct my_fixture {};

test_case_f(my_fixture, third_test_case_f) {}
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
test_case_args_arr(fifth_test_case_arr, args) {}
test_case_args_arr_f(my_fixture, sixth_test_case_arr_f, args) {}

test_case_args(seventh_test_case_args, my_arg, my_arg{"x", 10}, my_arg{"y", 11}) {}
test_case_args_f(my_fixture, eighth_test_case_args_f, my_arg, my_arg{"x", 10}, my_arg{"y", 11}) {}

template <typename Ch>
int testing_main(
    int argc,
    Ch* argv[]
) {
    char const file_name_check[] = "test_add_cases";

    auto const& tests = detail::tests__();

    testing_assert(tests.size() == 12);

    testing_assert(tests[0].name == TESTING_STR_LIT("first_test_case"));
    testing_assert(tests[0].arg.empty());
    testing_assert(!!strstr(tests[0].file, file_name_check));
    testing_assert(tests[0].line == 39);  // Brittle but I want to test this

    testing_assert(tests[1].name == TESTING_STR_LIT("second_test_case"));
    testing_assert(tests[1].arg.empty());
    testing_assert(!!strstr(tests[1].file, file_name_check));
    testing_assert(tests[1].line == 40);

    testing_assert(tests[2].name == TESTING_STR_LIT("my_fixture.third_test_case_f"));
    testing_assert(tests[2].arg.empty());
    testing_assert(!!strstr(tests[2].file, file_name_check));
    testing_assert(tests[2].line == 44);

    testing_assert(tests[3].name == TESTING_STR_LIT("my_fixture.fourth_test_case_f"));
    testing_assert(tests[3].arg.empty());
    testing_assert(!!strstr(tests[3].file, file_name_check));
    testing_assert(tests[3].line == 45);

    testing_assert(tests[4].name == TESTING_STR_LIT("fifth_test_case_arr"));
    testing_assert(tests[4].arg == TESTING_STR_LIT("my_arg{s: 'a', i: 1}"));
    testing_assert(!!strstr(tests[4].file, file_name_check));
    testing_assert(tests[4].line == 67);

    testing_assert(tests[5].name == TESTING_STR_LIT("fifth_test_case_arr"));
    testing_assert(tests[5].arg == TESTING_STR_LIT("my_arg{s: 'b', i: 2}"));
    testing_assert(!!strstr(tests[5].file, file_name_check));
    testing_assert(tests[5].line == 67);

    testing_assert(tests[6].name == TESTING_STR_LIT("my_fixture.sixth_test_case_arr_f"));
    testing_assert(tests[6].arg == TESTING_STR_LIT("my_arg{s: 'a', i: 1}"));
    testing_assert(!!strstr(tests[6].file, file_name_check));
    testing_assert(tests[6].line == 68);

    testing_assert(tests[7].name == TESTING_STR_LIT("my_fixture.sixth_test_case_arr_f"));
    testing_assert(tests[7].arg == TESTING_STR_LIT("my_arg{s: 'b', i: 2}"));
    testing_assert(!!strstr(tests[7].file, file_name_check));
    testing_assert(tests[7].line == 68);

    testing_assert(tests[8].name == TESTING_STR_LIT("seventh_test_case_args"));
    testing_assert(tests[8].arg == TESTING_STR_LIT("my_arg{s: 'x', i: 10}"));
    testing_assert(!!strstr(tests[8].file, file_name_check));
    testing_assert(tests[8].line == 70);

    testing_assert(tests[9].name == TESTING_STR_LIT("seventh_test_case_args"));
    testing_assert(tests[9].arg == TESTING_STR_LIT("my_arg{s: 'y', i: 11}"));
    testing_assert(!!strstr(tests[9].file, file_name_check));
    testing_assert(tests[9].line == 70);

    testing_assert(tests[10].name == TESTING_STR_LIT("my_fixture.eighth_test_case_args_f"));
    testing_assert(tests[10].arg == TESTING_STR_LIT("my_arg{s: 'x', i: 10}"));
    testing_assert(!!strstr(tests[10].file, file_name_check));
    testing_assert(tests[10].line == 71);

    testing_assert(tests[11].name == TESTING_STR_LIT("my_fixture.eighth_test_case_args_f"));
    testing_assert(tests[11].arg == TESTING_STR_LIT("my_arg{s: 'y', i: 11}"));
    testing_assert(!!strstr(tests[11].file, file_name_check));
    testing_assert(tests[11].line == 71);

    return 0;
}
