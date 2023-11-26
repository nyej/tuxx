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

#if !defined(NYEJ_TUXX_HPP)
#define NYEJ_TUXX_HPP

/*

                     _________   _    _   __    __   __    __
                    |___   ___| | |  | |  \ \  / /   \ \  / /
                        | |     | |  | |   \ \/ /     \ \/ /
                        | |     | |  | |    \  /       \  /
                        | |     | |  | |    /  \       /  \
                        | |     | |__| |   / /\ \     / /\ \
                        |_|     \______/  /_/  \_\   /_/  \_\  

    (Test-the-Units++) a simple, modern, extensible header-only C++ unit testing library.

 */
#include <chrono>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#if (!defined(TUXX_IS_WINDOWS) && !defined(TUXX_IS_UNIX))
    // Auto-detect the operating system
    #if defined(_WIN32)
        #define TUXX_IS_WINDOWS
        #if defined(UNICODE)
            #define TUXX_IS_WINDOWS_UNICODE
        #endif
    #elif defined(__unix__) || defined(__APPLE__)
        #define TUXX_IS_UNIX
    #else
        #error unsupported operating system
    #endif
#endif

#if defined(_WIN32)
    #include <io.h>

    #define tuxx_os_dup ::_dup
    #define tuxx_os_dup2 ::_dup2
    #define tuxx_os_pipe(pipe_fds) ::_pipe(pipe_fds, 4096, O_BINARY)
    #define tuxx_os_read ::_read
    #define tuxx_os_close ::_close

    inline int tuxx_os_isatty(int) {
        return false;
    }

    // Due to issues with include order and windows.h, we assume the user has
    // included windows.h the way they need to prior to including this header.
    inline
    int gethostname(
        char* buf,
        std::size_t len
    ) {
        return ::GetComputerNameA(buf, static_cast<DWORD>(len)) ? 0 : -1;
    }
#else
    #include <unistd.h>

    #define tuxx_os_dup ::dup
    #define tuxx_os_dup2 ::dup2
    #define tuxx_os_pipe ::pipe
    #define tuxx_os_read ::read
    #define tuxx_os_close ::close
    #define tuxx_os_isatty ::isatty
#endif

#if ( \
    !defined(TUXX_CHAR_TYPE_IS_CHAR) && \
    !defined(TUXX_CHAR_TYPE_IS_WCHAR_T) \
)
    #if defined(TUXX_IS_WINDOWS) && defined(TUXX_IS_WINDOWS_UNICODE)
        #define TUXX_CHAR_TYPE_IS_WCHAR_T
    #else
        #define TUXX_CHAR_TYPE_IS_CHAR
    #endif
#endif

#if defined(TUXX_CHAR_TYPE_IS_CHAR)
    namespace nyej { namespace tuxx { using char_type = char; } }
    #if !defined(TUXX_REPORT_OSTRM)
        #define TUXX_REPORT_OSTRM std::cout
    #endif
    #if !defined(TUXX_ERROR_OSTRM)
        #define TUXX_ERROR_OSTRM std::cerr
    #endif
    #define TUXX_STR_LIT(x) x
#elif defined(TUXX_CHAR_TYPE_IS_WCHAR_T)
    namespace nyej { namespace tuxx { using char_type = wchar_t; } }
    #if !defined(TUXX_REPORT_OSTRM)
        #define TUXX_REPORT_OSTRM std::wcout
    #endif
    #if !defined(TUXX_ERROR_OSTRM)
        #define TUXX_ERROR_OSTRM std::wcerr
    #endif
    #define TUXX_STR_LIT(x) L ## x
#else
    #error One of TUXX_CHAR_TYPE_IS_CHAR or TUXX_CHAR_TYPE_IS_WCHAR_T must be defined.
#endif

#if !defined(TUXX_EMOJI_PREFIX)
    #define TUXX_EMOJI_PREFIX TUXX_STR_LIT("\U0001f9ea")
#endif
#if !defined(TUXX_EMOJI_PASSED)
    #define TUXX_EMOJI_PASSED TUXX_STR_LIT("\u2705")
#endif
#if !defined(TUXX_EMOJI_FAILED)
    #define TUXX_EMOJI_FAILED TUXX_STR_LIT("\u274c")
#endif
#if !defined(TUXX_EMOJI_SUCCESS)
    #define TUXX_EMOJI_SUCCESS TUXX_STR_LIT("\U0001f60e")
#endif
#if !defined(TUXX_EMOJI_FAILURE)
    #define TUXX_EMOJI_FAILURE TUXX_STR_LIT("\U0001f912")
#endif

namespace nyej {
namespace tuxx {

using ostringstream_type = std::basic_ostringstream<char_type>;
using report_ostream_type = std::basic_ostream<char_type>;
using string_type = std::basic_string<char_type>;

namespace detail {

inline
std::string to_char(char const* s) {
    return s;
}

inline
std::string const& to_char(std::string const& s) {
    return s;
}

}
}
}

#if defined(TUXX_CHAR_TYPE_IS_CHAR)
    namespace nyej {
    namespace tuxx {
    namespace detail {

    inline
    std::string to_char_type(char const* s) {
        return to_char(s);
    }

    inline
    std::string const& to_char_type(std::string const& s) {
        return to_char(s);
    }

    }
    }
    }
#elif defined(TUXX_CHAR_TYPE_IS_WCHAR_T)
    #include <cwchar>

    namespace nyej {
    namespace tuxx {
    namespace detail {

    inline
    std::wstring to_char_type(char const* s) {
        using namespace std;
        std::wostringstream oss;
        oss << s;
        return oss.str();
    }

    inline
    std::wstring to_char_type(std::string const& s) {
        return to_char_type(s.c_str());
    }

    inline
    std::string to_char(wchar_t const* s) {
        using namespace std;
        mbstate_t st{};
        auto p_c_str = reinterpret_cast<wchar_t const*>(s);
        auto const len = wcsrtombs(nullptr, &p_c_str, 0, &st);
        if (len == static_cast<size_t>(-1)) {
            return "?bad conversion?";
        }
        string ret(len, ' ');
        if (wcsrtombs(&ret[0], &p_c_str, ret.length(), &st) == static_cast<size_t>(-1)) {
            return "?bad conversion?";  // Should not happen.
        }
        return ret;
    }

    inline
    std::string to_char(std::wstring const& s) {
        return to_char(s.c_str());
    }

    }
    }
    }
#endif

template <typename StrCh> inline
typename std::enable_if<
    !std::is_same<StrCh, ::nyej::tuxx::char_type>::value,
    std::basic_ostream<::nyej::tuxx::char_type>&
>::type operator<<(
    std::basic_ostream<::nyej::tuxx::char_type>& os,
    std::basic_string<StrCh> const& s
) {
    return os << ::nyej::tuxx::detail::to_char_type(s);
}

template <typename Ch> inline
std::basic_ostream<Ch>& operator<<(
    std::basic_ostream<Ch>& os,
    std::nullptr_t const&
) {
    return os << "(null)";
}

namespace nyej {
namespace tuxx { 

struct test_case_reporter;

namespace detail {

struct testing_context;

}

using test_fn_type = std::function<void(detail::testing_context const&)>;

struct test_case_instance {
    char const* file{};
    int line{};
    string_type name;
    std::size_t idx{};
    test_fn_type fn;
    string_type arg;

    test_case_instance() {}
    test_case_instance(
        char const* f,
        int l,
        string_type n,
        std::size_t i,
        test_fn_type fun,
        string_type a = string_type{}
    ) : file{f}, line{l}, name{std::move(n)}, idx{i}, fn{std::move(fun)}, arg{std::move(a)} {
        replace_ctrl_chars(name);
        replace_ctrl_chars(arg);
    }
private:
    void replace_ctrl_chars(string_type& s) {
        for (auto& c : s) {
            if ((c < ' ') && (c != '\t')) {
                c = ' ';
            }
        }
    }
};

struct test_case_failure_error {
    char const* file;
    int line;
    string_type detail;
};

namespace detail {

struct testing_context {
    test_case_instance const* tc{};
    test_case_reporter* reporter{};
    std::mutex* mtx{};

    testing_context(
        test_case_instance const* p_tc,
        test_case_reporter* p_r,
        std::mutex* p_mtx
    ) : tc{p_tc}, reporter{p_r}, mtx{p_mtx} {}
};

struct test_case_adder {
    test_case_adder(
        char const* file,
        int line,
        char const* name,
        std::vector<test_case_instance>& tests,
        test_fn_type fn
    ) {
        tests.push_back(
            test_case_instance{
                file,
                line,
                detail::to_char_type(name),
                tests.size(),
                std::move(fn)
            }
        );
    }
};

struct test_case_multi_adder {
    template <
        typename ParamIter,
        typename Fn
    >
    test_case_multi_adder(
        char const* file,
        int line,
        char const* name,
        std::vector<test_case_instance>& tests,
        ParamIter args_beg,
        ParamIter args_end,
        Fn const& fn
    ) {
        using namespace std;
        for ( ; args_beg != args_end; ++args_beg) {
            auto const& arg = *args_beg;
            ostringstream_type oss;
            oss << arg;
            auto const arg_str = oss.str();
            tests.push_back(
                test_case_instance{
                    file,
                    line,
                    detail::to_char_type(name),
                    tests.size(),
                    [fn, &arg](testing_context const& ctx) {
                        fn(ctx, arg);
                    },
                    arg_str
                }
            );
        }
    }
};

std::vector<test_case_instance>& tests__();

#if defined(TUXX_DEFINE_TEST_MAIN)
std::vector<test_case_instance>& tests__() {
    static std::vector<test_case_instance> tests;
    return tests;
}
#endif

template <typename Ch> inline
bool iequals(
    Ch const* a,
    Ch const* b
) {
    auto i = 0u;
    for (; a[i] && b[i] && (std::tolower(a[i]) == std::tolower(b[i])); ++i) {
    }
    return !a[i] && !b[i];
}

template <typename Ch> inline
bool iequals(
    std::basic_string<Ch> const& a,
    Ch const* b
) {
    return iequals(a.c_str(), b);
}

template <typename Ch> inline
bool iequals(
    Ch const* a,
    std::basic_string<Ch> const& b
) {
    return iequals(a, b.c_str());
}

template <typename Ch> inline
bool iequals(
    std::basic_string<Ch> const& a,
    std::basic_string<Ch> const& b
) {
    return iequals(a.c_str(), b.c_str());
}

}

struct test_case_reporter {
    virtual ~test_case_reporter() = default;
    virtual void start() {}
    // Override this only if running tests sequentially (not in parallel) and
    // you want to show the test case name before the test case completes.
    virtual void test_case_starting(test_case_instance const& tc) {}
    virtual void test_case_assert(
        test_case_instance const& tc,
        char const* file,
        int line,
        char const* assert_name,
        char const* expr,
        string_type const& msg,
        char const* ex_info = nullptr
    ) {}
    virtual void test_case_passed(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed
    ) = 0;
    virtual void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        std::chrono::steady_clock::duration const& elapsed
    ) = 0;
    virtual void end_test_cases(
        std::chrono::steady_clock::duration const& elapsed,
        std::vector<test_case_instance> const& failures
    ) = 0;
    virtual void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) = 0;
};

