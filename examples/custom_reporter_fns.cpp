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
// Example of creating a custom reporter without directly deriving from
// nyej::tuxx::test_case_reporter
//
////////////////////////////////////////////////////////////////////////////////


#include <iostream>

// -----------------------------------------------------------------------------
// Set required #defines and include tuxx...

#define TUXX_DEFINE_CUSTOM_REPORTER
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
// Factory function definition (again, can be in a separate file)
test_case_reporter* tuxx_make_custom_reporter(test_case_reporter_args const& args) {
    // Only handling test case pass/fail here.
    return test_case_reporter_fns_builder()
        .handle_test_case_passed(
            [&args](
                test_case_instance const& tc,
                chrono::steady_clock::duration const& elapsed
            ) {
                *args.p_report_ostream << tc.name << " P\n";
            }
        ).handle_test_case_failed(
            [&args](
                test_case_instance const& tc,
                test_case_failure_error const& err,
                chrono::steady_clock::duration const& elapsed
            ) {
                *args.p_report_ostream << tc.name << " F\n";
            }
        ).build();
}
