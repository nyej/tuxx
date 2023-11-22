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

#include <cstdlib>

#define TUXX_DEFINE_TEST_MAIN

#include <tuxx.hpp>

// Less than useful tests that demonstrate basic usage of tuxx.
// Set an environment variable, BASIC_TEST_ADD_FAILURES before
// running this to induce some errors.

test_case(simple_test_case) {
    assert_eq(10, 5 + 5);
    for (auto i = 0u; i < 10; ++i) {
        assert_le_msg(i, 10, "iteration: " << i);
    }
}

test_case(another_test_case) {
    assert_null(nullptr);
    if (getenv("BASIC_TEST_ADD_FAILURES")) {
        assert_false(true);
    }
}

static int args[] = {0, 1, 2, 3};
test_case_args_arr(args_test_case, args) {
    assert_le(arg, 10);

    if (getenv("BASIC_TEST_ADD_FAILURES")) {
        assert_true(false);
    }
}

test_case_args(args_test_case_2, int, 0, 1, 2, 3) {
    assert_le(arg, 10);
}

// Can be treated as a "suite" as well...
struct my_fixture {
    std::string name;
    my_fixture() : name{"fixture"} { /* Setup */ };
    ~my_fixture() { /* Teardown */ };
};

test_case_f(my_fixture, fixture_test_case_1) {
    assert_ieq(fixture.name, "fixture");
}

test_case_f(my_fixture, fixture_test_case_2) {
    assert_eq(fixture.name.length(), strlen("fixture"));
}

test_case_args_arr_f(my_fixture, fixture_and_args_test_case_1, args) {
    assert_le(arg, static_cast<int>(fixture.name.length()));
}

test_case_args_f(my_fixture, fixture_and_args_test_case_2, int, 0, 1, 2, 3) {
    assert_le(arg, static_cast<int>(fixture.name.length()));
}