// Passed to the TUXX_USER_TEST_CASE_REPORTER_INIT custom reporter
// creation macro. 
struct test_case_reporter_args {
    report_ostream_type* p_report_ostream{};
    test_case_instance const* p_test_cases{}; // Array of test cases.
    std::size_t n_tests{};
    std::size_t concurrency{};
    bool capture_output{};
    bool write_colorized{};
    bool use_emojis{};
};


struct test_case_reporter_fns : test_case_reporter {
    using start_fn_type = std::function<void()>;
    using test_case_starting_fn_type = std::function<void(test_case_instance const& tc)>;
    using test_case_assert_fn_type = std::function<
        void(
            test_case_instance const& tc,
            char const* file,
            int line,
            char const* assert_name,
            char const* expr,
            string_type const& msg,
            char const* opt_ex_info
        )
    >;
    using test_case_passed_fn_type = std::function<
        void(
            test_case_instance const& tc,
            std::chrono::steady_clock::duration const& elapsed
        )
    >;
    using test_case_failed_fn_type = std::function<
        void(
            test_case_instance const& tc,
            test_case_failure_error const& err,
            std::chrono::steady_clock::duration const& elapsed
        )
    >;
    using end_test_cases_fn_type = std::function<
        void(
            std::chrono::steady_clock::duration const& elapsed,
            std::vector<test_case_instance> const& failures
        )
    >;
    using finish_fn_type = std::function<
        void(
            string_type const& stdout_data,
            string_type const& stderr_data
        )
    >;

    start_fn_type start_fn;
    test_case_starting_fn_type test_case_starting_fn;
    test_case_assert_fn_type test_case_assert_fn;
    test_case_passed_fn_type test_case_passed_fn;
    test_case_failed_fn_type test_case_failed_fn;
    end_test_cases_fn_type end_test_cases_fn;
    finish_fn_type finish_fn;

    test_case_reporter_fns(
        start_fn_type start,
        test_case_starting_fn_type test_case_starting,
        test_case_assert_fn_type test_case_assert,
        test_case_passed_fn_type test_case_passed,
        test_case_failed_fn_type test_case_failed,
        end_test_cases_fn_type end_test_cases,
        finish_fn_type finish
    ) :
        start_fn{std::move(start)},
        test_case_starting_fn{std::move(test_case_starting)},
        test_case_assert_fn{std::move(test_case_assert)},
        test_case_passed_fn{std::move(test_case_passed)},
        test_case_failed_fn{std::move(test_case_failed)},
        end_test_cases_fn{std::move(end_test_cases)},
        finish_fn{std::move(finish)}
    {}

    void start() override {
        if (start_fn) {
            start_fn();
        }
    }
    void test_case_starting(test_case_instance const& tc) override {
        if (test_case_starting_fn) {
            test_case_starting_fn(tc);
        }
    }
    void test_case_assert(
        test_case_instance const& tc,
        char const* file,
        int line,
        char const* assert_name,
        char const* expr,
        string_type const& msg,
        char const* ex_info = nullptr
    ) override {
        if (test_case_assert_fn) {
            test_case_assert_fn(tc, file, line, assert_name, expr, msg, ex_info);
        }
    }
    void test_case_passed(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        if (test_case_passed_fn) {
            test_case_passed_fn(tc, elapsed);
        }
    }
    void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        if (test_case_failed_fn) {
            test_case_failed_fn(tc, err, elapsed);
        }
    }
    void end_test_cases(
        std::chrono::steady_clock::duration const& elapsed,
        std::vector<test_case_instance> const& failures
    ) override {
        if (end_test_cases_fn) {
            end_test_cases_fn(elapsed, failures);
        }
    }
    void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) override {
        if (finish_fn) {
            finish_fn(stdout_data, stderr_data);
        }
    }
};

struct test_case_reporter_fns_builder {
    test_case_reporter_fns::start_fn_type start_fn;
    test_case_reporter_fns::test_case_starting_fn_type test_case_starting_fn;
    test_case_reporter_fns::test_case_assert_fn_type test_case_assert_fn;
    test_case_reporter_fns::test_case_passed_fn_type test_case_passed_fn;
    test_case_reporter_fns::test_case_failed_fn_type test_case_failed_fn;
    test_case_reporter_fns::end_test_cases_fn_type end_test_cases_fn;
    test_case_reporter_fns::finish_fn_type finish_fn;

    test_case_reporter_fns_builder& handle_start(test_case_reporter_fns::start_fn_type f) {
        start_fn = std::move(f);
        return *this;
    }
    test_case_reporter_fns_builder& handle_test_case_starting(
        test_case_reporter_fns::test_case_starting_fn_type f
    ) {
        test_case_starting_fn = std::move(f);
        return *this;
    }
    test_case_reporter_fns_builder& handle_test_case_assert(
        test_case_reporter_fns::test_case_assert_fn_type f
    ) {
        test_case_assert_fn = std::move(f);
        return *this;
    }
    test_case_reporter_fns_builder& handle_test_case_passed(
        test_case_reporter_fns::test_case_passed_fn_type f
    ) {
        test_case_passed_fn = std::move(f);
        return *this;
    }
    test_case_reporter_fns_builder& handle_test_case_failed(
        test_case_reporter_fns::test_case_failed_fn_type f
    ) {
        test_case_failed_fn = std::move(f);
        return *this;
    }
    test_case_reporter_fns_builder& handle_end_test_cases(
        test_case_reporter_fns::end_test_cases_fn_type f
    ) {
        end_test_cases_fn = std::move(f);
        return *this;
    }
    test_case_reporter_fns_builder& handle_finish(test_case_reporter_fns::finish_fn_type f) {
        finish_fn = std::move(f);
        return *this;
    }

    test_case_reporter* build() {
        return new test_case_reporter_fns{
            start_fn,
            test_case_starting_fn,
            test_case_assert_fn,
            test_case_passed_fn,
            test_case_failed_fn,
            end_test_cases_fn,
            finish_fn
        };
    }
};

}
}

#define TUXX_MAKE_TEST_CASE_NAME3__(name, line) \
    test_case_ ## name ## _ ## line

#define TUXX_MAKE_TEST_CASE_NAME2__(name, line) \
    TUXX_MAKE_TEST_CASE_NAME3__(name, line)

#define TUXX_MAKE_TEST_CASE_NAME__(name) \
    TUXX_MAKE_TEST_CASE_NAME2__(name, __LINE__)

#define test_case(name) \
    static void TUXX_MAKE_TEST_CASE_NAME__(name)( \
        ::nyej::tuxx::detail::testing_context const& \
    ); \
    static ::nyej::tuxx::detail::test_case_adder TUXX_MAKE_TEST_CASE_NAME__( \
        tc_adder_ ## name \
    ){ \
        __FILE__, \
        __LINE__, \
        #name, \
        ::nyej::tuxx::detail::tests__(), \
        TUXX_MAKE_TEST_CASE_NAME__(name) \
    }; \
    static void TUXX_MAKE_TEST_CASE_NAME__(name)( \
        ::nyej::tuxx::detail::testing_context const& t_ctx__ \
    )

