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

#if !defined(NYEJ_TUXX_TESTING_HPP)
#define NYEJ_TUXX_TESTING_HPP

#include <algorithm>
#include <cctype>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include <tuxx.hpp>

struct testing_error {
    char const* file{};
    int line{};
    std::string detail;

    testing_error(
        char const* f,
        int l,
        std::string d
    ) : file{f}, line{l}, detail{std::move(d)} {}
};

#define testing_assert(expr) \
    do { \
        if (!(expr)) { \
            throw testing_error{__FILE__, __LINE__, std::string(#expr) + " failed"}; \
        } \
    } while (false)

#define testing_assert_msg(expr, msg) \
    do { \
        if (!(expr)) { \
            throw testing_error{ \
                __FILE__, \
                __LINE__, \
                std::string(#expr) + " [" + msg + "] failed" \
            }; \
        } \
    } while (false)

// Define our own char info to verify tuxx's is doing the right thing...
#if defined(TUXX_CHAR_TYPE_IS_CHAR)
    using char_type = char;
    #define TESTING_STR_LIT(x) x
    #define TESTING_LIT_PREFIX
#elif defined(TUXX_CHAR_TYPE_IS_WCHAR_T)
    using char_type = wchar_t;
    #define TESTING_STR_LIT(x) L ## x
    #define TESTING_LIT_PREFIX "L"
#else
    #error unsupported character type
#endif

using string_type = std::basic_string<char_type>;

template <
    typename ChDst,
    typename ChSrc
> inline
std::basic_string<ChDst> simple_str_conv(ChSrc const* s) {
    std::basic_string<ChDst> ret;
    for ( ; *s; ++s) {
        ret += static_cast<ChDst>(*s);
    }
    return ret;
}

inline
std::string simple_str_conv(char const* s) {
    return s;
}

inline
std::wstring simple_str_conv(wchar_t const* s) {
    return s;
}

inline
std::string simple_narrow(std::wstring const& s) {
    return simple_str_conv<char>(s.c_str());
}

inline
std::string const& simple_narrow(std::string const& s) {
    return s;
}

// Forward-declared entry point
void testing_main(std::vector<string_type> const& args);

inline
int testing_main_wrapper(std::vector<string_type> const& args) {
    using namespace std;
    try {
        testing_main(args);
        return 0;
    } catch (testing_error const& ex) {
        cerr << "TEST FAILED - " << ex.file << ':' << ex.line << ": " << ex.detail << endl;
    } catch (exception const& ex) {
        cerr << "TEST FAILED - unhandled exception: " << ex.what() << endl;
    } catch (...) {
        cerr << "TEST FAILED - unknown exception" << endl;
    }
    return 1;
}

struct testing_mutex {
    bool is_locked_;
    static testing_mutex* p_instance;
    testing_mutex() : is_locked_{} { p_instance = this; }
    ~testing_mutex() { p_instance = nullptr; }
    void lock() { is_locked_ = true; }
    void unlock() { is_locked_ = false; }
    bool is_locked() const { return is_locked_; }
    static testing_mutex* instance() { return p_instance; }
};

struct testing_unique_lock {
    testing_mutex* p_mtx;
    explicit testing_unique_lock(testing_mutex& m) : p_mtx{&m} { p_mtx->lock(); }
    ~testing_unique_lock() { p_mtx->unlock(); }
};

template <
    typename DurationType,
    typename MutexType
>
struct basic_testing_test_case_reporter : ::nyej::tuxx::basic_test_case_reporter<
    DurationType,
    MutexType
> {
    using test_case_instance_type = ::nyej::tuxx::basic_test_case_instance<DurationType, MutexType>;
    using duration_type = DurationType;
    using string_type = ::nyej::tuxx::string_type;
    struct tc_starting_call {
        test_case_instance_type const* tc;
        bool is_mtx_locked;
    };
    struct tc_assert_call {
        test_case_instance_type const* tc;
        char const* file;
        int line;
        char const* assert_name;
        char const* expr;
        string_type const& msg;
        char const* ex_info;
        bool is_mtx_locked;
    };
    struct tc_passed_call {
        test_case_instance_type const* tc;
        duration_type elapsed;
        bool is_mtx_locked;
    };
    struct tc_failed_call {
        test_case_instance_type const* tc;
        ::nyej::tuxx::test_case_failure_error err;
        duration_type elapsed;
        bool is_mtx_locked;
    };
    struct end_tcs_call {
        duration_type elapsed;
        std::vector<test_case_instance_type> failures;
    };
    struct finish_call {
        string_type stdout_data;
        string_type stderr_data;
        bool is_mtx_locked;
    };
    std::size_t start_ct{};
    std::vector<tc_starting_call> tc_starting_calls;
    std::vector<tc_assert_call> tc_assert_calls;
    std::vector<tc_passed_call> tc_passed_calls;
    std::vector<tc_failed_call> tc_failed_calls;
    std::vector<end_tcs_call> end_tcs_calls;
    std::vector<finish_call> finish_calls;
    void start() override {
        ++start_ct;
    }
    void test_case_starting(test_case_instance_type const& tc) override {
        tc_starting_calls.push_back(
            tc_starting_call{
                &tc,
                testing_mutex::instance() && testing_mutex::instance()->is_locked()
            }
        );
    }
    void test_case_assert(
        test_case_instance_type const& tc,
        char const* file,
        int line,
        char const* assert_name,
        char const* expr,
        string_type const& msg,
        char const* ex_info = nullptr
    ) override {
        tc_assert_calls.push_back(
            tc_assert_call{
                &tc,
                file,
                line,
                assert_name,
                expr,
                msg,
                ex_info,
                testing_mutex::instance() && testing_mutex::instance()->is_locked()
            }            
        );
    }
    void test_case_passed(
        test_case_instance_type const& tc,
        duration_type const& elapsed
    ) override {
        tc_passed_calls.push_back(
            tc_passed_call{
                &tc,
                elapsed,
                testing_mutex::instance() && testing_mutex::instance()->is_locked()
            }
        );
    }
    void test_case_failed(
        test_case_instance_type const& tc,
        ::nyej::tuxx::test_case_failure_error const& err,
        duration_type const& elapsed
    ) override {
        tc_failed_calls.push_back(
            tc_failed_call{
                &tc,
                err,
                elapsed,
                testing_mutex::instance() && testing_mutex::instance()->is_locked()
            }
        );
    }
    void end_test_cases(
        duration_type const& elapsed,
        std::vector<test_case_instance_type> const& failures
    ) override {
        end_tcs_calls.push_back(end_tcs_call{elapsed, failures});
    }
    void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) override {
        finish_calls.push_back(finish_call{stdout_data, stderr_data});
    }
};

using testing_test_case_reporter = basic_testing_test_case_reporter<int, testing_mutex>;

struct test_main_args {
    bool use_color;
    bool emojis_short;
    bool emojis_long;
    bool randomized_short;
    bool randomized_long;
    bool capture_short;
    bool capture_long;
    bool parallel_short;
    bool parallel_long;
    bool parallel_arg;
    static std::size_t constexpr parallel_value{3};

    static void combinations(std::function<void(test_main_args const&)> const& f);
};

template <typename Ch> inline
std::basic_ostream<Ch>& operator<<(
    std::basic_ostream<Ch>& os,
    test_main_args const& args
) {
    return os << "{color: " << args.use_color
        << ", emojis_short: " << args.emojis_short
        << ", emojis_long: " << args.emojis_long
        << ", rand_short: " << args.randomized_short
        << ", rand_long: " << args.randomized_long
        << ", capture_short: " << args.capture_short
        << ", capture_long: " << args.capture_long
        << ", para_short: " << args.parallel_short
        << ", para_long: " << args.parallel_long
        << ", para_arg: " << args.parallel_arg << '}';
}

struct test_main_ctx {
    using report_ostream_type = ::nyej::tuxx::report_ostream_type;
    using string_type = ::nyej::tuxx::string_type;
    using char_type = ::nyej::tuxx::char_type;
    using ostringstream_type = ::nyej::tuxx::ostringstream_type;

    struct usage_call {
        string_type prog_name;
        report_ostream_type const* p_os;
    };

    struct list_tests_call {
        std::vector<::nyej::tuxx::test_case_instance> const* p_tests;
        report_ostream_type const* p_os;
        bool use_color;
        bool use_emojis;
    };

    struct is_arg_specified_call {
        std::vector<string_type> const* p_args;
        char short_arg;
        char const* long_arg;
    };

    struct get_arg_value_call {
        std::vector<string_type> const* p_args;
        char short_arg;
        char const* long_arg;
        bool allow_dash;
    };

    struct make_json_reporter_call {
        report_ostream_type const* p_os;
        std::size_t n_tests;
        std::size_t concurrency;
    };

    struct make_junit_reporter_call {
        report_ostream_type const* p_os;
        string_type prog_name;
        std::size_t n_tests;
        string_type test_report_name;
    };

    struct make_delim_reporter_call {
        report_ostream_type const* p_os;
        char_type delim;
    };

    struct make_plain_text_reporter_call {
        report_ostream_type const* p_os;
        std::vector<::nyej::tuxx::test_case_instance> const* p_tests;
        std::size_t concurrency;
        bool use_color;
        bool use_emojis;
    };

    struct run_test_cases_call {
        std::vector<::nyej::tuxx::test_case_instance> const* p_tests;
        ::nyej::tuxx::test_case_reporter const* p_rep;
        std::size_t concurrency;
        bool randomized;
    };

    struct reporter_finish_call {
        ::nyej::tuxx::test_case_reporter const* p_rep;
        string_type stdout_data;
        string_type stderr_data;
    };

    std::string desc;
    std::vector<string_type> args;
    int run_test_cases_result;
    bool is_atty_result{};
    std::size_t hw_concurrency{};
    std::unique_ptr<::nyej::tuxx::test_case_reporter> p_rep;
    void const* p_rep_val{};  // Not for dereferencing, only for tracking address value.
    std::vector<::nyej::tuxx::test_case_instance> tests;
    ostringstream_type out_os;
    ostringstream_type err_os;
    std::vector<usage_call> usage_calls;
    std::vector<list_tests_call> list_tests_calls;
    std::vector<is_arg_specified_call> is_arg_specified_calls;
    std::vector<get_arg_value_call> get_arg_value_calls;
    std::vector<int> is_atty_calls;
    std::size_t get_hw_concurrency_calls{};
    std::vector<make_json_reporter_call> make_json_reporter_calls;
    std::vector<make_junit_reporter_call> make_junit_reporter_calls;
    std::vector<make_delim_reporter_call> make_delim_reporter_calls;
    std::vector<make_plain_text_reporter_call> make_plain_text_reporter_calls;
    std::vector<::nyej::tuxx::test_case_reporter_args> make_custom_reporter_calls;
    std::size_t with_redirection_calls{};
    std::vector<run_test_cases_call> run_test_cases_calls;
    std::vector<reporter_finish_call> reporter_finish_calls;
    ::nyej::tuxx::tst::main_deps deps;

    explicit test_main_ctx(
        std::string dsc,
        std::vector<string_type> tst_args = std::vector<string_type>(),
        int run_test_cases_res = 0,
        bool is_atty_res = true,
        std::size_t hw_concur = 8
    );

    explicit test_main_ctx(
        std::string dsc,
        test_main_args const& arg,
        int run_test_cases_res = 0,
        bool is_atty_res = true,
        std::size_t hw_concur = 8,
        std::vector<string_type> tst_args = std::vector<string_type>()
    );

    template <
        typename V,
        typename F
    >
    typename V::const_pointer lookup_call(
        V const& calls,
        F const& pred
    ) const {
        auto const it = std::find_if(calls.begin(), calls.end(), pred);
        if (it == calls.end()) {
            return nullptr;
        }
        return &*it;
    }

    template <typename V>
    typename V::const_pointer get_arg_query_call(
        V const& calls,
        char short_arg,
        char const* long_arg
    ) const {
        return lookup_call(
            calls,
            [&](typename V::value_type const& val) {
                if (short_arg && (val.short_arg != short_arg)) {
                    return false;
                }
                return !long_arg || std::strcmp(val.long_arg, long_arg) == 0;
            }
        );
    }

    is_arg_specified_call const* get_is_arg_specified_call(
        char short_arg,
        char const* long_arg
    ) const {
        return get_arg_query_call(is_arg_specified_calls, short_arg, long_arg);
    }

    get_arg_value_call const* get_get_arg_value_call(
        char short_arg,
        char const* long_arg
    ) const {
        return get_arg_query_call(get_arg_value_calls, short_arg, long_arg);
    }

    int run_main(::nyej::tuxx::test_case_reporter* rep = nullptr) {
        return ::nyej::tuxx::tst::main(args, tests, out_os, err_os, rep, deps);
    }

    // Asserts that need to be done for any reporter
    void common_reporter_asserts(
        int res,
        std::size_t concurrency_req,
        bool randomized,
        bool output_captured = true
    ) const;

private:
    test_main_ctx(test_main_ctx const&);
    test_main_ctx& operator=(test_main_ctx const&);
    test_main_ctx(test_main_ctx&&);
    test_main_ctx& operator=(test_main_ctx&&);
};

template <
    typename Ch,
    typename T
> inline
std::basic_ostream<Ch>& operator<<(
    std::basic_ostream<Ch>& os,
    std::vector<T> const& v
) {
    auto is_first = true;
    for (auto const& elem : v) {
        if (!is_first) { os << ','; };
        is_first = false;
        os << elem;
    }
    return os;
}

template <typename Ch> inline
std::basic_ostream<Ch>& operator<<(
    std::basic_ostream<Ch>& os,
    test_main_ctx const& ctx
) {
    return os << "{description: " << ctx.desc
        << ", args: " << ctx.args
        << ", run_test_cases_result: " << ctx.run_test_cases_result
        << ", is_atty: " << ctx.is_atty_result
        << ", hw_concurrency: " << ctx.hw_concurrency << '}';
}

#if defined(TESTING_DEFINE_MAIN)

#if defined(_WIN32)
    #if defined(TUXX_USE_WINMAIN)
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
    #endif
#elif defined(TUXX_IS_WINDOWS)
    #include <cstring>

    // Emulate what we need from Windows headers

    // NOTE: for the purposes of this test, it doesn't matter whether wchar_t
    //       is 16- or 32-bit.
    using HINSTANCE = void*;
    using BOOL = std::uint8_t;
    using DWORD = std::uint32_t;
    #if defined(TUXX_IS_WINDOWS_UNICODE)
        using TCHAR = wchar_t;
    #else
        using TCHAR = char;
    #endif
    #if defined(TUXX_USE_WINMAIN)
        #define WINAPI
        using LPTSTR = TCHAR*;
        static TCHAR** argv__;
        inline
        unsigned GetProcessImageFileName(
            void*,
            LPTSTR buf,
            unsigned bufSize
        ) {
            TCHAR* pEnd{argv__[0]};
            for (; *pEnd; ++pEnd) {}
            std::size_t const len = pEnd - argv__[0];
            std::memcpy(buf, argv__[0], std::min<std::size_t>(bufSize, len));
            return len;
        }
        inline
        void* GetCurrentProcess() {
            return nullptr;
        }
    #endif
#endif

#if defined(TUXX_IS_WINDOWS)
    #if defined(TUXX_USE_WINMAIN)
        int WINAPI WinMain(
            HINSTANCE,
            HINSTANCE,
            LPTSTR lpCmdLine,
            int
        ) {
            // Very simple argument parsing: no consideration of quoted values.
            auto args = ::nyej::tuxx::split_delim(
                std::basic_string<TCHAR>{lpCmdLine},
                ' '
            );
            TCHAR progName[1000];
            ::GetProcessImageFileName(
                ::GetCurrentProcess(),
                progName,
                sizeof(progName) / sizeof(progName[0])
            );
            args.insert(args.begin(), progName);
            return testing_main_wrapper(args);
        }
        #define TESTING_MAIN_IS_DEFINED
    #elif defined(TUXX_IS_WINDOWS_UNICODE)
        int wmain(
            int argc,
            wchar_t* argv[]
        ) {
            using namespace std;
            vector<string_type> args(argv, argv + argc);
            return testing_main_wrapper(argc, argv);
        }
        #define TESTING_MAIN_IS_DEFINED
    #endif
#endif

#if !defined(TESTING_MAIN_IS_DEFINED)
    int main(
        int argc,
        char* argv[]
    ) {
        using namespace std;
        vector<string_type> args;
        for (auto i = 0; i < argc; ++i) {
            args.push_back(simple_str_conv<char_type>(argv[i]));
        }
        return testing_main_wrapper(args);
    }
#endif

#if !defined(_WIN32) && defined(TUXX_IS_WINDOWS)
    // Create a main that calls WinMain/wmain as needed to emulate windows
    #if defined(TUXX_USE_WINMAIN)
        int main(
            int argc,
            char* argv[]
        ) {
            using namespace std;
            #if defined(TUXX_IS_WINDOWS_UNICODE)
                wstring cmdLine;
                vector<wstring> args;
                for (auto i = 0; i < argc; ++i) {
                    if (i > 1) {
                        cmdLine += ' ';
                    }
                    if (i) {
                        cmdLine += simple_str_conv<wchar_t>(argv[i]);
                    }
                    args.push_back(arg);
                }
                vector<wchar_t*> args_ptrs;
                for (auto& arg : args) {
                    args_ptrs.push_back(&arg[0]);
                }
                argv__ = args_ptrs.data();
            #else
                string cmdLine;
                for (auto i = 1; i < argc; ++i) {
                    if (i > 1) {
                        cmdLine += ' ';
                    }
                    cmdLine += argv[i];
                }
                argv__ = argv;
            #endif
            return WinMain(nullptr, nullptr, &cmdLine[0], 0);
        }
    #elif defined(TUXX_IS_WINDOWS_UNICODE)
        // Windows emulation path.
        int main(
            int argc,
            char* argv[]
        ) {
            using namespace std;
            vector<wstring> args;    
            for (auto i = 0; i < argc; ++i) {
                wstring arg;
                // Do a simple char -> wchar_t conversion here...
                for (auto j = 0; argv[i][j]; ++j) {
                    arg += static_cast<wchar_t>(argv[i][j]);
                }
                args.push_back(arg);
            }
            vector<wchar_t*> str_args(args.size());
            transform(
                args.begin(),
                args.end(),
                str_args.begin(),
                [](wstring& s) { return &s[0]; }
            );
            return wmain(argc - 1, str_args.data());
        }
    #endif
#endif

#endif

#endif
