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

using namespace std;
using namespace ::nyej::tuxx;

testing_mutex* testing_mutex::p_instance;

void test_main_args::combinations(function<void(test_main_args const&)> const& f) {
    for (auto idx_c = 0u; idx_c < 2; ++idx_c) {
        for (auto idx_es = 0u; idx_es < 2; ++idx_es) {
            for (auto idx_el = 0u; idx_el < 2; ++idx_el) {
                for (auto idx_rs = 0u; idx_rs < 2; ++idx_rs) {
                    for (auto idx_rl = 0u; idx_rl < 2; ++idx_rl) {
                        for (auto idx_cs = 0u; idx_cs < 2; ++idx_cs) {
                            for (auto idx_cl = 0u; idx_cl < 2; ++idx_cl) {
                                for (auto idx_ps = 0u; idx_ps < 2; ++idx_ps) {
                                    for (auto idx_pl = 0u; idx_pl < 2; ++idx_pl) {
                                        for (auto idx_pa = 0u; idx_pa < 2; ++idx_pa) {
                                            if (idx_pa && !idx_ps && !idx_pl) {
                                                // Not a sensical combination
                                                continue;
                                            }
                                            test_main_args const args{
                                                !!idx_c,
                                                !!idx_es,
                                                !!idx_el,
                                                !!idx_rs,
                                                !!idx_rl,
                                                !!idx_cs,
                                                !!idx_cl,
                                                !!idx_ps,
                                                !!idx_pl,
                                                !!idx_pa,
                                            };
                                            f(args);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

test_main_ctx::test_main_ctx(
    string dsc,
    vector<string_type> tst_args,
    int run_test_cases_res,
    bool is_atty_res,
    size_t hw_concur
) :
    desc{std::move(dsc)},
    args{std::move(tst_args)},
    run_test_cases_result{run_test_cases_res},
    is_atty_result{is_atty_res},
    hw_concurrency{hw_concur},
    p_rep{
        new basic_testing_test_case_reporter<chrono::steady_clock::duration, mutex>{}
    },
    p_rep_val{p_rep.get()}
{
    using namespace std;
    using namespace ::nyej::tuxx;
    deps.usage_fn = [this](
        string_type const& prog_name,
        report_ostream_type& os
    ) {
        usage_calls.push_back(usage_call{prog_name, &os});
    };
    deps.check_args_fn = [](vector<string_type> const&) { /* Nothing by default */};
    deps.list_tests_fn = [this](
        vector<test_case_instance> const& tsts,
        report_ostream_type& os,
        bool use_color,
        bool use_emojis
    ) {
        list_tests_calls.push_back(list_tests_call{&tsts, &os, use_color, use_emojis});
    };
    deps.is_arg_specified_fn = [this](
        vector<string_type>& args_vec,
        char short_arg,
        char const* long_arg
    ) {
        is_arg_specified_calls.push_back(is_arg_specified_call{&args_vec, short_arg, long_arg});
        return tst::is_arg_specified(args_vec, short_arg, long_arg);
    };
    deps.get_arg_value_fn = [this](
        vector<string_type>& args_vec,
        char short_arg,
        char const* long_arg,
        bool allow_dash
    ) {
        get_arg_value_calls.push_back(
            get_arg_value_call{
                &args_vec,
                short_arg,
                long_arg,
                allow_dash
            }
        );
        return tst::get_arg_value(args_vec, short_arg, long_arg, allow_dash);
    };
    deps.isatty_fn = [this](int fd) {
        is_atty_calls.push_back(fd);
        return is_atty_result ? 1 : 0;
    };
    deps.get_hardware_concurrency_fn = [this] {
        return hw_concurrency;
    };
    deps.make_json_reporter_fn = [this](
        report_ostream_type& os,
        size_t n_tests,
        size_t concurrency
    ) {
        if (&os != &out_os) {
            os << "test-text";
        }
        make_json_reporter_calls.push_back(make_json_reporter_call{&os, n_tests, concurrency});
        return p_rep.release();
    };
    deps.make_junit_reporter_fn = [this](
        report_ostream_type& os,
        string_type const& prog_name,
        size_t n_tests,
        string_type const& tests_report_name
    ) {
        if (&os != &out_os) {
            os << "test-text";
        }
        make_junit_reporter_calls.push_back(
            make_junit_reporter_call{
                &os,
                prog_name,
                n_tests,
                tests_report_name
            }
        );
        return p_rep.release();
    };
    deps.make_delim_reporter_fn = [this](
        report_ostream_type& os,
        char_type delim
    ) {
        if (&os != &out_os) {
            os << "test-text";
        }
        make_delim_reporter_calls.push_back(
            make_delim_reporter_call{
                &os,
                delim
            }
        );
        return p_rep.release();
    };
    deps.make_plain_text_reporter_fn = [this](
        report_ostream_type& os,
        vector<test_case_instance> const& tests,
        size_t concurrency,
        bool use_color,
        bool emojis
    ) {
        if (&os != &out_os) {
            os << "test-text";
        }
        make_plain_text_reporter_calls.push_back(
            make_plain_text_reporter_call{
                &os,
                &tests,
                concurrency,
                use_color,
                emojis
            }
        );
        return p_rep.release();
    };
    deps.make_custom_reporter_fn = [this](test_case_reporter_args const& args) {
        if (args.p_report_ostream != &out_os) {
            *args.p_report_ostream << "test-text";
        }
        make_custom_reporter_calls.push_back(args);
        return p_rep.release();
    };
    deps.with_redirection_fn = [this](function<int()> const& f) {
        ++with_redirection_calls;
        tst::with_redirection_result ret;
        ret.status = f();
        ret.stdout = TESTING_STR_LIT("stdout data");
        ret.stderr = TESTING_STR_LIT("stderr data");
        return ret;
    };
    deps.run_test_cases_fn = [this](
        vector<test_case_instance>& tsts,
        test_case_reporter& r,
        size_t concurrency,
        bool randomized
    ) {
        run_test_cases_calls.push_back(run_test_cases_call{&tsts, &r, concurrency, randomized});
        return run_test_cases_result;
    };
    deps.reporter_finish_fn = [this](
            test_case_reporter& rep,
            string_type const& stdout_data,
            string_type const& stderr_data
    ) {
        reporter_finish_calls.push_back(reporter_finish_call{&rep, stdout_data, stderr_data});
    };
}

test_main_ctx::test_main_ctx(
    string dsc,
    test_main_args const& arg,
    int run_test_cases_res,
    bool is_atty_res,
    size_t hw_concur,
    vector<string_type> tst_args
) : test_main_ctx{std::move(dsc), tst_args, run_test_cases_res, is_atty_res, hw_concur} {
    if (!arg.use_color) {
        args.push_back(TESTING_STR_LIT("--no-color"));
    }
    if (arg.emojis_short) {
        args.push_back(TESTING_STR_LIT("-e"));
    } else if (arg.emojis_long) {
        args.push_back(TESTING_STR_LIT("--emojis"));
    }
    if (arg.randomized_short) {
        args.push_back(TESTING_STR_LIT("-r"));
    } else if (arg.randomized_long) {
        args.push_back(TESTING_STR_LIT("--randomized"));
    }
    if (arg.capture_short) {
        args.push_back(TESTING_STR_LIT("-c"));
    } else if (arg.capture_long) {
        args.push_back(TESTING_STR_LIT("--capture"));
    }
    if (arg.parallel_short || arg.parallel_long) {
        args.push_back(
            arg.parallel_short ? TESTING_STR_LIT("-p") : TESTING_STR_LIT("--parallel")
        );
        if (arg.parallel_arg) {
            ostringstream_type oss;
            oss << test_main_args::parallel_value;
            args.push_back(oss.str());
        }
    }
}

void test_main_ctx::common_reporter_asserts(
    int res,
    std::size_t concurrency_req,
    bool randomized,
    bool output_captured
) const {
    testing_assert_msg(res == run_test_cases_result, desc);
    testing_assert_msg(usage_calls.empty(), desc);
    testing_assert_msg(list_tests_calls.empty(), desc);
    auto const p_match_call = get_get_arg_value_call('m', "match");
    testing_assert_msg(p_match_call, desc);
    testing_assert_msg(p_match_call->p_args == &args, desc);
    auto const p_help_call = get_is_arg_specified_call('h', "help");
    testing_assert_msg(p_help_call, desc);
    testing_assert_msg(p_help_call->p_args == &args, desc);
    auto const p_no_color_call = get_is_arg_specified_call(0, "no-color");
    testing_assert_msg(p_no_color_call, desc);
    testing_assert_msg(p_no_color_call->p_args == &args, desc);
    auto const p_list_call = get_is_arg_specified_call('l', "list");
    testing_assert_msg(p_list_call, desc);
    testing_assert_msg(p_list_call->p_args == &args, desc);
    auto const p_emojis_call = get_is_arg_specified_call('e', "emojis");
    testing_assert_msg(p_emojis_call, desc);
    testing_assert_msg(p_emojis_call->p_args == &args, desc);
    testing_assert_msg(get_get_arg_value_call('d', "delim"), desc);
    testing_assert_msg(get_is_arg_specified_call('j', "json"), desc);
    testing_assert_msg(get_is_arg_specified_call('j', "json"), desc);
    testing_assert_msg(get_get_arg_value_call(0, "junit"), desc);
    testing_assert_msg(get_is_arg_specified_call('r', "randomized"), desc);
    testing_assert_msg(get_is_arg_specified_call('c', "capture"), desc);
    testing_assert_msg(get_get_arg_value_call('p', "parallel"), desc);
    testing_assert_msg(get_get_arg_value_call('n', "number"), desc);
    testing_assert_msg(get_is_arg_specified_call('t', "text"), desc);
    testing_assert_msg(run_test_cases_calls.size() == 1, desc);
    auto const& call = run_test_cases_calls[0];
    testing_assert_msg(call.concurrency == min({concurrency_req, tests.size()}), desc);
    testing_assert_msg(call.p_tests == &tests, desc);
    testing_assert_msg(call.p_rep == p_rep_val, desc);
    testing_assert_msg(call.randomized == randomized, desc);
    testing_assert_msg(with_redirection_calls == (output_captured ? 1 : 0), desc);
    testing_assert_msg(reporter_finish_calls.size() == 1, desc);
    auto const& rf_calls = reporter_finish_calls[0];
    testing_assert_msg(rf_calls.p_rep == p_rep_val, desc);
    if (output_captured) {
        testing_assert_msg(!rf_calls.stdout_data.empty(), desc);
        testing_assert_msg(!rf_calls.stderr_data.empty(), desc);
        testing_assert_msg(!out_os.str().empty(), desc);
    } else {
        testing_assert_msg(rf_calls.stdout_data.empty(), desc);
        testing_assert_msg(rf_calls.stderr_data.empty(), desc);
        testing_assert_msg(out_os.str().empty(), desc);
    }
}