#define test_case_args_arr(name, args) \
    static void TUXX_MAKE_TEST_CASE_NAME__(name ## _args_arr)( \
        ::nyej::tuxx::detail::testing_context const&, \
        typename std::remove_reference< \
            typename std::remove_cv<decltype(args[0])>::type \
        >::type const& \
    ); \
    static ::nyej::tuxx::detail::test_case_multi_adder TUXX_MAKE_TEST_CASE_NAME__( \
        tc_adder_ ## name ## _args_arr \
    ){ \
        __FILE__, \
        __LINE__, \
        #name, \
        ::nyej::tuxx::detail::tests__(), \
        std::begin(args), \
        std::end(args), \
        TUXX_MAKE_TEST_CASE_NAME__(name ## _args_arr) \
    }; \
    static void TUXX_MAKE_TEST_CASE_NAME__(name ## _args_arr)( \
        ::nyej::tuxx::detail::testing_context const& t_ctx__, \
        typename std::remove_reference< \
            typename std::remove_cv<decltype(args[0])>::type \
        >::type const& arg \
    )

#define test_case_args(name, arg_type, ...) \
    static const arg_type TUXX_MAKE_TEST_CASE_NAME__(name ## _args_vals)[] = { \
        __VA_ARGS__ \
    }; \
    test_case_args_arr(name, TUXX_MAKE_TEST_CASE_NAME__(name ## _args_vals))

#define test_case_f(fix_type, name) \
    static void TUXX_MAKE_TEST_CASE_NAME__(fix_type ## _ ## name ## _f)( \
        ::nyej::tuxx::detail::testing_context const&, \
        fix_type& \
    ); \
    static ::nyej::tuxx::detail::test_case_adder TUXX_MAKE_TEST_CASE_NAME__( \
        tc_adder_ ## fix_type ## _ ## name ## _f \
    ){ \
        __FILE__, \
        __LINE__, \
        #fix_type "." #name, \
        ::nyej::tuxx::detail::tests__(), \
        [](::nyej::tuxx::detail::testing_context const& ctx) { \
            fix_type f{}; \
            TUXX_MAKE_TEST_CASE_NAME__(fix_type ## _ ## name ## _f)(ctx, f); \
        } \
    }; \
    static void TUXX_MAKE_TEST_CASE_NAME__(fix_type ## _ ## name ## _f)( \
        ::nyej::tuxx::detail::testing_context const& t_ctx__, \
        fix_type& fixture \
    )

#define test_case_args_arr_f(fix_type, name, args) \
    static void TUXX_MAKE_TEST_CASE_NAME__(fix_type ## _ ## name ## _args_ ## _f)( \
        ::nyej::tuxx::detail::testing_context const&, \
        fix_type&, \
        typename std::remove_reference< \
            typename std::remove_cv<decltype(args[0])>::type \
        >::type const& \
    ); \
    static ::nyej::tuxx::detail::test_case_multi_adder TUXX_MAKE_TEST_CASE_NAME__( \
        tc_adder_ ## fix_type ## _ ## name ## _args \
    ){ \
        __FILE__, \
        __LINE__, \
        #fix_type "." #name, \
        ::nyej::tuxx::detail::tests__(), \
        std::begin(args), \
        std::end(args), \
        []( \
            ::nyej::tuxx::detail::testing_context const& ctx, \
            typename std::remove_reference< \
                typename std::remove_cv<decltype(args[0])>::type \
            >::type const& p \
        ) { \
            fix_type f{}; \
            TUXX_MAKE_TEST_CASE_NAME__(fix_type ## _ ## name ## _args_ ## _f)(ctx, f, p); \
        } \
    }; \
    static void TUXX_MAKE_TEST_CASE_NAME__(fix_type ## _ ## name ## _args_ ## _f)( \
        ::nyej::tuxx::detail::testing_context const& t_ctx__, \
        fix_type& fixture, \
        typename std::remove_reference< \
            typename std::remove_cv<decltype(args[0])>::type \
        >::type const& arg \
    )

#define test_case_args_f(fix_type, name, arg_type, ...) \
    static const arg_type TUXX_MAKE_TEST_CASE_NAME__( \
        fix_type ## _ ## name ## _args_vals_ ## _f \
    )[] = {__VA_ARGS__}; \
    test_case_args_arr_f( \
        fix_type, \
        name, \
        TUXX_MAKE_TEST_CASE_NAME__(fix_type ## _ ## name ## _args_vals_ ## _f) \
    )

#define TUXX_CHECK_RESULT_NO_CALL_TC_ASSERT_MSG_STR__(result, detail, msg_str) \
    do { \
        if (!(result)) { \
            ::nyej::tuxx::ostringstream_type oss; \
            oss << detail; \
            if (!msg_str.empty()) { \
                oss << " | " << msg_str; \
            } \
            throw (::nyej::tuxx::test_case_failure_error{__FILE__, __LINE__, oss.str()}); \
        } \
    } while (false)

#define TUXX_CHECK_RESULT__(name, result, expr_txt, detail, msg) \
    do { \
        ::nyej::tuxx::ostringstream_type oss; \
        oss << msg; \
        auto const msg_str = oss.str(); \
        { \
            std::unique_lock<std::mutex> _(*t_ctx__.mtx); \
            t_ctx__.reporter->test_case_assert( \
                *t_ctx__.tc, \
                __FILE__, \
                __LINE__, \
                name, \
                expr_txt, \
                msg_str \
            ); \
        } \
        TUXX_CHECK_RESULT_NO_CALL_TC_ASSERT_MSG_STR__(result, detail, msg_str); \
    } while (false)

#define TUXX_CHECK_BIN_OP_RESULT__(name, a, b, msg, op, txt) \
    TUXX_CHECK_RESULT__( \
        name, \
        (a) op (b), \
        #a " " #op " " #b, \
        "expected '" \
            << #a << "' (" << (a) << ") to be " \
            << txt << " to '" \
            << #b << "' (" << (b) << ")", \
        msg \
    )

#define assert_true_msg(expr, msg) \
    TUXX_CHECK_RESULT__( \
        "assert_true", \
        expr, \
        #expr " is truthy", \
        "expected '" << #expr << "' (" << (expr) << ") to evaluate to true", \
        msg \
    )

#define assert_true(expr) \
    assert_true_msg(expr, "")

#define assert_false_msg(expr, msg) \
    TUXX_CHECK_RESULT__( \
        "assert_false", \
        !(expr), \
        #expr " is falsey", \
        "expected '" << #expr << "' (" << (expr) << ") to evaluate to false", \
        msg \
    )

#define assert_false(expr) \
    assert_false_msg(expr, "")

#define assert_null_msg(expr, msg) \
    TUXX_CHECK_RESULT__( \
        "assert_null", \
        (expr) == nullptr, \
        #expr " is null", \
        "expected '" << #expr << "' (" << (expr) << ") to be null", \
        msg \
    )

#define assert_null(expr) \
    assert_null_msg(expr, "")

#define assert_not_null_msg(expr, msg) \
    TUXX_CHECK_RESULT__( \
        "assert_not_null", \
        (expr) != nullptr, \
        #expr " is not null", \
        "expected '" << #expr << "' (" << static_cast<void const*>(expr) << ") to be non-null", \
        msg \
    )

#define assert_not_null(expr) \
    assert_not_null_msg(expr, "")

#define assert_eq_msg(a, b, msg) \
    TUXX_CHECK_BIN_OP_RESULT__("assert_eq", a, b, msg, ==, "equal")

#define assert_eq(a, b) \
    assert_eq_msg(a, b, "")

#define assert_ne_msg(a, b, msg) \
    TUXX_CHECK_BIN_OP_RESULT__("assert_ne", a, b, msg, !=, "not equal")

#define assert_ne(a, b) \
    assert_ne_msg(a, b, "")

#define assert_lt_msg(a, b, msg) \
    TUXX_CHECK_BIN_OP_RESULT__("assert_lt", a, b, msg, <, "less than")

#define assert_lt(a, b) \
    assert_lt_msg(a, b, "")

#define assert_le_msg(a, b, msg) \
    TUXX_CHECK_BIN_OP_RESULT__("assert_le", a, b, msg, <=, "less or equal")

#define assert_le(a, b) \
    assert_le_msg(a, b, "")

#define assert_gt_msg(a, b, msg) \
    TUXX_CHECK_BIN_OP_RESULT__("assert_gt", a, b, msg, >, "greater than")

#define assert_gt(a, b) \
    assert_gt_msg(a, b, "")

#define assert_ge_msg(a, b, msg) \
    TUXX_CHECK_BIN_OP_RESULT__("assert_ge", a, b, msg, >=, "greater than or equal")

#define assert_ge(a, b) \
    assert_ge_msg(a, b, "")

#define assert_close_msg(a, b, tol, msg) \
    TUXX_CHECK_RESULT__( \
        "assert_close", \
        (((a) > (b)) ? ((a) - (b)) : ((b) - (a))) <= (tol), \
        #a " ~= " #b " +/- " #tol, \
        "expected '" \
            << #a << "' (" << (a) << ") and '" \
            << #b << "' (" << (b) << ") differ by no more than '" \
            << #tol << "' (" << (tol) << ")", \
        msg \
    )

#define assert_close(a, b, tol) \
    assert_close_msg(a, b, tol, "")

#define assert_throws_msg(expr, ex, msg) \
    do { \
        ::nyej::tuxx::ostringstream_type oss; \
        oss << msg; \
        auto const msg_str = oss.str(); \
        { \
            std::unique_lock<std::mutex> _(*t_ctx__.mtx); \
            t_ctx__.reporter->test_case_assert( \
                *t_ctx__.tc, \
                __FILE__, \
                __LINE__, \
                "assert_throws", \
                #expr " throws " #ex, \
                msg_str, \
                #ex \
            ); \
        } \
        try { \
            expr; \
            TUXX_CHECK_RESULT_NO_CALL_TC_ASSERT_MSG_STR__( \
                false, \
                "expected '" \
                    << #expr << "' to throw an exception of type '" \
                    << #ex << "' but no exception was thrown", \
                msg_str \
            ); \
        } catch (ex const&) { \
            /* Pass */ \
        } catch (std::exception const& e) { \
            TUXX_CHECK_RESULT_NO_CALL_TC_ASSERT_MSG_STR__( \
                false, \
                "expected '" \
                    << #expr << "' to throw an exception of type '" \
                    << #ex << "' but instead got an exception '" << e.what() << "'", \
                msg_str \
            ); \
        } catch (...) { \
            TUXX_CHECK_RESULT_NO_CALL_TC_ASSERT_MSG_STR__( \
                false, \
                "expected '" \
                    << #expr << "' to throw an exception of type '" \
                    << #ex << "' but instead got an unrecognized exception", \
                msg_str \
            ); \
        } \
    } while (false)

#define assert_throws(expr, ex) \
    assert_throws_msg(expr, ex, "")

#define assert_ieq_msg_impl__(str_a, str_b, msg) \
    do { \
        ::nyej::tuxx::ostringstream_type oss; \
        oss << msg; \
        auto const msg_str = oss.str(); \
        { \
            std::unique_lock<std::mutex> _(*t_ctx__.mtx); \
            t_ctx__.reporter->test_case_assert( \
                *t_ctx__.tc, \
                __FILE__, \
                __LINE__, \
                "assert_ieq", \
                #str_a " ^v= " #str_b, \
                msg_str \
            ); \
        } \
        TUXX_CHECK_RESULT_NO_CALL_TC_ASSERT_MSG_STR__( \
            ::nyej::tuxx::detail::iequals(str_a, str_b), \
            "expected '" << (str_a) << "' to equal '" << (str_b) << "', ignoring case", \
            msg_str \
        ); \
    } while (false)

#define assert_ieq_msg(str_a, str_b, msg) \
    assert_ieq_msg_impl__(str_a, str_b, msg)

#define assert_ieq(str_a, str_b) \
    assert_ieq_msg_impl__(str_a, str_b, "")

#define test_fail(msg) \
    TUXX_CHECK_RESULT__("test_fail", false, "fail", "test forcibly failed", msg)

#if defined(TUXX_DEFINE_TEST_MAIN) || defined(TUXX_UNDER_TEST)
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <iomanip>
#include <limits>
#include <random>
#include <thread>

#if defined(TUXX_IS_UNIX)
#include <unistd.h>
#endif

#if defined(TUXX_UNDER_TEST)
    // Give the normally-anonymous namespace an actual name when unit testing tuxx.
    #define TUXX_TST_NS tst
    #define TUXX_TST_NS_PREFIX TUXX_TST_NS::
#else
    #define TUXX_TST_NS
    #define TUXX_TST_NS_PREFIX
#endif

#if !defined(TUXX_COLOR_SQ_BR)
    #define TUXX_COLOR_SQ_BR TUXX_TST_NS_PREFIX color::fg_gray
#endif
#if !defined(TUXX_COLOR_TEST_ID)
    #define TUXX_COLOR_TEST_ID TUXX_TST_NS_PREFIX color::fg_cyan
#endif
#if !defined(TUXX_COLOR_TEST_NAME)
    #define TUXX_COLOR_TEST_NAME TUXX_TST_NS_PREFIX color::fg_yellow
#endif
#if !defined(TUXX_COLOR_PAREN)
    #define TUXX_COLOR_PAREN TUXX_TST_NS_PREFIX color::fg_blue
#endif
#if !defined(TUXX_COLOR_PARAM)
    #define TUXX_COLOR_PARAM TUXX_TST_NS_PREFIX color::fg_magenta
#endif
#if !defined(TUXX_COLOR_PUNCT)
    #define TUXX_COLOR_PUNCT TUXX_TST_NS_PREFIX color::fg_blue
#endif
#if !defined(TUXX_COLOR_DURATION)
    #define TUXX_COLOR_DURATION TUXX_TST_NS_PREFIX color::fg_white
#endif
#if !defined(TUXX_COLOR_SUCCESS)
    #define TUXX_COLOR_SUCCESS TUXX_TST_NS_PREFIX color::fg_green
#endif
#if !defined(TUXX_COLOR_FAIL)
    #define TUXX_COLOR_FAIL TUXX_TST_NS_PREFIX color::fg_red
#endif
#if !defined(TUXX_COLOR_FILE_REF)
    #define TUXX_COLOR_FILE_REF TUXX_TST_NS_PREFIX color::fg_gray
#endif

#if defined(TUXX_DEFINE_CUSTOM_REPORTER)
::nyej::tuxx::test_case_reporter* tuxx_make_custom_reporter(
    ::nyej::tuxx::test_case_reporter_args const&
);
#endif

namespace nyej {
namespace tuxx {
namespace TUXX_TST_NS {

enum class color : unsigned {
    fg_black = 30,
    fg_red,
    fg_green,
    fg_yellow,
    fg_blue,
    fg_magenta,
    fg_cyan,
    fg_white,
    fg_gray = 90,
};

template <typename T>
report_ostream_type& write_with_color(
    report_ostream_type& os,
    T const& value,
    color c,
    bool color_enabled
) {
    if (color_enabled) {
        return os << "\033[" << static_cast<unsigned>(c) << 'm' << value << "\033[0m";
    }
    return os << value;
}

report_ostream_type& write_idx(
    std::size_t idx,
    std::size_t total_count,
    report_ostream_type& os,
    bool color_enabled
) {
    using namespace std;
    ++idx; // Using 1-based indexing.
    ++total_count;
    std::size_t const field_width = (total_count < 10) ? 1 :
        (total_count < 100) ? 2 :
        (total_count < 1000) ? 3 :
        (total_count < 10000) ? 4 :
        (total_count < 100000) ? 5 :
        (total_count < 1000000) ? 6 :
        (total_count < 10000000) ? 7 : 10;
    write_with_color(os, "[", TUXX_COLOR_SQ_BR, color_enabled);
    ostringstream_type oss;
    oss << setw(field_width) << idx;
    write_with_color(os, oss.str(), TUXX_COLOR_TEST_ID, color_enabled);
    return write_with_color(os, "]", TUXX_COLOR_SQ_BR, color_enabled);
}

report_ostream_type& write_test_case_name(
    report_ostream_type& os,
    test_case_instance const& tc,
    bool color_enabled,
    std::size_t width = 0
) {
    using namespace std;
    ostringstream_type oss;
    write_with_color(oss, tc.name, TUXX_COLOR_TEST_NAME, color_enabled);
    if (!tc.arg.empty()) {
        write_with_color(oss, "(", TUXX_COLOR_PAREN, color_enabled);
        write_with_color(oss, tc.arg, TUXX_COLOR_PARAM, color_enabled);
        write_with_color(oss, ")", TUXX_COLOR_PAREN, color_enabled);
    }
    auto const str = oss.str();
    if (width) {
        if (color_enabled) {
            // Need to account for color escape sequences
            width += str.length()
                - tc.name.length()
                - (tc.arg.empty() ? 0 : tc.arg.length() + 2);
        }
        os << setw(width) << setfill(TUXX_STR_LIT(' ')) << left;
    }
    return os << str;
}

report_ostream_type& write_test_case_line_prefix(
    test_case_instance const& tc,
    report_ostream_type& os,
    size_t max_test_case_id,
    bool colorized,
    bool use_emojis,
    size_t width = 0
) {
    write_idx(tc.idx, max_test_case_id, os, colorized) << ' ';
    if (use_emojis) {
        os << TUXX_EMOJI_PREFIX << ' ';
    }
    return write_test_case_name(os, tc, colorized, width) << std::flush;
}

template <typename Duration>
report_ostream_type& write_duration(
    report_ostream_type& os,
    Duration const& d,
    bool color_enabled,
    bool include_brackets = true
) {
    using namespace std;
    using namespace std::chrono;
    if (include_brackets) {
        write_with_color(os, "[", TUXX_COLOR_SQ_BR, color_enabled);
    }
    static auto constexpr fill_ch = TUXX_STR_LIT('0');
    static auto constexpr col = ":";
    static size_t const MS_IN_SEC = 1000;
    static size_t const MS_IN_MIN = MS_IN_SEC * 60;
    static size_t const MS_IN_HOUR = MS_IN_MIN * 60;
    auto ms = duration_cast<milliseconds>(d).count();
    auto const h = ms / MS_IN_HOUR;
    ms -= h * MS_IN_HOUR;
    auto const m = ms / MS_IN_MIN;
    ms -= m * MS_IN_MIN;
    auto const s = ms / MS_IN_SEC;
    ms -= s * MS_IN_SEC;
    ostringstream_type oss;
    oss << setw(2) << setfill(fill_ch) << h;
    write_with_color(os, oss.str(), TUXX_COLOR_DURATION, color_enabled);
    write_with_color(os, col, TUXX_COLOR_PUNCT, color_enabled);
    oss.str(TUXX_STR_LIT(""));
    oss << setw(2) << setfill(fill_ch) << m;
    write_with_color(os, oss.str(), TUXX_COLOR_DURATION, color_enabled);
    write_with_color(os, col, TUXX_COLOR_PUNCT, color_enabled);
    oss.str(TUXX_STR_LIT(""));
    oss << setw(2) << setfill(fill_ch) << s;
    write_with_color(os, oss.str(), TUXX_COLOR_DURATION, color_enabled);
    write_with_color(os, ".", TUXX_COLOR_PUNCT, color_enabled);
    oss.str(TUXX_STR_LIT(""));
    oss << setw(3) << setfill(fill_ch) << ms;
    write_with_color(os, oss.str(), TUXX_COLOR_DURATION, color_enabled);
    if (include_brackets) {
        write_with_color(os, "]", TUXX_COLOR_SQ_BR, color_enabled);
    }
    return os;
}

std::string get_time_stamp(std::chrono::system_clock::time_point const& pt) {
    using namespace std;
    using namespace std::chrono;
    auto const curr_time_t = system_clock::to_time_t(pt);
    struct tm curr_tm{};
    localtime_r(&curr_time_t, &curr_tm);
    ostringstream oss;
    oss << put_time(&curr_tm, "%FT%T%z");
    return oss.str();
}

struct test_case_reporter_plain_text : test_case_reporter {
    report_ostream_type& os;
    std::size_t max_test_case_id{};
    std::size_t max_test_case_name_len{};
    std::size_t n_total_tests;
    bool const do_write_color;
    std::size_t const concurrency;
    bool use_emojis;
    std::size_t n_total_asserts{};
    test_case_reporter_plain_text(
        report_ostream_type& o,
        std::vector<test_case_instance> const& tests,
        bool write_col,
        std::size_t n_concur,
        bool use_emos
    ) :
        os{o},
        n_total_tests{tests.size()},
        do_write_color{write_col},
        concurrency{n_concur},
        use_emojis{use_emos}
    {
        for (auto const& tc : tests) {
            if (tc.idx > max_test_case_id) {
                max_test_case_id = tc.idx;
            }
            auto len = tc.name.length();
            if (!tc.arg.empty()) {
                len += tc.arg.length() + 2;
            }
            if (len > max_test_case_name_len) {
                max_test_case_name_len = len;
            }
        }
    }
    void start() override {
        os << '\n';
    }
    void test_case_starting(test_case_instance const& tc) override {
        if (concurrency == 1) {
            write_test_case_line_prefix(
                tc,
                os,
                max_test_case_id,
                do_write_color,
                use_emojis,
                max_test_case_name_len
            );
        }
    }
    void test_case_assert(
        test_case_instance const&,
        char const*,
        int,
        char const*,
        char const*,
        string_type const&,
        char const*
    ) override {
        ++n_total_asserts;
    }
    void test_case_passed(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        if (concurrency > 1) {
            write_test_case_line_prefix(
                tc,
                os,
                max_test_case_id,
                do_write_color,
                use_emojis,
                max_test_case_name_len
            );
        }
        os << ' ';
        write_passed(os);
        os << ' ';
        write_duration(os, elapsed, do_write_color) << '\n';
    }
    void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        using namespace std;
        if (concurrency > 1) {
            write_test_case_line_prefix(
                tc,
                os,
                max_test_case_id,
                do_write_color,
                use_emojis,
                max_test_case_name_len
            );
        }
        os << ' ';
        write_failed(os);
        os << ' ';
        write_duration(os, elapsed, do_write_color) << '\n';
        ostringstream_type oss;
        oss << err.file << ':' << err.line;
        write_with_color(os << '\t', oss.str(), TUXX_COLOR_FILE_REF, do_write_color);
        write_with_color(os << '\t', err.detail, TUXX_COLOR_FAIL, do_write_color)
            << '\n';
    }
    void end_test_cases(
        std::chrono::steady_clock::duration const& elapsed,
        std::vector<test_case_instance> const& failures
    ) override {
        using namespace std;
        auto const width = 12u;
        auto const n_passed = n_total_tests - failures.size();
        os << "\nCONCURRENCY:          " << setw(width) << setfill(TUXX_STR_LIT(' ')) << right
            << concurrency
           << "\nTOTAL TEST CASES RUN: " << setw(width) << setfill(TUXX_STR_LIT(' ')) << right
            << n_total_tests
           << "\nTOTAL ASSERTS:        " << setw(width) << setfill(TUXX_STR_LIT(' ')) << right
            << n_total_asserts
           << "\nELAPSED:              ";
        write_duration(os, elapsed, do_write_color, false) << '\n';
        write_with_color(os, "PASSED", TUXX_COLOR_SUCCESS, do_write_color)
            << ":               " << setw(width) << setfill(TUXX_STR_LIT(' '))
            << n_passed << '\n';
        if (failures.empty()) {
            os << '\n';
            write_passed(os, TUXX_EMOJI_SUCCESS, "SUCCESS");
        } else {
            write_with_color(os, "FAILED", TUXX_COLOR_FAIL, do_write_color)
                << ":               " << setw(width) << setfill(TUXX_STR_LIT(' '))
                << failures.size() << '\n';
            write_with_color(os << '\n', "FAILURES", TUXX_COLOR_FAIL, do_write_color)
                << ":\n";
            for (auto const& tc : failures) {
                os << '\t';
                write_test_case_line_prefix(
                    tc,
                    os,
                    max_test_case_id,
                    do_write_color,
                    use_emojis,
                    max_test_case_name_len
                );
                if (use_emojis) {
                    os << ' ';
                    write_failed(os);
                }
                os << '\n';
            }
            os << '\n';
            write_failed(os, TUXX_EMOJI_FAILURE, "FAILURE");
        }
        os << "\n\n";
    }
    void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) override {
        if (!stdout_data.empty()) {
            os << "\nSTDOUT TEXT:\n" << stdout_data.c_str() << "\n";
        }
        if (!stderr_data.empty()) {
            os << "\nSTDERR TEXT:\n" << stderr_data.c_str() << "\n";
        }
    }

private:
    report_ostream_type& write_passed(
        report_ostream_type& os,
        char_type const* pass_emoji = TUXX_EMOJI_PASSED,
        char const* pass_txt = "PASSED"
    ) {
        if (use_emojis) {
            os << pass_emoji;
        } else {
            write_with_color(os, pass_txt, TUXX_COLOR_SUCCESS, do_write_color);
        }
        return os;
    }
    report_ostream_type& write_failed(
        report_ostream_type& os,
        char_type const* fail_emoji = TUXX_EMOJI_FAILED,
        char const* fail_txt = "FAILED"
    ) {
        if (use_emojis) {
            os << fail_emoji;
        } else {
            write_with_color(os, fail_txt, TUXX_COLOR_FAIL, do_write_color);
        }
        return os;
    }
};

struct test_case_reporter_delim : test_case_reporter {
    report_ostream_type& os;
    char_type const delim;
    test_case_reporter_delim(
        report_ostream_type& o,
        char_type d
    ) : os{o}, delim{d} {}
    void start() override {
        os << "ID" << delim
            << "NAME" << delim
            << "ARG" << delim
            << "STATUS" << delim
            << "ELAPSED" << delim
            << "DETAIL\n";
    }
    void test_case_passed(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        write_test_case_line(tc, elapsed);
    }
    void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        write_test_case_line(tc, elapsed, &err);
    }
    void end_test_cases(
        std::chrono::steady_clock::duration const& elapsed,
        std::vector<test_case_instance> const& failures
    ) override {
    }
    void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) override {
        if (!stdout_data.empty()) {
            os << "\nSTDOUT TEXT:\n" << stdout_data.c_str() << "\n";
        }
        if (!stderr_data.empty()) {
            os << "\nSTDERR TEXT:\n" << stderr_data.c_str() << "\n";
        }
    }
private:
    void write_test_case_line(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed,
        test_case_failure_error const* opt_err = nullptr
    ) {
        os << tc.idx << delim << tc.name << delim;
        if (!tc.arg.empty()) {
            write_delim_safe(os, tc.arg);
        }
        os << delim << (opt_err ? "FAIL" : "PASS") << delim;
        write_duration(os, elapsed, false, false) << delim;
        if (opt_err) {
            write_delim_safe(os, detail::to_char_type(opt_err->file));
            os << ':' << opt_err->line << " - ";
            write_delim_safe(os, opt_err->detail);
        }
        os << '\n';
    }
    void write_delim_safe(
        report_ostream_type& os,
        string_type const& s
    ) {
        for (auto const c : s) {
            if (c == delim) {
                os << "<<delim>>";
            } else {
                os << c;
            }
        }
    }
};

struct test_case_reporter_json : test_case_reporter {
    report_ostream_type& os;
    std::size_t n_total_tests;
    std::size_t concurrency;
    std::size_t n_total_asserts{};
    bool is_first{true};
    std::chrono::steady_clock::duration total_elapsed;
    std::vector<test_case_instance> tc_failures;
    test_case_reporter_json(
        report_ostream_type& o,
        std::size_t n_tests,
        std::size_t n_concur
    ) : os{o}, n_total_tests{n_tests}, concurrency{n_concur} {}
    void start() override {
        char host_name[1024];
        gethostname(host_name, sizeof(host_name));
        os << "{\"host-name\":";
        escape(os, host_name) << ",\"time-stamp\":\""
            << get_time_stamp(std::chrono::system_clock::now()).c_str() << "\","
            << "\"concurrency\":" << concurrency << ','
            << "\"n-total\":" << n_total_tests << ','
            << "\"test-cases\":[";
    }
    void test_case_assert(
        test_case_instance const&,
        char const*,
        int,
        char const*,
        char const*,
        string_type const&,
        char const*
    ) override {
        ++n_total_asserts;
    }
    void test_case_passed(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        write_test_case_name(tc)
            << ",\"file-name\":";
        escape(os, tc.file)
            << ",\"line\":" << tc.line
            << ",\"elapsed\":\"";
        write_duration(os, elapsed, false, false)
            << "\",\"passed\":true}";
    }
    void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        write_test_case_name(tc);
        os << ",\"elapsed\":\"";
        write_duration(os, elapsed, false, false) << "\",";
        os << "\"error\":{";
        os << "\"file\":";
        escape(os, err.file) << ',';
        os << "\"line\":" << err.line << ',';
        os << "\"detail\":";
        escape(os, err.detail);
        os << "},";
        os << "\"passed\":false}";
    }
    void end_test_cases(
        std::chrono::steady_clock::duration const& elapsed,
        std::vector<test_case_instance> const& failures
    ) override {
        total_elapsed = elapsed;
        tc_failures = failures;
    }
    void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) override {
        auto const n_passed = n_total_tests - tc_failures.size();
        os << "],"
           << "\"n-total-asserts\":" << n_total_asserts << ','
           << "\"n-passed\":" << n_passed << ','
           << "\"n-failed\":" << tc_failures.size() << ','
           << "\"elapsed\":\"";
        write_duration(os, total_elapsed, false, false);
        os << "\",\"passed\":"
            << (tc_failures.empty() ? "true" : "false") << ','
            << "\"std-out\":";
        escape(os, stdout_data) << ','
            << "\"std-err\":";
        escape(os, stderr_data) << '}';
    }
private:
    report_ostream_type& write_test_case_name(test_case_instance const& tc) {
        if (!is_first) {
            os << ',';
        }
        is_first = false;
        os << "{\"name\":";
        escape(os, tc.name);
        if (!tc.arg.empty()) {
            os << ",\"arg\":";
            escape(os, tc.arg);
        }
        return os;
    }

    static report_ostream_type& escape(
        report_ostream_type& os,
        std::basic_string<char_type> const& s
    ) {
        using namespace std;
        os << '"';
        for (auto const c : s) {
            switch (c) {
                case '"': os << "\\\""; break;
                case '\\': os << "\\\\"; break;
                case '\a': os << "\\a"; break;
                case '\b': os << "\\b"; break;
                case '\f': os << "\\f"; break;
                case '\n': os << "\\n"; break;
                case '\r': os << "\\r"; break;
                case '\t': os << "\\t"; break;
                case '\v': os << "\\v"; break;
                default: {
                    if (c < 32) {
                        os << "\\u" << hex << setw(4) << setfill(TUXX_STR_LIT('0'))
                        << static_cast<unsigned>(c & 0xff)
                        << dec;
                    } else {
                        os << c;
                    }
                }
            }
        }
        return os << '"';
    }

    template <typename S>
    static report_ostream_type& escape(
        report_ostream_type& os,
        S const& s
    ) {
        ostringstream_type oss;
        oss << s;
        return escape(os, oss.str());
    }
};

struct test_case_reporter_junit : test_case_reporter {
    struct test_case_result {
        char const* file{};
        int line{};
        string_type full_name;
        bool passed{};
        std::size_t n_asserts{};
        std::chrono::steady_clock::duration elapsed{};
        string_type error;
    };
    report_ostream_type& os;
    string_type test_suite_name;
    string_type test_report_name;
    std::size_t n_total_tests;
    std::chrono::system_clock::time_point start_time;
    size_t n_failures{};
    std::vector<test_case_result> results;
    std::size_t n_asserts{};
    std::size_t n_total_asserts{};
    std::chrono::steady_clock::duration total_elapsed;
    test_case_reporter_junit(
        report_ostream_type& o,
        string_type prog_name,
        std::size_t n_tests,
        string_type const& test_report_nm = string_type{}
    ) :
        os{o},
        test_suite_name{cleanup_name(std::move(prog_name))},
        test_report_name{cleanup_name(test_report_nm)},
        n_total_tests{n_tests}
    {
        results.reserve(n_tests);
    }
    void start() override {
        start_time = std::chrono::system_clock::now();
    }
    void test_case_assert(
        test_case_instance const&,
        char const*,
        int,
        char const*,
        char const*,
        string_type const&,
        char const*
    ) override {
        ++n_total_asserts;
        ++n_asserts;
    }
    void test_case_passed(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        test_case_result tc_res;
        tc_res.file = tc.file;
        tc_res.line = tc.line;
        tc_res.full_name = get_test_case_name(tc);
        tc_res.passed = true;
        tc_res.n_asserts = n_asserts;
        tc_res.elapsed = elapsed;
        results.push_back(tc_res);
        n_asserts = 0;
    }
    void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        std::chrono::steady_clock::duration const& elapsed
    ) override {
        ostringstream_type oss;
        oss << err.file << ':' << err.line << ": " << err.detail;
        test_case_result tc_res;
        tc_res.file = tc.file;
        tc_res.line = tc.line;
        tc_res.full_name = get_test_case_name(tc);
        tc_res.passed = false;
        tc_res.n_asserts = n_asserts;
        tc_res.elapsed = elapsed;
        tc_res.error = oss.str();
        results.push_back(tc_res);
        ++n_failures;
        n_asserts = 0;
    }
    void end_test_cases(
        std::chrono::steady_clock::duration const& elapsed,
        std::vector<test_case_instance> const&
    ) override {
        total_elapsed = elapsed;
    }
    void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) override {
        using namespace std;
        using namespace std::chrono;
        auto indent = 0u;
        if (!test_report_name.empty()) {
            char host_name[1024];
            gethostname(host_name, sizeof(host_name));
            os << "<?xml version='1.0' encoding='utf-8'>\n"
               << "<testsuites name='";
            escape(os, test_report_name) << "' hostname='";
            escape(os, host_name) << "' ";
            add_testsuite_details(os, total_elapsed) << ">\n";
            ++indent;
        }
        do_indent(os, indent) << "<testsuite name='";
        escape(os, test_suite_name) << "' ";
        add_testsuite_details(os, total_elapsed) << ">\n";
        ++indent;
        do_indent(os, indent) << "<system-out>";
        escape(os, stdout_data) << "</system_out>\n";
        do_indent(os, indent) << "<system-err>";
        escape(os, stderr_data) << "</system_err>\n";

        for (auto const& tcr : results) {
            do_indent(os, indent) << "<testcase name='";
            escape(os, tcr.full_name) << "' classname='";
            escape(os, test_suite_name) << "' assertions='" << tcr.n_asserts << "' file='";
            escape(os, tcr.file) << "' line='" << tcr.line << "' "
                << "time='" << duration<double>(tcr.elapsed).count() << "' ";
            if (tcr.passed) {
                os << "/>\n";
            } else {
                os << ">\n";
                ++indent;
                do_indent(os, indent) << "<failure message='";
                escape(os, tcr.error) << "' type='AssertionError' />\n";
                --indent;
                do_indent(os, indent) << "</testcase>\n";
            }
        }
        --indent;
        do_indent(os, indent) << "</testsuite>\n";
        if (!test_report_name.empty()) {
            os << "</testsuites>\n";
        }
    }
private:
    report_ostream_type& do_indent(
        report_ostream_type& os,
        unsigned n
    ) {
        for (auto i = 0u; i < n; ++i) {
            os << '\t';
        }
        return os;
    }
    report_ostream_type& add_testsuite_details(
        report_ostream_type& os,
        std::chrono::steady_clock::duration const& elapsed
    ) {
        using namespace std;
        using namespace std::chrono;
        return os << "tests='" << n_total_tests << "' "
            << "failures='" << n_failures << "' "
            << "errors='0' "
            << "skipped='0' "
            << "timestamp='" << get_time_stamp(start_time).c_str() << '\''
            << "time='" << duration<double>{elapsed}.count() << "' "
            << "assertions='" << n_total_asserts << "' ";
    }
    static report_ostream_type& escape(
        report_ostream_type& os,
        std::basic_string<char_type> const& s
    ) {
        using namespace std;
        for (auto const c : s) {
            switch (c) {
                case '&': os << "&amp;"; break;
                case '\'': os << "&apos;"; break;
                case '>': os << "&gt;"; break;
                case '<': os << "&lt;"; break;
                case '"': os << "&quot;"; break;
                default: {
                    os << c;
                }
            }
        }
        return os;
    }
    template <typename S>
    static report_ostream_type& escape(
        report_ostream_type& os,
        S const& s
    ) {
        ostringstream_type oss;
        oss << s;
        return escape(os, oss.str());
    }
    static string_type get_test_case_name(test_case_instance const& tc) {
        string_type ret{tc.name};
        if (!tc.arg.empty()) {
            ret += TUXX_STR_LIT('(') + tc.arg + TUXX_STR_LIT(')');
        }
        return ret;
    }
    static string_type cleanup_name(string_type name) {
        static char_type const bad_chars[] = TUXX_STR_LIT("./\\- ");
        auto const idx_first_good = name.find_first_not_of(bad_chars);
        name.erase(0, idx_first_good);
        auto const idx_last_good = name.find_last_not_of(bad_chars);
        if (idx_last_good != string_type::npos) {
            name.erase(idx_last_good + 1);
        }
        for (auto& c : name) {
            auto was_found = false;
            for (auto p = bad_chars; *p; ++p) {
                if (*p == c) {
                    was_found = true;
                    break;
                }
            }
            if (was_found) {
                c = '_';
            }
        }
        return name;
    }
};

void usage(
    string_type const& prog_name,
    report_ostream_type& os
) {
    using namespace std;
    auto const prog_str = prog_name.c_str();
    os << prog_str << " unit tests" << "\n\n"
        "usage: " << prog_str <<
        "[-h|--help] "
        "[-l|--list] "
        "[-e|--emojis] "
        "[-m|--match <list>] "
        "[-n|--numbers <list>] "
        "[-p|--parallel <n> "
        "[--no-color] "
        "[-j|--json] "
        "[--junit]"
        "[--junit-full]"
        "[-d|--delim] <delim> "
        "[-r|--randomized] "
        "[-t|--plain]"
        "[-o|--redirect]"
        "\n\n"
        "options:\n"
        "-h|--help                      Print this message and exit.\n"
        "\n"
        "-l|--list                      List all test cases and exit.\n"
        "\n"
        "-e|--emojis                    Enable emojis for results with text reporter. Only applies\n"
        "                               to basic text reporter.\n"
        "\n"
        "-m|--match <list>              Run test cases that start with the substrings specified\n"
        "                               in the comma-separaged list. Matches are case-insensitve.\n\n"
        "                               Example: " << prog_str << " -m test_suite_x,test_suite_y\n"
        "\n"
        "-n|--number <list>             Run test cases numbered according to the list provided. The\n"
        "                               numbers will have come from a previous call to --list. Each\n"
        "                               element of list can be a single number or a range specified\n"
        "                               as -# to indicate all tests up to the number should be run\n"
        "                               (exclusive), #1-#2 to indicate tests #1 through to #2\n"
        "                               (exclusive) should be run or #- to indicate that tests #\n"
        "                               through the end should be run. The program will ensure tests\n"
        "                               are only run once regardless of how many times they may match\n"
        "                               the expressions in the list.\n"
        "                               Example: " << prog_str << " -n 1,4-8,3\n\n" <<
        "\n"
        "-p|--parallel <n>              Run test cases with the specified parallellism. This is not\n"
        "                               recommended for unit testing code that contains shared mutable\n"
        "                               state\n"
        "\n"
        "--no-color                     Print output without colorization\n"
        "\n"
        "-j|--json                      Produce json output (implies --no-color)\n"
        "\n"
        "--junit [<report-name>]        Produce a JUnit-style report. If <report-name> is provided,\n"
        "                               this will produce a full report with the <?xml ?> header and\n"
        "                               and the <testsuites> top-level tag. If omitted, this will produce\n"
        "                               a test suite report starting with the <testsuite> tag. The latter\n"
        "                               is intended to be used in the context of a a higher-level script\n"
        "                               that runs through a directory hierarchy of tests and produce the\n"
        "                               full report itself\n"
        "\n"
        "-d|--delim <delim>             Produces a table of output using the specified column delimiter.\n"
        "                               The delimiter can be any printable single character or '\\t'\n"
        "\n"
        "-r|--randomized                Runs tests in randomized order. Ignored for -l|--list.\n"
        "-t|--plain                     Chooses the default plain text reporter even if a custom reporter\n"
        "                               is available\n"
        "-c|--capture                   Capture stdout/stderr even when using default reporter\n"
        "\n"
        "Short form arguments except 'm', 'n', 'd', and 'p' (if a value is provided) can be combined. For\n"
        "example, '-epr' would enable emojis, run test cases in parallel and random shuffle the them."
       << endl;
}

void to_lower(string_type& s) {
    for (auto& c : s) {
        c = std::tolower(c);
    }
}

// Needs to be a template because it can be called when Ch differs from char_type.
template <typename Ch>
std::vector<std::basic_string<Ch>> split_delim(
    std::basic_string<Ch> const& s,
    char delim,
    bool include_empty = false
) {
    using namespace std;
    vector<basic_string<Ch>> ret;
    size_t idx_begin_of_item{};
    for (
        auto idx_delim = s.find(delim);
        idx_delim != basic_string<Ch>::npos;
        idx_begin_of_item = idx_delim + 1, idx_delim = s.find(delim, idx_begin_of_item)
    ) {
        auto const len = idx_delim - idx_begin_of_item;
        if (len || include_empty) {
            ret.push_back(basic_string<Ch>{s.data() + idx_begin_of_item, len});
        }
    }
    if (idx_begin_of_item < s.length()) {
        ret.push_back(
            basic_string<Ch>{
                s.data() + idx_begin_of_item,
                s.length() - idx_begin_of_item
            }
        );
    } else if (include_empty && (idx_begin_of_item == s.length())) {
        ret.push_back(basic_string<Ch>{});
    }
    return ret;
}

// If zero is returned, the resulting delimiter is empty or longer than one character.
char_type parse_delim(string_type const& s) {
    using namespace std;
    if (s.length() == 1) {
        return static_cast<char_type>(s[0]);
    }
    if (s.length() == 2) {
        if ((s[0] == '\\') && (s[1] == 't')) {
            return TUXX_STR_LIT('\t');
        }
    }
    return 0;
}

std::pair<std::size_t, std::size_t> read_number_range(string_type const& num_pair) {
    using namespace std;

    basic_istringstream<char_type> iss{num_pair};
    size_t idx_low = 1;

    auto ch = iss.peek();
    if (ch == '-') {
        iss.get();
        auto idx_high_excl = numeric_limits<std::size_t>::max();
        if (!(iss >> idx_high_excl)) {
            throw runtime_error{
                "Invalid test number match expression for -n|--number: '"
                    + detail::to_char(num_pair) + "'"
            };
        }
        return make_pair(idx_low, idx_high_excl);
    }

    iss >> idx_low;

    if (iss.eof() && iss.fail()) {
        throw runtime_error{
            "Invalid test number match expression for -n|--number: '"
                + detail::to_char(num_pair) + "'"
        };
    }

    if (iss.fail()) {
        // Did not start with a number - clear state and continue (may be a "-N" expression)...
        iss.clear();
    }

    ch = iss.peek();
    if (ch == istringstream::traits_type::eof()) {
        // Only have a single number at this point.
        return make_pair(idx_low, idx_low + 1);
    }

    if (ch != '-')  {
        throw runtime_error{
            "Invalid test number match expression for -n|--number: '"
                + detail::to_char(num_pair) + "'"
        };
    }

    iss.get();

    auto idx_high_excl = numeric_limits<size_t>::max();
    iss >> idx_high_excl;
    return make_pair(idx_low, idx_high_excl);
}

// .first=true: short arg was matched.
std::pair<bool, std::vector<string_type>::iterator> find_arg(
    std::vector<string_type>& args,
    char id_short,
    std::string id_lng = ""
) {
    using namespace std;
    if (!args.empty()) {
        auto const id_short_ch = static_cast<char_type>(id_short);
        auto const id_long = detail::to_char_type(id_lng);
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            auto& arg = *it;
            if (arg.length() < 2) {
                continue;
            }
            if (id_short_ch && (arg[1] != '-')) {
                // Short-form argument. Check to see if id_short_ch is somewhere in the argument.
                auto const idx_ch = arg.find(id_short_ch, 1);
                if (idx_ch != string_type::npos) {
                    return make_pair(true, it);
                }
            }
            if ((!id_long.empty() && (*it == id_long))) {
                return make_pair(false, it);
            }
        }
    }
    return make_pair(false, args.end());
}

