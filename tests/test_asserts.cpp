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

#include <vector>
#include <tuxx.hpp>

using namespace std;
using namespace nyej::tuxx;

// We're not #define'ing TUXX_DEFINE_TEST_MAIN here so we need to add an
// implementation for this
std::vector<test_case_instance>& nyej::tuxx::detail::tests__() {
    static std::vector<test_case_instance> tests;
    return tests;
}

struct testing_assert_info {
    test_case_instance tc;
    char const* file;
    int line;
    char const* assert_name;
    char const* expr;
    string_type msg;
    char const* ex_info;
};

struct testing_reporter : test_case_reporter {
    vector<testing_assert_info> asserts;
    void test_case_assert(
        test_case_instance const& tc,
        char const* file,
        int line,
        char const* assert_name,
        char const* expr,
        string_type const& msg,
        char const* ex_info = nullptr
    ) override {
        testing_assert_info inf;
        inf.tc = tc;
        inf.file = file;
        inf.line = line;
        inf.assert_name = assert_name;
        inf.expr = expr;
        inf.msg = msg;
        inf.ex_info = ex_info;
        asserts.push_back(inf);
    }
    void test_case_passed(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed
    ) override {}
    void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        std::chrono::steady_clock::duration const& elapsed
    ) override {}
    void end_test_cases(
        std::chrono::steady_clock::duration const& elapsed,
        std::vector<test_case_instance> const& failures
    ) override {}
    void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) override {}
};

#define TESTING_PREP_T_CTX(assert_nm) \
    char const assert_name[] = assert_nm; \
    string_type const test_name{TESTING_STR_LIT("test_" assert_nm)}; \
    test_case_instance tc{__FILE__, __LINE__,  test_name, 1, nullptr}; \
    testing_reporter rpt; \
    detail::testing_context t_ctx__{&tc, &rpt, &m}; \
    auto line_no = __LINE__;

#define TESTING_MSG 15 << " abc"
#define TESTING_MSG_TXT TESTING_STR_LIT("15 abc")
#define CHECK_ASSERT_RESULTS(idx, expr_txt, msg_expr) \
    do { \
        ++line_no; \
        testing_assert(strcmp(rpt.asserts[idx].assert_name, assert_name) == 0); \
        testing_assert(strcmp(rpt.asserts[idx].expr, expr_txt) == 0); \
        testing_assert(strcmp(rpt.asserts[idx].file, __FILE__) == 0); \
        testing_assert(rpt.asserts[idx].line == line_no); \
        testing_assert(rpt.asserts[idx].msg == msg_expr); \
    } while (false)

