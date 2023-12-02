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

#define TUXX_DEFINE_CUSTOM_REPORTER
#define TESTING_DEFINE_MAIN

#include "testing.hpp"

#include <chrono>
#include <iostream>
#include <mutex>

using namespace std;
using namespace ::nyej::tuxx;

void testing_main(vector<string_type> const&) {
    static char_type prog_name[] = TESTING_STR_LIT("prog-name");

    vector<test_case_instance> tests{
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-1"), 1, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-2"), 2, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-3"), 3, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-4"), 4, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("a-test-1"), 5, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("a-test-2"), 6, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("a-test-3"), 7, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("a-test-4"), 8, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("b-test-1"), 9, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("b-test-2"), 10, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("b-test-3"), 11, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("b-test-4"), 12, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("b-test-5"), 13, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("b-test-6"), 14, nullptr}
    };

    auto& os = TUXX_REPORT_OSTRM;

    test_main_args::combinations(
        [&](test_main_args const& arg) {
            auto const concurrency_req = (arg.parallel_long || arg.parallel_short)
                ? (arg.parallel_arg ? test_main_args::parallel_value : 1)
                : 1;

            {
                vector<string_type> prog_args{prog_name};
                test_main_ctx t_ctx{"default custom reporter", arg, 0, 1, 1, prog_args};
                t_ctx.tests = tests;
                os << t_ctx << endl;
                auto const res = t_ctx.run_main();
                auto const did_capture_output = arg.capture_long || arg.capture_short;
                t_ctx.common_reporter_asserts(
                    res,
                    concurrency_req,
                    arg.randomized_long || arg.randomized_short,
                    did_capture_output
                );
                testing_assert(t_ctx.make_custom_reporter_calls.size() == 1);
                auto const& call = t_ctx.make_custom_reporter_calls[0];
                testing_assert(call.capture_output == (arg.capture_short || arg.capture_long));
                testing_assert(call.concurrency == concurrency_req);
                testing_assert(call.n_tests == t_ctx.tests.size());
                if (did_capture_output) {
                    testing_assert(call.p_report_ostream != &t_ctx.out_os);
                } else {
                    testing_assert(call.p_report_ostream == &t_ctx.out_os);
                }
                testing_assert(call.p_test_cases == t_ctx.tests.data());
                testing_assert(call.use_emojis == (arg.emojis_short || arg.emojis_long));
                testing_assert(call.write_colorized == arg.use_color);
                testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                testing_assert(t_ctx.make_json_reporter_calls.empty());
                testing_assert(t_ctx.make_junit_reporter_calls.empty());
            }

            {
                char_type const* text_args[] = {TESTING_STR_LIT("-t"), TESTING_STR_LIT("--text")};
                for (auto const text_arg : text_args) {
                    vector<string_type> prog_args{prog_name, text_arg};
                    test_main_ctx t_ctx{"force default reporter", arg, 0, 1, 1, prog_args};
                    t_ctx.tests = tests;
                    os << t_ctx << endl;
                    auto const res = t_ctx.run_main();
                    auto const did_capture_output = arg.capture_long || arg.capture_short;
                    t_ctx.common_reporter_asserts(
                        res,
                        concurrency_req,
                        arg.randomized_long || arg.randomized_short,
                        did_capture_output
                    );
                    testing_assert(t_ctx.make_custom_reporter_calls.empty());
                    testing_assert(t_ctx.make_plain_text_reporter_calls.size() == 1);
                    auto const& call = t_ctx.make_plain_text_reporter_calls[0];
                    testing_assert(call.concurrency == concurrency_req);
                    testing_assert(call.p_tests == &t_ctx.tests);
                    if (did_capture_output) {
                        testing_assert(call.p_os != &t_ctx.out_os);
                    } else {
                        testing_assert(call.p_os == &t_ctx.out_os);
                    }
                    testing_assert(call.use_emojis == (arg.emojis_short || arg.emojis_long));
                    testing_assert(call.use_color == arg.use_color);
                    testing_assert(t_ctx.make_json_reporter_calls.empty());
                    testing_assert(t_ctx.make_junit_reporter_calls.empty());
                }
            }
        }
    );
}

// Needs to be defined but it is not actually called here.
test_case_reporter* tuxx_make_custom_reporter(test_case_reporter_args const& args) {
    return nullptr;
}