void remove_arg(
    char id,
    std::vector<string_type>& args,
    std::vector<string_type>::iterator it
) {
    auto const id_c = static_cast<char_type>(id);
    auto& arg = *it;
    for (auto idx = arg.find(id_c, 1); idx != string_type::npos; idx = arg.find(id_c, 1)) {
        arg.erase(idx, 1);
    }
    if (arg == TUXX_STR_LIT("-")) {
        args.erase(it);
    }
}

bool is_arg_specified(
    std::vector<string_type>& args,
    char id_short,
    char const* id_long = nullptr
) {
    auto const res = find_arg(args, id_short, id_long);
    if (res.second != args.end()) {
        if (res.first) {
            remove_arg(id_short, args, res.second);
        } else {
            args.erase(res.second);
        }
        return true;
    }
    return false;
}

std::pair<bool, string_type> get_arg_value(
    std::vector<string_type>& args,
    char id_short,
    char const* id_long = nullptr
) {
    using namespace std;
    auto const res = find_arg(args, id_short, id_long);
    if (res.second != args.end()) {
        if (res.first) {
            auto const ret = make_pair(true, res.second->substr(2));
            args.erase(res.second);
            return ret;
        }
        auto const it_value = args.erase(res.second);
        if (it_value >= args.end()) {
            return make_pair(true, string_type{});
        }
        string_type const value{*it_value};
        args.erase(it_value);
        return make_pair(true, value);
    }
    return make_pair(false, string_type{});
}

