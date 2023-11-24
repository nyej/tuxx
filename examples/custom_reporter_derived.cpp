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

////////////////////////////////////////////////////////////////////////////////
//
// Example of creating a custom reporter by deriving a new class from
// nyej::tux::test_case_reporter.
//
////////////////////////////////////////////////////////////////////////////////


#include <iostream>

// -----------------------------------------------------------------------------
// Forward declarations for things we need before including tuxx...
// NOTE: this part and the implementation of our custom test reporter can be
//       in a separate file...

namespace nyej {
namespace tuxx {
    struct test_case_reporter;
    struct test_case_reporter_args;
}
}

static nyej::tuxx::test_case_reporter* make_my_test_case_reporter(
    nyej::tuxx::test_case_reporter_args const&
);

// -----------------------------------------------------------------------------
// Set required #defines and include tuxx...

#define TUXX_USER_TEST_CASE_REPORTER_INIT(args) make_my_test_case_reporter(args)
#define TUXX_DEFINE_TEST_MAIN

#include <tuxx.hpp>

using namespace std;
using namespace nyej::tuxx;

// -----------------------------------------------------------------------------
// Add some of our test cases here...

test_case(custom_reporter__tc_1) {
}
test_case(custom_reporter__tc_2) {
    if (getenv("CUSTOM_REPORTER_ADD_FAILURES")) {
        assert_false(true);
    }
}

// -----------------------------------------------------------------------------
// Definition of our test custom reporter and make_my_test_case_reporter 
// (again, can be in a separate file)

namespace {

struct my_test_case_reporter : test_case_reporter {
    report_ostream_type& os;
    explicit my_test_case_reporter(report_ostream_type& ostrm) : os{ostrm} {}
    void test_case_passed(
        test_case_instance const& tc,
        chrono::steady_clock::duration const& elapsed
    ) override {
        os << tc.name << " P\n";
    }
    void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        chrono::steady_clock::duration const& elapsed
    ) override {
        os << tc.name << " F\n";
    }
    void end_test_cases(
        chrono::steady_clock::duration const& elapsed,
        vector<test_case_instance> const& failures
    ) override {}
    void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) override {}
}; 

}

// -----------------------------------------------------------------------------
// Factory function definition (again, can be in a separate file)
static test_case_reporter* make_my_test_case_reporter(test_case_reporter_args const& args) {
    return new my_test_case_reporter{*args.p_report_ostream};
}