template <typename Ch>
int testing_main(
    int argc,
    Ch* argv[]
) {
    mutex m;

    {
        TESTING_PREP_T_CTX("assert_true");
        assert_true(true);
        assert_true(2);
        assert_true_msg(true, TESTING_MSG);

        testing_assert(rpt.asserts.size() == 3u);

        CHECK_ASSERT_RESULTS(0, "true is truthy", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(1, "2 is truthy", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(2, "true is truthy", TESTING_MSG_TXT);
    }

    {
        int* p_null = nullptr;
        TESTING_PREP_T_CTX("assert_false");
        assert_false(false);
        assert_false(0 + 0);
        assert_false(p_null);
        assert_false_msg(false, TESTING_MSG);

        testing_assert(rpt.asserts.size() == 4u);

        CHECK_ASSERT_RESULTS(0, "false is falsey", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(1, "0 + 0 is falsey", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(2, "p_null is falsey", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(3, "false is falsey", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_null");
        assert_null(nullptr);
        assert_null(0);
        assert_null_msg(nullptr, TESTING_MSG);

        testing_assert(rpt.asserts.size() == 3u);

        CHECK_ASSERT_RESULTS(0, "nullptr is null", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(1, "0 is null", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(2, "nullptr is null", TESTING_MSG_TXT);
    }

    {
        int i = 0;
        int* p_i = &i;
        TESTING_PREP_T_CTX("assert_not_null");
        assert_not_null(p_i);
        assert_not_null_msg(p_i, TESTING_MSG);

        testing_assert(rpt.asserts.size() == 2u);

        CHECK_ASSERT_RESULTS(0, "p_i is not null", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(1, "p_i is not null", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_eq");
        assert_eq(2, (1 + 1));
        assert_eq(string_type{TESTING_STR_LIT("Hello")}, TESTING_STR_LIT("Hello"));
        assert_eq_msg(3, (2 + 1), TESTING_MSG);

        testing_assert(rpt.asserts.size() == 3u);

        CHECK_ASSERT_RESULTS(0, "2 == (1 + 1)", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(
            1,
            "string_type{" TESTING_LIT_PREFIX "\"Hello\"} == " TESTING_LIT_PREFIX "\"Hello\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(2, "3 == (2 + 1)", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_ne");
        assert_ne(2, (1 + 2));
        assert_ne(string_type{TESTING_STR_LIT("Hello")}, TESTING_STR_LIT("Hello1"));
        assert_ne_msg(2, 3, TESTING_MSG);

        testing_assert(rpt.asserts.size() == 3u);

        CHECK_ASSERT_RESULTS(0, "2 != (1 + 2)", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(
            1,
            "string_type{" TESTING_LIT_PREFIX "\"Hello\"} != " TESTING_LIT_PREFIX "\"Hello1\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(2, "2 != 3", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_lt");
        assert_lt(1, (1 + 1));
        assert_lt(string_type{TESTING_STR_LIT("Hello")}, TESTING_STR_LIT("Jello"));
        assert_lt_msg(2, (2 + 1), TESTING_MSG);

        testing_assert(rpt.asserts.size() == 3u);

        CHECK_ASSERT_RESULTS(0, "1 < (1 + 1)", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(
            1,
            "string_type{" TESTING_LIT_PREFIX "\"Hello\"} < " TESTING_LIT_PREFIX "\"Jello\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(2, "2 < (2 + 1)", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_le");
        assert_le(1, (1 + 1));
        assert_le(2, (1 + 1));
        assert_le(string_type{TESTING_STR_LIT("Hello")}, TESTING_STR_LIT("Jello"));
        assert_le(string_type{TESTING_STR_LIT("Hello")}, TESTING_STR_LIT("Hello"));
        assert_le_msg(2, (2 + 1), TESTING_MSG);
        assert_le_msg(3, (2 + 1), TESTING_MSG);

        testing_assert(rpt.asserts.size() == 6u);

        CHECK_ASSERT_RESULTS(0, "1 <= (1 + 1)", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(1, "2 <= (1 + 1)", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(
            2,
            "string_type{" TESTING_LIT_PREFIX "\"Hello\"} <= " TESTING_LIT_PREFIX "\"Jello\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(
            3,
            "string_type{" TESTING_LIT_PREFIX "\"Hello\"} <= " TESTING_LIT_PREFIX "\"Hello\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(4, "2 <= (2 + 1)", TESTING_MSG_TXT);
        CHECK_ASSERT_RESULTS(5, "3 <= (2 + 1)", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_gt");
        assert_gt(4, (1 + 1));
        assert_gt(string_type{TESTING_STR_LIT("Jello")}, TESTING_STR_LIT("Hello"));
        assert_gt_msg(4, (2 + 1), TESTING_MSG);

        testing_assert(rpt.asserts.size() == 3u);

        CHECK_ASSERT_RESULTS(0, "4 > (1 + 1)", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(
            1,
            "string_type{" TESTING_LIT_PREFIX "\"Jello\"} > " TESTING_LIT_PREFIX "\"Hello\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(2, "4 > (2 + 1)", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_ge");
        assert_ge(4, (1 + 1));
        assert_ge(2, (1 + 1));
        assert_ge(string_type{TESTING_STR_LIT("Jello")}, TESTING_STR_LIT("Hello"));
        assert_ge(string_type{TESTING_STR_LIT("Hello")}, TESTING_STR_LIT("Hello"));
        assert_ge_msg(4, (2 + 1), TESTING_MSG);
        assert_ge_msg(3, (2 + 1), TESTING_MSG);

        testing_assert(rpt.asserts.size() == 6u);

        CHECK_ASSERT_RESULTS(0, "4 >= (1 + 1)", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(1, "2 >= (1 + 1)", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(
            2,
            "string_type{" TESTING_LIT_PREFIX "\"Jello\"} >= " TESTING_LIT_PREFIX "\"Hello\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(
            3,
            "string_type{" TESTING_LIT_PREFIX "\"Hello\"} >= " TESTING_LIT_PREFIX "\"Hello\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(4, "4 >= (2 + 1)", TESTING_MSG_TXT);
        CHECK_ASSERT_RESULTS(5, "3 >= (2 + 1)", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_close");
        assert_close(4, (1 + 1), 7 - 3);
        assert_close_msg(5, (2 + 1), 7 - 3, TESTING_MSG);

        testing_assert(rpt.asserts.size() == 2u);

        CHECK_ASSERT_RESULTS(0, "4 ~= (1 + 1) +/- 7 - 3", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(1, "5 ~= (2 + 1) +/- 7 - 3", TESTING_MSG_TXT);
    }

    {
        TESTING_PREP_T_CTX("assert_throws");
        assert_throws(throw runtime_error{"hello"}, runtime_error);
        assert_throws_msg(throw logic_error{"good bye"}, logic_error, TESTING_MSG);

        testing_assert(rpt.asserts.size() == 2u);

        CHECK_ASSERT_RESULTS(
            0,
            "throw runtime_error{\"hello\"} throws runtime_error",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(
            1,
            "throw logic_error{\"good bye\"} throws logic_error",
            TESTING_MSG_TXT
        );
    }

    {
        TESTING_PREP_T_CTX("assert_ieq");
        assert_ieq("hello", "HEllO");
        assert_ieq(TESTING_STR_LIT("hello"), TESTING_STR_LIT("HEllO"));
        assert_ieq(string_type{TESTING_STR_LIT("hello")}, TESTING_STR_LIT("HEllO"));
        assert_ieq(TESTING_STR_LIT("hello"), string_type{TESTING_STR_LIT("HEllO")});
        assert_ieq(string_type{TESTING_STR_LIT("hello")}, string_type{TESTING_STR_LIT("HEllO")});
        assert_ieq(string{"hello"}, "HEllO");
        assert_ieq("hello", string{"HEllO"});
        assert_ieq(string{"hello"}, string{"HEllO"});
        assert_ieq_msg("hello", string{"HEllO"}, TESTING_MSG);
        assert_ieq_msg(TESTING_STR_LIT("hello"), string_type{TESTING_STR_LIT("HEllO")}, TESTING_MSG);

        testing_assert(rpt.asserts.size() == 10u);

        CHECK_ASSERT_RESULTS(
            0,
            "\"hello\" ^v= \"HEllO\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(
            1,
            TESTING_LIT_PREFIX "\"hello\" ^v= " TESTING_LIT_PREFIX "\"HEllO\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(
            2,
            "string_type{" TESTING_LIT_PREFIX "\"hello\"} ^v= " TESTING_LIT_PREFIX "\"HEllO\"",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(
            3,
            TESTING_LIT_PREFIX "\"hello\" ^v= string_type{" TESTING_LIT_PREFIX "\"HEllO\"}",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(
            4,
            "string_type{" TESTING_LIT_PREFIX "\"hello\"} ^v= "
                "string_type{" TESTING_LIT_PREFIX "\"HEllO\"}",
            TESTING_STR_LIT("")
        );
        CHECK_ASSERT_RESULTS(5, "string{\"hello\"} ^v= \"HEllO\"", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(6, "\"hello\" ^v= string{\"HEllO\"}", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(7, "string{\"hello\"} ^v= string{\"HEllO\"}", TESTING_STR_LIT(""));
        CHECK_ASSERT_RESULTS(8, "\"hello\" ^v= string{\"HEllO\"}", TESTING_MSG_TXT);
        CHECK_ASSERT_RESULTS(
            9,
            TESTING_LIT_PREFIX "\"hello\" ^v= string_type{" TESTING_LIT_PREFIX "\"HEllO\"}",
            TESTING_MSG_TXT
        );
    }

    {
        char const assert_name[] = "test_fail";
        test_case_instance tc{__FILE__, __LINE__, TESTING_STR_LIT("test_fail"), 1, nullptr};
        testing_reporter rpt;
        detail::testing_context t_ctx__{&tc, &rpt, &m};
        auto line_no = 0;
        try {
            line_no = __LINE__;
            test_fail(TESTING_MSG);
            testing_assert(false);
        } catch (nyej::tuxx::test_case_failure_error const&) {
        }

        testing_assert(rpt.asserts.size() == 1u);

        CHECK_ASSERT_RESULTS(0, "fail", TESTING_MSG_TXT);
    }

    return 0;
}