bool run_test_case(
    test_case_instance const& tc,
    test_case_reporter& rep,
    std::mutex& m,
    bool is_concurrent
) {
    using namespace std;
    using namespace std::chrono;
    steady_clock::time_point start_time;
    try {
        if (!is_concurrent) {
            unique_lock<mutex> _(m);
            rep.test_case_starting(tc);
        }
        detail::testing_context t_ctx{&tc, &rep, &m};
        start_time = steady_clock::now();
        tc.fn(t_ctx);
        {
            unique_lock<mutex> _(m);
            rep.test_case_passed(tc, steady_clock::now() - start_time);
        }
        return true;
    } catch (test_case_failure_error const& ex) {
        unique_lock<mutex> _(m);
        rep.test_case_failed(tc, ex, steady_clock::now() - start_time);
    } catch (std::exception const& ex) {
        unique_lock<mutex> _(m);
        rep.test_case_failed(
            tc,
            test_case_failure_error{
                tc.file,
                tc.line,
                TUXX_STR_LIT("Unhandled std::exception: ")
                    + detail::to_char_type(ex.what())
            },
            steady_clock::now() - start_time
        );
    } catch (...) {
        unique_lock<mutex> _(m);
        rep.test_case_failed(
            tc,
            test_case_failure_error{tc.file, tc.line, TUXX_STR_LIT("Unknown exception")},
            steady_clock::now() - start_time
        );
    }
    return false;
}

int run_test_cases(
    std::vector<test_case_instance>& tests,
    test_case_reporter& rep,
    std::size_t concurrency,
    bool randomized
) {
    using namespace std;
    using namespace chrono;

    if (randomized) {
        random_device rd;
        mt19937 g{rd()};
        shuffle(begin(tests), end(tests), g);
    }

    vector<test_case_instance> failures;
    auto const start_time = steady_clock::now();
    rep.start();
    if (!tests.empty()) {
        mutex mtx;
        condition_variable cond;
        auto is_ready = false;
        atomic<size_t> idx{};
        // Create up to concurrency-1 threads (the current thread is the extra 1).
        auto const n_workers{concurrency - 1};
        auto const worker_fn{
            [&tests, &idx, &mtx, &cond, &is_ready, &rep, &failures, &concurrency]() -> void{
                {
                    unique_lock<mutex> lk(mtx);
                    cond.wait(lk, [&is_ready]{ return is_ready; });
                }
                for (auto idx_curr = idx++; idx_curr < tests.size(); idx_curr = idx++) {
                    auto const& tc = tests[idx_curr];
                    auto const res = run_test_case(tc, rep, mtx, concurrency > 1);
                    if (!res) {
                        unique_lock<mutex> _(mtx);
                        failures.push_back(tc);
                    }
                }
            }
        };
        vector<thread> workers;
        workers.reserve(n_workers);
        for (auto i = 0u; i < n_workers; ++i) {
            workers.emplace_back(worker_fn);
        }
        is_ready = true;
        cond.notify_all();
        worker_fn();
        for (auto& w : workers) {
            w.join();
        }
    }
    rep.end_test_cases(steady_clock::now() - start_time, failures);
    return failures.empty() ? 0 : 1;
}

void list_tests(
    std::vector<test_case_instance> const& tests,
    report_ostream_type& os,
    bool color_enabled,
    bool use_emojis
) {
    for (auto const& tc : tests) {
        // Listing doesn't currently use a reporter.
        write_test_case_line_prefix(tc, os, tests.size() - 1, color_enabled, use_emojis) << '\n';
    }
}

bool does_test_case_match_names(
    test_case_instance const& tc,
    std::vector<string_type> const& matches
) {
    using namespace std;
    auto tc_name_lower{tc.name};
    to_lower(tc_name_lower);
    return find_if(
        matches.begin(),
        matches.end(),
        [&tc_name_lower](string_type const& match) -> bool {
            if (match.length() > tc_name_lower.length()) {
                return false;
            }
            auto match_lower = match;
            to_lower(match_lower);
            // Starts-with
            for (auto i = 0u; i < match.length(); ++i) {
                if (match_lower[i] != tc_name_lower[i]) {
                    return false;
                }
            }
            return true;
        }
    ) != matches.end();
}

bool does_test_case_match_number_exprs(
    test_case_instance const& tc,
    std::vector<string_type> const& num_exprs
) {
    using namespace std;
    auto const idx = tc.idx + 1;
    return find_if(
        num_exprs.begin(),
        num_exprs.end(),
        [idx](string_type const& num_expr) -> bool {
            auto const res = read_number_range(num_expr);
            return (idx >= res.first) && (idx < res.second);
        }
    ) != num_exprs.end();
}

struct file_desc {
    std::string name;
    int fd{-1};
    explicit file_desc(
        std::string nm = "",
        int f_desc = -1
    ) : name{std::move(nm)}, fd(f_desc) {}
    ~file_desc() { close(); }
    void attach(
        std::string nm,
        int f_desc
    ) {
        close();
        name = std::move(nm);
        fd = f_desc;
    }
    void release() { fd = -1; }
    void close() {
        if (fd >= 0) {
            if (tuxx_os_close(fd) < 0) {
                char buf[1000];
                strerror_r(errno, buf, sizeof(buf));
                fprintf(stderr, "Descriptor '%s' - error with close: %s", name.c_str(), buf);
            }
            fd = -1;
        }
    }

private:
    file_desc(file_desc const&);
    file_desc& operator=(file_desc const&);
};

struct redirect_output_deps {
    std::function<
        void(
            std::ostream&,
            std::wostream&
        )
    > flush_fn;
    std::function<int(int)> dup_fn;
    std::function<
        int(
            int,
            int
        )
    > dup2_fn;
    std::function<int(int*)> pipe_fn;
    static redirect_output_deps make_default() {
        redirect_output_deps ret;
        ret.flush_fn = [](
            std::ostream& os,
            std::wostream& wos
        ) {
            os.flush();
            wos.flush();
        };
        ret.dup_fn = tuxx_os_dup;
        ret.dup2_fn = tuxx_os_dup2;
        ret.pipe_fn = tuxx_os_pipe;
        return ret;
    }
};

template <typename FileDescType>
void redirect_output(
    int fd,
    FileDescType& out_saved,
    FileDescType& out_tmp,
    redirect_output_deps const& deps = redirect_output_deps::make_default()
) {
    using namespace std;
    auto const is_stdout = fd == STDOUT_FILENO;
    string const name = is_stdout ? "stdout" : "stderr";
    if (is_stdout) {
        deps.flush_fn(cout, wcout);
    } else {
        deps.flush_fn(cerr, wcerr);
    }
    out_saved.attach(name + "-saved", deps.dup_fn(fd));
    if (out_saved.fd < 0) {
        throw runtime_error{out_saved.name + " - failed to dup descriptor"};
    }
    int pipe_fds[2];
    if (deps.pipe_fn(pipe_fds) < 0) {
        throw runtime_error{name + " - failed to create redirection pipe"};
    }
    out_tmp.attach(name + "-r", pipe_fds[0]);
    FileDescType w{name + "-w", pipe_fds[1]};
    if (deps.dup2_fn(w.fd, fd) < 0) {
        throw runtime_error{w.name + " - failed to dup2 descriptor"};
    }
}

template <typename FileDescType>
void restore_output(
    int fd,
    FileDescType& saved,
    redirect_output_deps const& deps = redirect_output_deps::make_default()
) {
    using namespace std;
    if (fd == STDOUT_FILENO) {
        deps.flush_fn(cout, wcout);
    } else {
        deps.flush_fn(cerr, wcerr);
    }
    if (deps.dup2_fn(saved.fd, fd) < 0) {
        throw runtime_error{saved.name + " - failed to restore original descriptor"};
    }
    saved.release();
}

struct with_redirection_result {
    int status{};
    string_type stdout;
    string_type stderr;
};

template <
    typename ThreadType,
    typename FileDescType
>
struct with_redirection_deps {
    std::function<
        ssize_t(
            int,
            void*,
            std::size_t
        )
    > read_fn;
    std::function<
        std::ostream&(
            std::ostream&,
            char const*,
            std::streamsize
        )
    > write_fn;
    std::function<
        void(
            int,
            FileDescType&,
            FileDescType&
        )
    > redirect_output_fn;
    std::function<
        void(
            int,
            FileDescType&
        )
    > restore_output_fn;
    std::function<ThreadType(std::function<void()>)> create_thread_fn;
    std::function<void(ThreadType&)> join_thread_fn;
    static with_redirection_deps make_default() {
        with_redirection_deps ret;
        ret.read_fn = tuxx_os_read;
        ret.write_fn = [](
            std::ostream& os,
            char const* data,
            std::streamsize n
        ) -> std::ostream& {
            return os.write(data, n);
        };
        ret.redirect_output_fn = [](
            int fd,
            FileDescType& out_saved,
            FileDescType& out_tmp
        ) {
            return redirect_output(fd, out_saved, out_tmp);
        };
        ret.restore_output_fn = [](
            int fd,
            FileDescType& saved
        ) {
            return restore_output(fd, saved);
        };
        ret.create_thread_fn = [](std::function<void()> f) { return ThreadType{std::move(f)}; };
        ret.join_thread_fn = [](ThreadType& t) { t.join(); };
        return ret;
    }
};

template <
    typename ThreadType,
    typename FileDescType
>
with_redirection_result with_redirection_t(
    std::function<int()> const& f,
    with_redirection_deps<ThreadType, FileDescType> const& deps =
        with_redirection_deps<ThreadType, FileDescType>::make_default()
) {
    using namespace std;

    // Setup stdout/stderr redirection for capture...
    auto redirect_thread_fn = [&deps](
        ostringstream& oss,
        int rfd
    ) {
        char buf[1000];
        for (;;) {
            auto n_read = deps.read_fn(rfd, buf, sizeof(buf));
            if (n_read <= 0) {
                return;
            }
            deps.write_fn(oss, buf, n_read);
        }
    };

    FileDescType saved_stdout;
    FileDescType tmp_stdout_r;
    deps.redirect_output_fn(STDOUT_FILENO, saved_stdout, tmp_stdout_r);

    ostringstream stdout_oss;
    ThreadType stdout_thr{
        [&stdout_oss, redirect_thread_fn, &tmp_stdout_r]() {
            redirect_thread_fn(stdout_oss, tmp_stdout_r.fd);
        }
    };

    FileDescType saved_stderr;
    FileDescType tmp_stderr_r;
    deps.redirect_output_fn(STDERR_FILENO, saved_stderr, tmp_stderr_r);

    ostringstream stderr_oss;
    ThreadType stderr_thr{
        [&stderr_oss, redirect_thread_fn, &tmp_stderr_r]() {
            redirect_thread_fn(stderr_oss, tmp_stderr_r.fd);
        }
    };

    with_redirection_result ret;
    ret.status = f();

    // If we fail to restore stdout/stderr here, there's not really much
    // that can be done to inform the user other than returning an error.
    deps.restore_output_fn(STDOUT_FILENO, saved_stdout);
    deps.join_thread_fn(stdout_thr);

    deps.restore_output_fn(STDERR_FILENO, saved_stderr);
    deps.join_thread_fn(stderr_thr);

    ret.stdout = detail::to_char_type(stdout_oss.str());
    ret.stderr = detail::to_char_type(stderr_oss.str());
    
    return ret;
}

with_redirection_result with_redirection(
    std::function<int()> const& f,
    with_redirection_deps<std::thread, file_desc> const& deps =
        with_redirection_deps<std::thread, file_desc>::make_default()
) {
    return with_redirection_t<std::thread, file_desc>(f, deps);
}

void check_args(std::vector<string_type> const& args) {
    using namespace std;
    for (auto idx_arg = 1u; idx_arg < args.size(); ++idx_arg) {
        auto const& arg = args[idx_arg];
        if ((arg.length() < 2) || (arg[0] != '-')) {
            throw invalid_argument{"bad argument: '" + detail::to_char(arg) + "'"};
        }
        if (arg[1] == '-') {
            continue;   // Long-form argument
        }
        auto const idx_ch = arg.find_first_not_of(TUXX_STR_LIT("cehljprt"), 1);
        if (idx_ch != string_type::npos) {
            throw invalid_argument{
                string{"unrecognized argument '"} +
                    static_cast<char>(arg[idx_ch]) +
                    "' or argument cannot be combined with other arguments"
            };
        }
    }
}

struct main_deps {
    test_case_reporter* p_reporter{};
    std::function<
        void(
            string_type const&,
            std::basic_ostream<char_type>& os
        )
    > usage_fn;
    std::function<void(std::vector<string_type> const&)> check_args_fn;
    std::function<
        void(
            std::vector<test_case_instance> const&,
            report_ostream_type&,
            bool,
            bool
        )
    > list_tests_fn;
    std::function<
        bool(
            std::vector<string_type>&,
            char,
            char const*
        )
    > is_arg_specified_fn;
    std::function<
        std::pair<bool, string_type>(
            std::vector<string_type>&,
            char,
            char const*
        )
    > get_arg_value_fn;
    std::function<unsigned int()> get_hardware_concurrency_fn;
    std::function<int(int)> isatty_fn;
    std::function<
        with_redirection_result(std::function<int()> const&)
    > with_redirection_fn;
    std::function<
        int(
            std::vector<test_case_instance>&,
            test_case_reporter&,
            size_t,
            bool
        )
    > run_test_cases_fn;

    static main_deps make_default() {
        main_deps ret;
        ret.usage_fn = usage;
        ret.check_args_fn = check_args;
        ret.list_tests_fn = list_tests;
        ret.is_arg_specified_fn = is_arg_specified;
        ret.get_arg_value_fn = get_arg_value;
        ret.isatty_fn = tuxx_os_isatty;
        ret.with_redirection_fn = [](std::function<int()> const& f) {
            return with_redirection(f);
        };
        ret.run_test_cases_fn = run_test_cases;
        ret.get_hardware_concurrency_fn = [] { return std::thread::hardware_concurrency(); };
        return ret;
    }
};

int main(
    std::vector<string_type>& args,
    std::vector<test_case_instance>& tests,
    report_ostream_type& out_strm,
    report_ostream_type& err_strm,
    main_deps const& deps = main_deps::make_default()
) {
    using namespace std;
    using namespace tuxx;

    try {
        deps.check_args_fn(args);
    } catch (exception const& ex) {
        err_strm << "ERROR - " << ex.what() << endl;
        return 1;
    }

    if (deps.is_arg_specified_fn(args, 'h', "help")) {
        deps.usage_fn(args[0], out_strm);
        return 0;
    }
    auto write_colorized = !deps.is_arg_specified_fn(args, 0, "no-color");
    auto const use_emojis = deps.is_arg_specified_fn(args, 'e', "emojis");
    if (deps.is_arg_specified_fn(args, 'l', "list")) {
        for (auto i = 1u; i < args.size(); ++i) {
            err_strm << "WARNING: ignoring argument(s) '" << args[i] << "'\n";
        }
        deps.list_tests_fn(tests, out_strm, write_colorized, use_emojis);
        return 0;
    }

    auto const write_json = deps.is_arg_specified_fn(args, 'j', "json");
    auto const force_use_default_reporter = deps.is_arg_specified_fn(args, 't', "text");
    if (force_use_default_reporter && write_json) {
        err_strm << "ERROR: cannot specify both the default and json output.\n" ;
        return 1;
    }
    auto const randomized = deps.is_arg_specified_fn(args, 'r', "randomized");
    auto capture_output = deps.is_arg_specified_fn(args, 'c', "capture");

    auto write_junit = false;
    string_type junit_test_report_name;
    {
        auto res = deps.get_arg_value_fn(args, 0, "junit");
        auto const was_found = res.first;
        if (was_found) {
            if (force_use_default_reporter) {
                err_strm << "ERROR: cannot specify both the default and junit output.\n";
                return 1;
            }
            if (write_json) {
                err_strm << "ERROR: cannot specify both json and junit output.\n";
                return 1;
            }
            write_junit = true;
            junit_test_report_name = move(res.second);
        }
    }

    char_type opt_delim{};
    {
        auto const res = deps.get_arg_value_fn(args, 'd', "delim");
        auto const was_found = res.first;
        if (was_found) {
            if (force_use_default_reporter) {
                err_strm
                    << "ERROR: cannot specify both the default and delimited output.\n" ;
                return 1;
            }
            if (write_json) {
                err_strm << "ERROR: cannot specify both json and delimited output.\n";
                deps.usage_fn(args[0], out_strm);
                return 1;
            }
            if (write_junit) {
                err_strm << "ERROR: cannot specify both junit and delimited output.\n";
                deps.usage_fn(args[0], out_strm);
                return 1;
            }
            if (res.second.empty()) {
                err_strm
                    << "ERROR: delimiter character must be specified with -d|--delim.\n";
                deps.usage_fn(args[0], out_strm);
                return 1;
            }
            opt_delim = parse_delim(res.second);
            if (!opt_delim) {
                err_strm
                    << "ERROR: -d|--delim delimiter is invalid - must be a single character or '\t'"
                    << endl;
                deps.usage_fn(args[0], out_strm);
                return 1;
            }
        }
    }

    size_t concurrency{1};
    {
        auto const res = deps.get_arg_value_fn(args, 'p', "parallel");
        auto const was_found = res.first;
        if (was_found) {
            if (!res.second.empty()) {
                basic_istringstream<char_type> iss(res.second);
                if (!(iss >> concurrency)) {
                    err_strm
                        << "ERROR: a numeric value must be provided for -p|--parallel" << endl;
                    deps.usage_fn(args[0], out_strm);
                    return 1;
                }
            } else {
                concurrency = deps.get_hardware_concurrency_fn();
            }
        }
    }

    string_type opt_name_matches;
    {
        auto res = deps.get_arg_value_fn(args, 'm', "match");
        auto const was_found = res.first;
        opt_name_matches = move(res.second);
        if (was_found) {
            if (opt_name_matches.empty()) {
                err_strm << "ERROR: value required for '-m|--match' argument" << endl;
                deps.usage_fn(args[0], out_strm);
                return 1;
            }
        }
    }

    string_type opt_num_matches;
    {
        auto res = deps.get_arg_value_fn(args, 'n', "number");
        auto const was_found = res.first;
        opt_num_matches = move(res.second);
        if (was_found) {
            if (opt_num_matches.empty()) {
                err_strm << "ERROR: value required for '-n|--number' argument" << endl;
                deps.usage_fn(args[0], out_strm);
                return 1;
            }
        }
    }

    if (args.size() > 1) {
        auto const prog_name = args[0];
        args.erase(args.begin());
        for (auto const& arg : args) {
            err_strm << "ERROR: unrecognized argument: '" << arg.c_str() << "'" << endl;
        }
        deps.usage_fn(prog_name, out_strm);
        return 1;
    }

    if (!deps.isatty_fn(STDOUT_FILENO)) {
        // Avoid control characters if redirecting output...
        write_colorized = false;
    }

    function<bool(test_case_instance const&)> filter;

    if (!opt_name_matches.empty()) {
        auto const name_matches = split_delim(opt_name_matches, ',');
        filter = [name_matches](test_case_instance const& tc) {
            return does_test_case_match_names(tc, name_matches);
        };
    }
    if (!opt_num_matches.empty()) {
        auto const num_matches = split_delim(opt_num_matches, ',');
        if (filter) {
            filter = [filter, num_matches](test_case_instance const& tc) {
                return filter(tc) || does_test_case_match_number_exprs(tc, num_matches);
            };
        } else {
            filter = [num_matches](test_case_instance const& tc) {
                return does_test_case_match_number_exprs(tc, num_matches);
            };
        }
    }
    if (!filter) {
        filter = [](test_case_instance const& tc) { return true; };
    }

    tests.erase(
        remove_if(
            tests.begin(),
            tests.end(),
            [filter](test_case_instance const& tc) { return !filter(tc); }
        ),
        tests.end()
    );

    if (tests.size() < concurrency) {
        concurrency = tests.size();
    }

    // Use a temporary output stream if capturing stdout and stderr from code
    // under test.
    ostringstream_type rpt_os;

#if defined(TUXX_DEFINE_CUSTOM_REPORTER)
    // Test_args needs to be declared here to keep it alive for at least as
    // long as the reporter.
    test_case_reporter_args test_args;
#endif
    unique_ptr<test_case_reporter> reporter;
    auto p_reporter = deps.p_reporter;

    if (!p_reporter) {
        if (write_json) {
            capture_output = true;  // Force to true
            reporter.reset(new test_case_reporter_json{rpt_os, tests.size(), concurrency});
        } else if (write_junit) {
            capture_output = true;  // Force to true
            reporter.reset(
                new test_case_reporter_junit(
                    rpt_os,
                    args[0],
                    tests.size(),
                    junit_test_report_name
                )
            );
        } else if (opt_delim) {
            capture_output = true;  // Force to true
            reporter.reset(new test_case_reporter_delim{rpt_os, static_cast<char_type>(opt_delim)});
        } else {
            auto& ostrm = (capture_output ? rpt_os : out_strm);
#if defined(TUXX_DEFINE_CUSTOM_REPORTER)
            if (!force_use_default_reporter) {
                test_args.p_report_ostream = &ostrm;
                test_args.p_test_cases = tests.data();
                test_args.n_tests = tests.size();
                test_args.write_colorized = write_colorized;
                test_args.use_emojis = use_emojis;
                test_args.concurrency = concurrency;
                test_args.capture_output = capture_output;
                reporter.reset(::tuxx_make_custom_reporter(test_args));
            }
#endif
            if (!reporter) {
                reporter.reset(
                    new test_case_reporter_plain_text{
                        ostrm,
                        tests,
                        write_colorized,
                        concurrency,
                        use_emojis
                    }
                );
            }
        }
        p_reporter = reporter.get();
    }

    int ret{};
    if (capture_output) {
        auto const res = deps.with_redirection_fn(
            [&tests, &deps, concurrency, randomized, p_reporter] {
                return deps.run_test_cases_fn(tests, *p_reporter, concurrency, randomized);
            }
        );

        reporter->finish(res.stdout, res.stderr);

        out_strm << rpt_os.str() << "'" << flush;
    } else {
        ret = deps.run_test_cases_fn(tests, *p_reporter, concurrency, randomized);
        reporter->finish(string_type{}, string_type{});
    }

    return ret;
}

int wrap_main(
    std::vector<string_type>& args,
    std::vector<test_case_instance>& tests,
    report_ostream_type& out_strm = TUXX_REPORT_OSTRM,
    report_ostream_type& err_strm = TUXX_ERROR_OSTRM,
    std::function<
        int(
            std::vector<string_type>&,
            std::vector<test_case_instance>,
            report_ostream_type&,
            report_ostream_type&
        )
    > main_fn = nullptr
) {
    if (!main_fn) {
        main_fn = [](
            std::vector<string_type>& args,
            std::vector<test_case_instance> tests,
            report_ostream_type& out_strm,
            report_ostream_type& err_strm
        ) {
            return main(args, tests, out_strm, err_strm);
        };
    }
    try {
        return main_fn(args, tests, out_strm, err_strm);
    } catch (std::exception const& ex) {
        err_strm << "ERROR - unhandled exception: " << ex.what() << std::endl;
    } catch (...) {
        err_strm << "ERROR - unknown exception" << std::endl;
    }
    return 1;
}

int do_char_main(
    int argc,
    char_type* argv[]
) {
    std::vector<string_type> args(argv, argv + argc);
    return wrap_main(args, detail::tests__());
}

}
}
}
#endif

#if !defined(TUXX_UNDER_TEST)
    #if defined(TUXX_DEFINE_TEST_MAIN)
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
                    return ::nyej::tuxx::wrap_main(args, ::nyej::tuxx::detail::tests__());
                }
                #define TUXX_MAIN_IS_DEFINED
            #elif defined(TUXX_IS_WINDOWS_UNICODE)
                int wmain(
                    int argc,
                    wchar_t* argv[]
                ) {
                    return ::nyej::tuxx::do_char_main(argc, argv);
                }
                #define TUXX_MAIN_IS_DEFINED
            #endif
        #endif

        #if !defined(TUXX_MAIN_IS_DEFINED)
            int main(
                int argc,
                char* argv[]
            ) {
                return ::nyej::tuxx::do_char_main(argc, argv);
            }
        #endif
    #endif
#endif

#endif
