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

using namespace std;
using namespace nyej::tuxx;

namespace {

void test_iequals() {
    testing_assert(detail::iequals(TESTING_STR_LIT("hello"), TESTING_STR_LIT("HelLo")));
    testing_assert(
        detail::iequals(
            string_type{TESTING_STR_LIT("hello")},
            TESTING_STR_LIT("HelLo")
        )
    );
    testing_assert(
        detail::iequals(
            TESTING_STR_LIT("HelLo"),
            string_type{TESTING_STR_LIT("hello")}
        )
    );
    testing_assert(
        detail::iequals(
            string_type{TESTING_STR_LIT("HelLo")},
            string_type{TESTING_STR_LIT("hello")}
        )
    );
    testing_assert(
        !detail::iequals(
            TESTING_STR_LIT("hello"),
            TESTING_STR_LIT("helo")
        )
    );
    testing_assert(
        !detail::iequals(
            string_type{TESTING_STR_LIT("hello")},
            TESTING_STR_LIT("helo")
        )
    );
    testing_assert(
        !detail::iequals(
            TESTING_STR_LIT("helo"),
            string_type{TESTING_STR_LIT("hello")}
        )
    );
    testing_assert(
        !detail::iequals(
            string_type{TESTING_STR_LIT("helo")},
            string_type{TESTING_STR_LIT("hello")}
        )
    );
}

void test_calc_id_field_width() {
    testing_assert(tst::calc_id_field_width(1) == 1);
    testing_assert(tst::calc_id_field_width(9) == 1);
    testing_assert(tst::calc_id_field_width(10) == 2);
    testing_assert(tst::calc_id_field_width(54) == 2);
    testing_assert(tst::calc_id_field_width(99) == 2);
    testing_assert(tst::calc_id_field_width(100) == 3);
    testing_assert(tst::calc_id_field_width(561) == 3);
    testing_assert(tst::calc_id_field_width(999) == 3);
    testing_assert(tst::calc_id_field_width(1000) == 4);
    testing_assert(tst::calc_id_field_width(4987) == 4);
    testing_assert(tst::calc_id_field_width(9999) == 4);
    testing_assert(tst::calc_id_field_width(10000) == 5);
    testing_assert(tst::calc_id_field_width(10001) == 5);
    testing_assert(tst::calc_id_field_width(99999) == 5);
    testing_assert(tst::calc_id_field_width(100000) == 6);
    testing_assert(tst::calc_id_field_width(100001) == 6);
}

void test_write_id() {
    {
        ostringstream_type oss;
        tst::write_id(14, 1, oss, false);
        testing_assert(oss.str() == TESTING_STR_LIT("[14]"));
    }

    {
        ostringstream_type oss;
        tst::write_id(14, 16, oss, false);
        testing_assert(oss.str() == TESTING_STR_LIT("[14]"));
    }

    {
        ostringstream_type oss;
        tst::write_id(14, 100, oss, false);
        testing_assert(oss.str() == TESTING_STR_LIT("[ 14]"));
    }

    // With color...

    {
        ostringstream_type oss;
        tst::write_id(14, 1, oss, true);
        ostringstream_type expected_oss;
        expected_oss << "\033[" << static_cast<unsigned>(TUXX_COLOR_SQ_BR) << "m[\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_TEST_ID) << "m14\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_SQ_BR) << "m]\033[0m";
        testing_assert(oss.str() == expected_oss.str());
    }

    {
        ostringstream_type oss;
        tst::write_id(14, 16, oss, true);
        ostringstream_type expected_oss;
        expected_oss << "\033[" << static_cast<unsigned>(TUXX_COLOR_SQ_BR) << "m[\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_TEST_ID) << "m14\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_SQ_BR) << "m]\033[0m";
        testing_assert(oss.str() == expected_oss.str());
    }

    {
        ostringstream_type oss;
        tst::write_id(14, 100, oss, true);
        ostringstream_type expected_oss;
        expected_oss << "\033[" << static_cast<unsigned>(TUXX_COLOR_SQ_BR) << "m[\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_TEST_ID) << "m 14\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_SQ_BR) << "m]\033[0m";
        testing_assert(oss.str() == expected_oss.str());
    }
}

#if defined(USE_ESCAPE)
string_type escape(string_type const& s) {
    string_type ret;
    for (auto c : s) {
        if (c < ' ') {
            static char const hex[] = "0123456789abcdef";
            ret += hex[(c >> 4) & 0xf];
            ret += hex[c & 0xf];
        } else {
            ret += c;
        }
    }
    return ret;
}
#endif

void test_write_test_case_name() {
    {
        ostringstream_type oss;
        test_case_instance tc{__FILE__, __LINE__, TESTING_STR_LIT("test"), 20, nullptr};
        tst::write_test_case_name(oss, tc, false);
        testing_assert(oss.str() == TESTING_STR_LIT("test"));
    }

    {
        ostringstream_type oss;
        test_case_instance tc{__FILE__, __LINE__, TESTING_STR_LIT("test"), 20, nullptr};
        tst::write_test_case_name(oss, tc, false, 10);
        testing_assert(oss.str() == TESTING_STR_LIT("test      "));
    }

    {
        ostringstream_type oss;
        test_case_instance tc{
            __FILE__,
            __LINE__,
            TESTING_STR_LIT("test"),
            20,
            nullptr,
            TESTING_STR_LIT("aa")
        };
        tst::write_test_case_name(oss, tc, false);
        testing_assert(oss.str() == TESTING_STR_LIT("test(aa)"));
    }

    {
        ostringstream_type oss;
        test_case_instance tc{
            __FILE__,
            __LINE__,
            TESTING_STR_LIT("test"),
            20,
            nullptr,
            TESTING_STR_LIT("aa")
        };
        tst::write_test_case_name(oss, tc, false, 10);
        testing_assert(oss.str() == TESTING_STR_LIT("test(aa)  "));
    }

    // With color...

    {
        ostringstream_type oss;
        test_case_instance tc{__FILE__, __LINE__, TESTING_STR_LIT("test"), 20, nullptr};
        tst::write_test_case_name(oss, tc, true);
        ostringstream_type expected_oss;
        expected_oss << "\033[" << static_cast<unsigned>(TUXX_COLOR_TEST_NAME) << "mtest\033[0m";
        testing_assert(oss.str() == expected_oss.str());
    }

    {
        ostringstream_type oss;
        test_case_instance tc{__FILE__, __LINE__, TESTING_STR_LIT("test"), 20, nullptr};
        tst::write_test_case_name(oss, tc, true, 10);
        ostringstream_type expected_oss;
        expected_oss << "\033[" << static_cast<unsigned>(TUXX_COLOR_TEST_NAME)
            << "mtest\033[0m      ";
        testing_assert(oss.str() == expected_oss.str());
    }

    {
        ostringstream_type oss;
        test_case_instance tc{
            __FILE__,
            __LINE__,
            TESTING_STR_LIT("test"),
            20,
            nullptr,
            TESTING_STR_LIT("aa")
        };
        tst::write_test_case_name(oss, tc, true);
        ostringstream_type expected_oss;
        expected_oss << "\033[" << static_cast<unsigned>(TUXX_COLOR_TEST_NAME)
            << "mtest\033[0m\033[" << static_cast<unsigned>(TUXX_COLOR_PAREN) << "m(\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_PARAM) << "maa\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_PAREN) << "m)\033[0m";
        testing_assert(oss.str() == expected_oss.str());
    }

    {
        ostringstream_type oss;
        test_case_instance tc{
            __FILE__,
            __LINE__,
            TESTING_STR_LIT("test"),
            20,
            nullptr,
            TESTING_STR_LIT("aa")
        };
        tst::write_test_case_name(oss, tc, true, 10);
        ostringstream_type expected_oss;
        expected_oss << "\033[" << static_cast<unsigned>(TUXX_COLOR_TEST_NAME)
            << "mtest\033[0m\033[" << static_cast<unsigned>(TUXX_COLOR_PAREN) << "m(\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_PARAM) << "maa\033[0m"
            << "\033[" << static_cast<unsigned>(TUXX_COLOR_PAREN) << "m)\033[0m  ";
        testing_assert(oss.str() == expected_oss.str());
    }
}

void test_write_test_case_line_prefix() {
    test_case_instance tc{__FILE__, __LINE__, TESTING_STR_LIT("test"), 20, nullptr};

    static struct {
        bool with_color;
        bool with_emojis;
    } const args[] = {
        {false, false},
        {false, true},
        {true, false},
        {true, true}
    };

    for (auto const& arg : args) {
        ostringstream_type oss;

        size_t write_id_id{};
        size_t write_id_max_id{};
        report_ostream_type* write_id_p_os{};
        bool write_id_write_color{};
        size_t write_id_order{100};

        report_ostream_type* write_test_case_name_p_os{};
        test_case_instance const* write_test_case_name_p_tc{};
        bool write_test_case_name_write_color{};
        size_t write_test_case_name_width{};
        size_t write_test_case_name_order{100};

        report_ostream_type* flush_p_os{};
        size_t flush_order{100};

        size_t call_order{};
        auto const max_test_case_id = 10u;
        auto& res = tst::write_test_case_line_prefix(
            tc,
            oss,
            max_test_case_id,
            arg.with_color,
            arg.with_emojis,
            0,
            tst::write_test_case_line_prefix_deps{
                [&](
                    size_t id,
                    size_t max_id,
                    report_ostream_type& os,
                    bool write_color
                ) -> report_ostream_type& {
                    write_id_id = id;
                    write_id_max_id = max_id;
                    write_id_p_os = &os;
                    write_id_write_color = write_color;
                    write_id_order = call_order++;
                    return os;
                },
                [&](
                    report_ostream_type& os,
                    test_case_instance const& tc,
                    bool write_color,
                    size_t width
                ) {
                    write_test_case_name_p_os = &os;
                    write_test_case_name_p_tc = &tc;
                    write_test_case_name_write_color = write_color;
                    write_test_case_name_width = width;
                    write_test_case_name_order = call_order++;
                },
                [&](report_ostream_type& os) {
                    flush_p_os = &os;
                    flush_order = call_order++;
                }
            }
        );
        testing_assert(&res == &oss);

        testing_assert(write_id_id == tc.id);
        testing_assert(write_id_max_id == max_test_case_id);
        testing_assert(write_id_p_os == &oss); // Also verifies stub was called.
        testing_assert(write_id_write_color == arg.with_color);
        testing_assert(write_id_order == 0);

        testing_assert(write_test_case_name_p_os == &oss); // Also verifies stub was called.
        testing_assert(write_test_case_name_p_tc == &tc);
        testing_assert(write_test_case_name_write_color == arg.with_color);
        testing_assert(write_test_case_name_width == 0);
        testing_assert(write_test_case_name_order == 1);

        testing_assert(flush_p_os == &oss); // Also verifies stub was called.
        testing_assert(flush_order == 2);

        testing_assert(call_order == 3);    // Only 3 calls should have been made.

        // This is all that was written in this test...
        if (arg.with_emojis) {
            ostringstream_type expected_oss;
            expected_oss << ' ' << TUXX_EMOJI_PREFIX << ' ';
            testing_assert(oss.str() == expected_oss.str());
        } else {
            testing_assert(oss.str() == TESTING_STR_LIT(" "));
        }
    }
}

bool is_color(
    string_type const& s,
    size_t idx,
    tst::color col,
    char c
) {
    ostringstream_type oss;
    oss << "\033[" << static_cast<unsigned>(col) << "m" << c << "\033[0m";
    return s.find(oss.str(), idx) == idx;
}

bool is_color(
    string_type const& s,
    size_t idx,
    tst::color col,
    size_t n_digits
) {
    ostringstream_type oss;
    oss << "\033[" << static_cast<unsigned>(col) << "m";
    auto const str = oss.str();
    if (s.find(str, idx) != idx) {
        return false;
    }
    for (
        auto idx_src = str.length() + idx, i = size_t{};
        (i < n_digits) && ((idx_src + i) < s.length());
        ++i
    ) {
        if (!isdigit(s[idx_src + i])) {
            return false;
        }
    }
    idx += str.length() + n_digits;
    return s.find(TESTING_STR_LIT("\033[0m"), idx) == idx;
}

void test_write_duration() {
    auto const start = chrono::system_clock::now();
    auto const elapsed = chrono::system_clock::now() - start;  // Going to be 0.

    static struct {
        bool write_color;
        bool include_brackets;
    } const args[] = {
        {false, false},
        {false, true},
        {true, false},
        {true, true}
    };

    for (auto const& arg : args) {
        ostringstream_type oss;
        auto& res = tst::write_duration(oss, elapsed, arg.write_color, arg.include_brackets);

        auto const str = oss.str();

        testing_assert(&res == &oss);
        auto offset = 0u;

        // @[37m00@[0m@[34m:@[0m@[37m00@[0m@[34m:@[0m@[37m00@[0m@[34m.@[0m@[37m000@[0m

        if (arg.include_brackets) {
            if (arg.write_color) {
                testing_assert(is_color(str, 0, TUXX_COLOR_SQ_BR, '['));
                offset = 10;
            } else {
                testing_assert(str[0] == '[');
                offset = 1;
            }
        }
        if (arg.write_color) {
            testing_assert(is_color(str, offset, TUXX_COLOR_DURATION, size_t{2}));
            offset += 11;
            testing_assert(is_color(str, offset, TUXX_COLOR_PUNCT, ':'));
            offset += 10;
            testing_assert(is_color(str, offset, TUXX_COLOR_DURATION, size_t{2}));
            offset += 11;
            testing_assert(is_color(str, offset, TUXX_COLOR_PUNCT, ':'));
            offset += 10;
            testing_assert(is_color(str, offset, TUXX_COLOR_DURATION, size_t{2}));
            offset += 11;
            testing_assert(is_color(str, offset, TUXX_COLOR_PUNCT, '.'));
            offset += 10;
            testing_assert(is_color(str, offset, TUXX_COLOR_DURATION, size_t{3}));
            offset += 12;
        } else {
            testing_assert(isdigit(str[offset++]));
            testing_assert(isdigit(str[offset++]));
            testing_assert(str[offset++] == ':');
            testing_assert(isdigit(str[offset++]));
            testing_assert(isdigit(str[offset++]));
            testing_assert(str[offset++] == ':');
            testing_assert(isdigit(str[offset++]));
            testing_assert(isdigit(str[offset++]));
            testing_assert(str[offset++] == '.');
            testing_assert(isdigit(str[offset++]));
            testing_assert(isdigit(str[offset++]));
            testing_assert(isdigit(str[offset++]));
        }
        if (arg.include_brackets) {
            if (arg.write_color) {
                testing_assert(is_color(str, offset, TUXX_COLOR_SQ_BR, ']'));
                offset += 10;
            } else {
                testing_assert(str[offset++] == ']');
            }
        }
        testing_assert(offset == str.length());
    }
}

void test_get_time_stamp() {
    auto const s = tst::get_time_stamp(chrono::system_clock::now());
    testing_assert(s.length() == 24);
    testing_assert(isdigit(s[0]));
    testing_assert(isdigit(s[1]));
    testing_assert(isdigit(s[2]));
    testing_assert(isdigit(s[3]));
    testing_assert(s[4] == '-');
    testing_assert(isdigit(s[5]));
    testing_assert(isdigit(s[6]));
    testing_assert(s[7] == '-');
    testing_assert(isdigit(s[8]));
    testing_assert(isdigit(s[9]));
    testing_assert(s[10] == 'T');
    testing_assert(isdigit(s[11]));
    testing_assert(isdigit(s[12]));
    testing_assert(s[13] == ':');
    testing_assert(isdigit(s[14]));
    testing_assert(isdigit(s[15]));
    testing_assert(s[16] == ':');
    testing_assert(isdigit(s[17]));
    testing_assert(isdigit(s[18]));
    testing_assert((s[19] == '-') || s[19] == '+');
    testing_assert(isdigit(s[20]));
    testing_assert(isdigit(s[21]));
    testing_assert(isdigit(s[22]));
    testing_assert(isdigit(s[23]));
}

void test_usage() {
    // Just testing program name here...
    ostringstream_type oss;
    auto const prog_name = TESTING_STR_LIT("xyz");
    tst::usage(prog_name, oss);
    auto const str = oss.str();
    testing_assert(str.find(prog_name) == 0);
    testing_assert(str.find(TESTING_STR_LIT(" unit tests")) == 3);
}

void test_to_lower() {
    string_type s{TESTING_STR_LIT("hello world")};
    tst::to_lower(s);
    testing_assert(s == TESTING_STR_LIT("hello world"));

    s = TESTING_STR_LIT("HeLLo WoRLd");
    tst::to_lower(s);
    testing_assert(s == TESTING_STR_LIT("hello world"));

    s.clear();
    tst::to_lower(s);
    testing_assert(s.empty());
}

void test_split_delim() {
    {
        auto const res = tst::split_delim<char_type>(TESTING_STR_LIT("a,,bbb,cccccc,"), ',');
        testing_assert(res.size() == 3);
        testing_assert(res[0] == TESTING_STR_LIT("a"));
        testing_assert(res[1] == TESTING_STR_LIT("bbb"));
        testing_assert(res[2] == TESTING_STR_LIT("cccccc"));
    }

    {
        auto const res = tst::split_delim<char_type>(TESTING_STR_LIT("a,,bbb,cccccc,"), ',', true);
        testing_assert(res.size() == 5);
        testing_assert(res[0] == TESTING_STR_LIT("a"));
        testing_assert(res[1].empty());
        testing_assert(res[2] == TESTING_STR_LIT("bbb"));
        testing_assert(res[3] == TESTING_STR_LIT("cccccc"));
        testing_assert(res[4].empty());
    }
}

void test_parse_delim() {
    testing_assert(tst::parse_delim(TESTING_STR_LIT("x")) == char_type{'x'});
    testing_assert(tst::parse_delim(TESTING_STR_LIT("\\t")) == char_type{'\t'});
    testing_assert(tst::parse_delim(TESTING_STR_LIT("\\n")) == char_type{});
    testing_assert(tst::parse_delim(TESTING_STR_LIT("")) == char_type{});
    testing_assert(tst::parse_delim(TESTING_STR_LIT("abc")) == char_type{});
}

void test_read_number_range() {
    {
        try {
            tst::read_number_range(TESTING_STR_LIT("-"));
            testing_assert(false && "expected exception on dash");
        } catch (...) {
        }
    }
    {
        try {
            tst::read_number_range(TESTING_STR_LIT(""));
            testing_assert(false && "expected exception on empty string");
        } catch (...) {
        }
    }
    {
        auto const res = tst::read_number_range(TESTING_STR_LIT("15"));
        testing_assert(res.first == 15);
        testing_assert(res.second == 15);
    }
    {
        auto const res = tst::read_number_range(TESTING_STR_LIT("15-15"));
        testing_assert(res.first == 15);
        testing_assert(res.second == 15);
    }
    {
        auto const res = tst::read_number_range(TESTING_STR_LIT("-34"));
        testing_assert(res.first == 1);
        testing_assert(res.second == 34);
    }
    {
        auto const res = tst::read_number_range(TESTING_STR_LIT("34-"));
        testing_assert(res.first == 34);
        testing_assert(res.second == numeric_limits<size_t>::max());
    }
    {
        auto const res = tst::read_number_range(TESTING_STR_LIT("12-145"));
        testing_assert(res.first == 12);
        testing_assert(res.second == 145);
    }
    {
        try {
            tst::read_number_range(TESTING_STR_LIT("512-145"));
            testing_assert(false && "expected exception on inverted indexes");
        } catch (...) {
        }
    }
}

void test_find_arg() {
    vector<string_type> args{
        TESTING_STR_LIT("-a"),
        TESTING_STR_LIT("--long-arg"),
        TESTING_STR_LIT("-xyz")
    };

    {
        auto const res = tst::find_arg(args, 'b');
        testing_assert(!res.first);
        testing_assert(res.second == args.end());
    }

    {
        auto const res = tst::find_arg(args, 'b', "some-arg");
        testing_assert(!res.first);
        testing_assert(res.second == args.end());
    }

    {
        auto const res = tst::find_arg(args, 'l', "long-arg");
        testing_assert(!res.first);
        testing_assert(res.second == (args.begin() + 1));
    }

    static char const short_args[] = "xyz";

    for (auto i = 0u; short_args[i]; ++i) {
        auto const res = tst::find_arg(args, short_args[i], "long-xyz");
        testing_assert(res.first);
        testing_assert(res.second == (args.begin() + 2));
    }
}

void test_remove_arg() {
    auto const long_arg = TESTING_STR_LIT("--long-arg");

    vector<string_type> args{TESTING_STR_LIT("-a"), long_arg, TESTING_STR_LIT("-xyz") };

    tst::remove_arg('a', args, args.begin());
    testing_assert(args.size() == 2);
    testing_assert(args[0] == long_arg);
    testing_assert(args[1] == TESTING_STR_LIT("-xyz"));

    tst::remove_arg('y', args, args.begin() + 1);
    testing_assert(args.size() == 2);
    testing_assert(args[0] == long_arg);
    testing_assert(args[1] == TESTING_STR_LIT("-xz"));

    tst::remove_arg('x', args, args.begin() + 1);
    testing_assert(args.size() == 2);
    testing_assert(args[0] == long_arg);
    testing_assert(args[1] == TESTING_STR_LIT("-z"));

    tst::remove_arg('z', args, args.begin() + 1);
    testing_assert(args.size() == 1);
    testing_assert(args[0] == long_arg);
}

void test_is_arg_specified() {
    auto const long_arg = TESTING_STR_LIT("--long-arg");

    vector<string_type> args{TESTING_STR_LIT("-a"), long_arg, TESTING_STR_LIT("-xyz") };

    {
        auto const res = tst::is_arg_specified(args, 'g');
        testing_assert(!res);
    }

    {
        auto res = tst::is_arg_specified(args, 'g', "long-arg");
        testing_assert(res);
        testing_assert(args.size() == 2);
        testing_assert(args[0] == TESTING_STR_LIT("-a"));
        testing_assert(args[1] == TESTING_STR_LIT("-xyz"));
    }

    {
        auto res = tst::is_arg_specified(args, 'z');
        testing_assert(res);
        testing_assert(args.size() == 2);
        testing_assert(args[0] == TESTING_STR_LIT("-a"));
        testing_assert(args[1] == TESTING_STR_LIT("-xy"));
    }

    {
        auto res = tst::is_arg_specified(args, 'x');
        testing_assert(res);
        testing_assert(args.size() == 2);
        testing_assert(args[0] == TESTING_STR_LIT("-a"));
        testing_assert(args[1] == TESTING_STR_LIT("-y"));
    }

    {
        auto res = tst::is_arg_specified(args, 'y');
        testing_assert(res);
        testing_assert(args.size() == 1);
        testing_assert(args[0] == TESTING_STR_LIT("-a"));
    }

    {
        auto res = tst::is_arg_specified(args, 'a');
        testing_assert(res);
        testing_assert(args.empty());
    }
}

void test_get_arg_value() {
    vector<string_type> args{
        TESTING_STR_LIT("-n"),
        TESTING_STR_LIT("5"),
        TESTING_STR_LIT("--value"),
        TESTING_STR_LIT("the-value"),
        TESTING_STR_LIT("-n5-96"),
        TESTING_STR_LIT("-m"),
        TESTING_STR_LIT("abc"),
        TESTING_STR_LIT("-mxyz"),
        TESTING_STR_LIT("--number"),
        TESTING_STR_LIT("32"),
        TESTING_STR_LIT("--number"),    // Will be allowed to consume following argument
        TESTING_STR_LIT("--xyz"),
        TESTING_STR_LIT("--number"),    // Will not be allowed to consume following argument
        TESTING_STR_LIT("--xyz"),
        TESTING_STR_LIT("--last")       // Missing valued (at the end)
    };

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'g', "adfafd");
        testing_assert(!res.first);
        testing_assert(args.size() == len);
    }

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'n', "aefef");
        testing_assert(res.first);
        testing_assert(res.second == TESTING_STR_LIT("5"));
        testing_assert(args.size() == (len - 2));
    }

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 0, "value");
        testing_assert(res.first);
        testing_assert(res.second == TESTING_STR_LIT("the-value"));
        testing_assert(args.size() == (len - 2));
    }

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'n', "tgsf");
        testing_assert(res.first);
        testing_assert(res.second == TESTING_STR_LIT("5-96"));
        testing_assert(args.size() == (len - 1));
    }

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'm');
        testing_assert(res.first);
        testing_assert(res.second == TESTING_STR_LIT("abc"));
        testing_assert(args.size() == (len - 2));
    }

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'm');
        testing_assert(res.first);
        testing_assert(res.second == TESTING_STR_LIT("xyz"));
        testing_assert(args.size() == (len - 1));
    }

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'n', "number");
        testing_assert(res.first);
        testing_assert(res.second == TESTING_STR_LIT("32"));
        testing_assert(args.size() == (len - 2));
    }

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'n', "number", true);
        testing_assert(res.first);
        testing_assert(res.second == TESTING_STR_LIT("--xyz"));
        testing_assert(args.size() == (len - 2));
    }


    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'n', "number");
        testing_assert(res.first);
        testing_assert(res.second.empty());
        testing_assert(args.size() == (len - 1));
    }

    {
        // Not really a test, just consuming this value.
        auto const len = args.size();
        testing_assert(tst::is_arg_specified(args, 0, "xyz"));
        testing_assert(args.size() == (len - 1));
    }

    {
        auto const len = args.size();
        auto const res = tst::get_arg_value(args, 'l', "last");
        testing_assert(res.first);
        testing_assert(res.second.empty());
        testing_assert(args.size() == (len - 1));
    }

    testing_assert(args.empty());
}

void test_run_test_case() {
    auto now_call_ct = 0u;

    tst::run_test_case_deps<int, int> deps{
        [&]{
            return ++now_call_ct;
        },
        [&](
            int const& a,
            int const& b
        ) {
            return a - b;
        }
    };

    using test_case_instance_type = basic_test_case_instance<int, testing_mutex>;
    using testing_context_type = detail::basic_testing_context<int, testing_mutex>;

    // Happy path
    for (auto i = 0u; i < 2; ++i) {
        auto is_concurrent = !!i;
        auto const test_name = TESTING_STR_LIT("the-test");
        auto tc_fn_call_ct = 0u;
        testing_context_type tc_ctx{nullptr, nullptr, nullptr};
        test_case_instance_type tc{
            __FILE__,
            __LINE__,
            test_name,
            1,
            [&](testing_context_type const& t_ctx__) {
                TUXX_CHECK_RESULT_IMPL__("assert_true", true, "true", "", "", testing_unique_lock);
                TUXX_CHECK_RESULT_IMPL__(
                    "assert_false",
                    true,
                    "false",
                    "",
                    "",
                    testing_unique_lock
                );
                ++tc_fn_call_ct;
                tc_ctx = t_ctx__;
            }
        };
        testing_mutex mtx;
        testing_test_case_reporter rep;
        auto const res = tst::run_test_case<testing_unique_lock, int, int>(
            tc,
            rep,
            mtx,
            is_concurrent,
            deps
        );
        testing_assert(res);
        testing_assert(tc_fn_call_ct == 1);
        testing_assert(tc_ctx.tc == &tc);
        testing_assert(tc_ctx.reporter == &rep);
        testing_assert(tc_ctx.mtx == &mtx);
        testing_assert(rep.start_ct == 0);
        if (is_concurrent) {
            testing_assert(rep.tc_starting_calls.empty());
        } else {
            testing_assert(rep.tc_starting_calls.size() == 1);
            testing_assert(rep.tc_starting_calls[0].tc == &tc);
            testing_assert(!rep.tc_starting_calls[0].is_mtx_locked);
        }
        testing_assert(rep.tc_passed_calls.size() == 1);
        testing_assert(rep.tc_passed_calls[0].tc == &tc);
        testing_assert(rep.tc_passed_calls[0].is_mtx_locked);
        testing_assert(rep.tc_passed_calls[0].elapsed == 1);
        testing_assert(rep.tc_assert_calls.size() == 2);
        testing_assert(strcmp(rep.tc_assert_calls[0].assert_name, "assert_true") == 0);
        testing_assert(strcmp(rep.tc_assert_calls[0].expr, "true") == 0);
        testing_assert(rep.tc_assert_calls[0].is_mtx_locked);
        testing_assert(rep.tc_assert_calls[0].tc == &tc);
        testing_assert(strcmp(rep.tc_assert_calls[1].assert_name, "assert_false") == 0);
        testing_assert(strcmp(rep.tc_assert_calls[1].expr, "false") == 0);
        testing_assert(rep.tc_assert_calls[1].is_mtx_locked);
        testing_assert(rep.tc_assert_calls[1].tc == &tc);
        testing_assert(rep.tc_failed_calls.empty());
        testing_assert(rep.end_tcs_calls.empty());
        testing_assert(rep.finish_calls.empty());
        testing_assert(rep.tc_failed_calls.empty());
    }

    // Assertion failure path
    for (auto i = 0u; i < 2; ++i) {
        auto is_concurrent = !!i;
        auto const test_name = TESTING_STR_LIT("the-test");
        auto tc_fn_call_ct = 0u;
        auto fail_line = 0;
        testing_context_type tc_ctx{nullptr, nullptr, nullptr};
        test_case_instance_type tc{
            __FILE__,
            __LINE__,
            test_name,
            1,
            [&](testing_context_type const& t_ctx__) {
                ++tc_fn_call_ct;
                tc_ctx = t_ctx__;
                TUXX_CHECK_RESULT_IMPL__("assert_true", true, "true", "", "", testing_unique_lock);
                fail_line = __LINE__ + 1; // KEEP FOLLOWING STATEMENT ON A SINGLE LINE
                TUXX_CHECK_RESULT_IMPL__("assert_false", false, "true", "the-detail", "", testing_unique_lock);
                // This assert will not be recorded.
                TUXX_CHECK_RESULT_IMPL__("assert_true", true, "true", "", "", testing_unique_lock);
            }
        };
        testing_mutex mtx;
        testing_test_case_reporter rep;
        auto const res = tst::run_test_case<testing_unique_lock, int, int>(
            tc,
            rep,
            mtx,
            is_concurrent,
            deps
        );
        testing_assert(!res);
        testing_assert(tc_fn_call_ct == 1);
        testing_assert(tc_ctx.tc == &tc);
        testing_assert(tc_ctx.reporter == &rep);
        testing_assert(tc_ctx.mtx == &mtx);
        if (is_concurrent) {
            testing_assert(rep.tc_starting_calls.empty());
        } else {
            testing_assert(rep.tc_starting_calls.size() == 1);
            testing_assert(rep.tc_starting_calls[0].tc == &tc);
            testing_assert(!rep.tc_starting_calls[0].is_mtx_locked);
        }
        testing_assert(rep.tc_passed_calls.empty());
        testing_assert(rep.tc_assert_calls.size() == 2);
        testing_assert(strcmp(rep.tc_assert_calls[0].assert_name, "assert_true") == 0);
        testing_assert(strcmp(rep.tc_assert_calls[0].expr, "true") == 0);
        testing_assert(rep.tc_assert_calls[0].is_mtx_locked);
        testing_assert(rep.tc_assert_calls[0].tc == &tc);
        testing_assert(strcmp(rep.tc_assert_calls[1].assert_name, "assert_false") == 0);
        testing_assert(strcmp(rep.tc_assert_calls[1].expr, "true") == 0);
        testing_assert(rep.tc_assert_calls[1].is_mtx_locked);
        testing_assert(rep.tc_assert_calls[1].tc == &tc);
        testing_assert(rep.end_tcs_calls.empty());
        testing_assert(rep.finish_calls.empty());
        testing_assert(rep.tc_failed_calls.size() == 1);
        testing_assert(rep.tc_failed_calls[0].elapsed == 1);
        testing_assert(rep.tc_failed_calls[0].is_mtx_locked);
        testing_assert(rep.tc_failed_calls[0].tc == &tc);
        testing_assert(strcmp(rep.tc_failed_calls[0].err.file, __FILE__) == 0);
        testing_assert(rep.tc_failed_calls[0].err.detail == TESTING_STR_LIT("the-detail"));
        testing_assert(rep.tc_failed_calls[0].err.line == fail_line);
    }

    // Unhandled exception path
    for (auto i = 0u; i < 2; ++i) {
        auto is_concurrent = !!i;
        auto const test_name = TESTING_STR_LIT("the-test");
        auto tc_fn_call_ct = 0u;
        testing_context_type tc_ctx{nullptr, nullptr, nullptr};
        test_case_instance_type tc{
            __FILE__,
            __LINE__,
            test_name,
            1,
            [&](testing_context_type const& t_ctx__) {
                ++tc_fn_call_ct;
                tc_ctx = t_ctx__;
                TUXX_CHECK_RESULT_IMPL__("assert_true", true, "true", "", "", testing_unique_lock);
                throw runtime_error{"test exception"};
                // This assert will not be recorded.
                TUXX_CHECK_RESULT_IMPL__("assert_true", true, "true", "", "", testing_unique_lock);
            }
        };
        testing_mutex mtx;
        testing_test_case_reporter rep;
        auto const res = tst::run_test_case<testing_unique_lock, int, int>(
            tc,
            rep,
            mtx,
            is_concurrent,
            deps
        );
        testing_assert(!res);
        testing_assert(tc_fn_call_ct == 1);
        testing_assert(tc_ctx.tc == &tc);
        testing_assert(tc_ctx.reporter == &rep);
        testing_assert(tc_ctx.mtx == &mtx);
        if (is_concurrent) {
            testing_assert(rep.tc_starting_calls.empty());
        } else {
            testing_assert(rep.tc_starting_calls.size() == 1);
            testing_assert(rep.tc_starting_calls[0].tc == &tc);
            testing_assert(!rep.tc_starting_calls[0].is_mtx_locked);
        }
        testing_assert(rep.tc_passed_calls.empty());
        testing_assert(rep.tc_assert_calls.size() == 1);
        testing_assert(strcmp(rep.tc_assert_calls[0].assert_name, "assert_true") == 0);
        testing_assert(strcmp(rep.tc_assert_calls[0].expr, "true") == 0);
        testing_assert(rep.tc_assert_calls[0].is_mtx_locked);
        testing_assert(rep.tc_assert_calls[0].tc == &tc);
        testing_assert(rep.end_tcs_calls.empty());
        testing_assert(rep.finish_calls.empty());
        testing_assert(rep.tc_failed_calls.size() == 1);
        testing_assert(rep.tc_failed_calls[0].elapsed == 1);
        testing_assert(rep.tc_failed_calls[0].is_mtx_locked);
        testing_assert(rep.tc_failed_calls[0].tc == &tc);
        testing_assert(strcmp(rep.tc_failed_calls[0].err.file, __FILE__) == 0);
        testing_assert(
            rep.tc_failed_calls[0].err.detail.find(
                TESTING_STR_LIT("Unhandled exception: ")
            ) == 0
        );
        testing_assert(
            rep.tc_failed_calls[0].err.detail.find(TESTING_STR_LIT("test exception"))
                != string_type::npos
        );
        testing_assert(rep.tc_failed_calls[0].err.line == tc.line);
    }

    // Unhandled unknown exception path
    for (auto i = 0u; i < 2; ++i) {
        auto is_concurrent = !!i;
        auto const test_name = TESTING_STR_LIT("the-test");
        auto tc_fn_call_ct = 0u;
        testing_context_type tc_ctx{nullptr, nullptr, nullptr};
        test_case_instance_type tc{
            __FILE__,
            __LINE__,
            test_name,
            1,
            [&](testing_context_type const& t_ctx__) {
                ++tc_fn_call_ct;
                tc_ctx = t_ctx__;
                TUXX_CHECK_RESULT_IMPL__("assert_true", true, "true", "", "", testing_unique_lock);
                throw 5;
                // This assert will not be recorded.
                TUXX_CHECK_RESULT_IMPL__("assert_true", true, "true", "", "", testing_unique_lock);
            }
        };
        testing_mutex mtx;
        testing_test_case_reporter rep;
        auto const res = tst::run_test_case<testing_unique_lock, int, int>(
            tc,
            rep,
            mtx,
            is_concurrent,
            deps
        );
        testing_assert(!res);
        testing_assert(tc_fn_call_ct == 1);
        testing_assert(tc_ctx.tc == &tc);
        testing_assert(tc_ctx.reporter == &rep);
        testing_assert(tc_ctx.mtx == &mtx);
        if (is_concurrent) {
            testing_assert(rep.tc_starting_calls.empty());
        } else {
            testing_assert(rep.tc_starting_calls.size() == 1);
            testing_assert(rep.tc_starting_calls[0].tc == &tc);
            testing_assert(!rep.tc_starting_calls[0].is_mtx_locked);
        }
        testing_assert(rep.tc_passed_calls.empty());
        testing_assert(rep.tc_assert_calls.size() == 1);
        testing_assert(strcmp(rep.tc_assert_calls[0].assert_name, "assert_true") == 0);
        testing_assert(strcmp(rep.tc_assert_calls[0].expr, "true") == 0);
        testing_assert(rep.tc_assert_calls[0].is_mtx_locked);
        testing_assert(rep.tc_assert_calls[0].tc == &tc);
        testing_assert(rep.end_tcs_calls.empty());
        testing_assert(rep.finish_calls.empty());
        testing_assert(rep.tc_failed_calls.size() == 1);
        testing_assert(rep.tc_failed_calls[0].elapsed == 1);
        testing_assert(rep.tc_failed_calls[0].is_mtx_locked);
        testing_assert(rep.tc_failed_calls[0].tc == &tc);
        testing_assert(strcmp(rep.tc_failed_calls[0].err.file, __FILE__) == 0);
        testing_assert(rep.tc_failed_calls[0].err.detail == TESTING_STR_LIT("Unknown exception"));
        testing_assert(rep.tc_failed_calls[0].err.line == tc.line);
    }
}

struct run_test_case_call {
    basic_test_case_instance<int, testing_mutex> const* tc;
    basic_test_case_reporter<int, testing_mutex> const* rep;
    testing_mutex const* mtx;
    bool is_concurrent;
    bool is_mtx_locked;
};

struct add_failure_call {
    basic_test_case_instance<int, testing_mutex> const* tc;
    bool is_mtx_locked;
};

struct test_run_test_cases_arg {
    size_t concurrency;
    bool empty_tests;
    bool randomized;
    bool with_failures;
};

void test_run_test_cases() {
    using test_case_instance_type = basic_test_case_instance<int, testing_mutex>;
    using test_case_reporter_type = basic_test_case_reporter<int, testing_mutex>;

    vector<test_case_instance_type> tests_list{
        {__FILE__, __LINE__, TESTING_STR_LIT("test-0"), 0, nullptr},
        {__FILE__, __LINE__, TESTING_STR_LIT("test-1"), 1, nullptr},
        {__FILE__, __LINE__, TESTING_STR_LIT("test-2"), 2, nullptr},
        {__FILE__, __LINE__, TESTING_STR_LIT("test-3"), 3, nullptr},
        {__FILE__, __LINE__, TESTING_STR_LIT("test-4"), 4, nullptr}
    };

    static size_t concurrencies[] = {0, 1, 3, 13/* > tests.size() */};
    vector<test_run_test_cases_arg> args;
    for (auto c : concurrencies) {
        for (auto idx_et = 0u; idx_et < 2; ++idx_et) {
            for (auto idx_r = 0u; idx_r < 2; ++idx_r) {
                for (auto idx_f = 0u; idx_f < 2; ++idx_f) {
                    args.push_back(test_run_test_cases_arg{c, !!idx_et, !!idx_r, !!idx_f});
                }
            }
        }
    }
    for (auto const& arg : args) {
        auto now_call_ct = 0u;

        vector<vector<test_case_instance_type> const*> randomize_calls;
        vector<run_test_case_call> run_tc_calls;
        vector<bool> cond_wait_calls;
        vector<bool> cond_notify_all_calls;
        vector<bool> create_thread_calls;
        vector<bool> join_thread_calls;
        vector<add_failure_call> add_failure_calls;

        auto did_pass = true;

        tst::run_test_cases_deps<
            int,
            testing_mutex,
            testing_unique_lock,
            int,
            int,
            int
        > deps{
            [&](
                test_case_instance_type const& tc,
                test_case_reporter_type& rep,
                testing_mutex& m,
                bool is_concurrent
            ) {
                run_tc_calls.push_back(
                    run_test_case_call{
                        &tc,
                        &rep,
                        &m,
                        is_concurrent,
                        testing_mutex::instance()->is_locked()
                    }
                );
                if (arg.with_failures) {
                    auto const ret = did_pass;
                    did_pass = !did_pass;
                    return ret;
                }
                return true;
            },
            [&]{
                return ++now_call_ct;
            },
            [&](
                int const& a,
                int const& b
            ) {
                return a - b;
            },
            [&](vector<test_case_instance_type>& tests) {
                randomize_calls.push_back(&tests);
            },
            [&](
                int& cond,
                testing_unique_lock& lk,
                function<bool()> const&
            ) {
                cond_wait_calls.push_back(testing_mutex::instance()->is_locked());
            },
            [&](int& cond) {
                cond_notify_all_calls.push_back(testing_mutex::instance()->is_locked());
            },
            [&](function<void()> f) {
                create_thread_calls.push_back(testing_mutex::instance()->is_locked());
                f();
                return 0;
            },
            [&](int&) {
                join_thread_calls.push_back(testing_mutex::instance()->is_locked());
            },
            [&](
                vector<test_case_instance_type>& failures,
                test_case_instance_type const& tc
            ) {
                add_failure_calls.push_back(
                    add_failure_call{
                        &tc,
                        testing_mutex::instance()->is_locked()
                    }
                );
                failures.push_back(tc); // Needed to test reporter's end_test_cases().
            }
        };

        testing_test_case_reporter rep;
        auto is_concurrent = arg.concurrency > 1;

        vector<test_case_instance_type> empty_tests;
        auto& tests = arg.empty_tests ? empty_tests : tests_list;
        auto const actual_threads = min(max(size_t{1}, arg.concurrency), tests.size());
        auto const ret = tst::run_test_cases_t<
            int,
            testing_mutex,
            testing_unique_lock,
            int,
            int,
            int
        >(tests, rep, arg.concurrency, arg.randomized, deps);

        auto const expect_failures = (!arg.empty_tests && arg.with_failures);

        testing_assert(ret == expect_failures ? 1 : 0);
        if (!tests.empty() && arg.randomized) {
            testing_assert(randomize_calls.size() == 1);
            testing_assert(randomize_calls[0] == &tests);
        } else {
            testing_assert(randomize_calls.empty());
        }
        // The test case reporter only has start() and end_test_cases() called on it
        // in this test scenario (due to our dependency injection).
        testing_assert(rep.start_ct == 1);
        testing_assert(rep.end_tcs_calls.size() == 1);
        testing_assert(rep.end_tcs_calls[0].elapsed = 1);

        if (expect_failures) {
            testing_assert(rep.end_tcs_calls[0].failures.size() == 2);
            testing_assert(strcmp(rep.end_tcs_calls[0].failures[0].file, __FILE__) == 0);
            testing_assert(rep.end_tcs_calls[0].failures[0].id == tests[1].id);
            testing_assert(rep.end_tcs_calls[0].failures[0].name == tests[1].name);
            testing_assert(strcmp(rep.end_tcs_calls[0].failures[1].file, __FILE__) == 0);
            testing_assert(rep.end_tcs_calls[0].failures[1].id == tests[3].id);
            testing_assert(rep.end_tcs_calls[0].failures[1].name == tests[3].name);
            testing_assert(add_failure_calls.size() == 2);
            testing_assert(add_failure_calls[0].tc == &tests[1]);
            testing_assert(add_failure_calls[0].is_mtx_locked == is_concurrent);
            testing_assert(add_failure_calls[1].tc == &tests[3]);
            testing_assert(add_failure_calls[1].is_mtx_locked == is_concurrent);
        } else {
            testing_assert(add_failure_calls.empty());
            testing_assert(rep.end_tcs_calls[0].failures.empty());
        }

        if (tests.empty() || !is_concurrent) {
            testing_assert(create_thread_calls.empty());
            testing_assert(join_thread_calls.empty());
            testing_assert(cond_wait_calls.empty());
            testing_assert(cond_notify_all_calls.empty());
        } else {
            testing_assert(create_thread_calls.size() == (actual_threads - 1));
            for (auto const& ctc_is_mtx_locked : create_thread_calls) {
                testing_assert(!ctc_is_mtx_locked);
            }
            testing_assert(cond_wait_calls.size() == actual_threads);
            for (auto const& cwc_is_mtx_locked : cond_wait_calls) {
                testing_assert(cwc_is_mtx_locked);
            }
            testing_assert(cond_notify_all_calls.size() == 1);
            for (auto const& cnc_is_mtx_locked : cond_notify_all_calls) {
                testing_assert(!cnc_is_mtx_locked);
            }
            testing_assert(join_thread_calls.size() == (actual_threads - 1));
            for (auto const& jtc_is_mtx_locked : join_thread_calls) {
                testing_assert(!jtc_is_mtx_locked);
            }
        }

        testing_assert(run_tc_calls.size() == tests.size());
    }
}

void test_list_tests() {
    static struct {
        bool write_color;
        bool use_emojis;
    } const args[] = {
        {false, false},
        {false, true},
        {true, false},
        {true, true}
    };

    for (auto const& arg : args) {

        size_t n_write_newline_calls{};
        vector<string_type> tc_names;
        vector<size_t> max_tc_ids;
        vector<report_ostream_type*> p_oses;
        vector<bool> did_write_colors;
        vector<bool> did_use_emojis;
        ostringstream_type oss;
        static char_type const* tc_name_strs[] = {
            TESTING_STR_LIT("test-0"),
            TESTING_STR_LIT("test-1"),
            TESTING_STR_LIT("test-2"),
            TESTING_STR_LIT("test-3"),
            TESTING_STR_LIT("test-4"),
            nullptr
        };
        vector<test_case_instance> tests;
        for (auto idx_tc_name = 0u; tc_name_strs[idx_tc_name]; ++idx_tc_name) {
            tests.push_back(
                test_case_instance{
                    __FILE__,
                    __LINE__,
                    tc_name_strs[idx_tc_name],
                    1,          // Not relevant in this test
                    nullptr     // Not relevant in this test
                }
            );
        }
        list_tests(
            tests,
            oss,
            arg.write_color,
            arg.use_emojis,
            tst::list_tests_deps{
                [&](
                    test_case_instance const& tc,
                    report_ostream_type& os,
                    size_t max_test_case_id,
                    bool write_color,
                    bool use_emojis
                ) {
                    tc_names.push_back(tc.name);
                    max_tc_ids.push_back(max_test_case_id);
                    p_oses.push_back(&os);
                    did_write_colors.push_back(write_color);
                    did_use_emojis.push_back(use_emojis);
                },
                [&](report_ostream_type& os) { ++n_write_newline_calls; }
            }
        );
        testing_assert(tc_names.size() == tests.size());
        testing_assert(max_tc_ids.size() == tests.size());
        testing_assert(p_oses.size() == tests.size());
        testing_assert(did_write_colors.size() == tests.size());
        testing_assert(did_use_emojis.size() == tests.size());
        testing_assert(n_write_newline_calls == tests.size());

        for (auto idx_call = 0u; idx_call < tests.size(); ++idx_call) {
            testing_assert(tc_names[idx_call] == tests[idx_call].name);
            testing_assert(max_tc_ids[idx_call] == (tests.size() - 1));
            testing_assert(p_oses[idx_call] == &oss);
            testing_assert(did_write_colors[idx_call] == arg.write_color);
            testing_assert(did_use_emojis[idx_call] == arg.use_emojis);
        }
    }
}

void test_does_test_case_match_names() {
    test_case_instance tc{__FILE__, __LINE__, TESTING_STR_LIT("group-1_test-0"), 1, nullptr};
    vector<string_type> matches{
        TESTING_STR_LIT("a"),
        TESTING_STR_LIT("grip")
    };
    testing_assert(!tst::does_test_case_match_names(tc, matches));
    matches.push_back(TESTING_STR_LIT("group-1"));
    testing_assert(tst::does_test_case_match_names(tc, matches));
    matches.back() = TESTING_STR_LIT("gr");
    testing_assert(tst::does_test_case_match_names(tc, matches));
}

void test_does_test_case_match_number_exprs() {
    test_case_instance tc{__FILE__, __LINE__, TESTING_STR_LIT("group-1_test-0"), 7, nullptr};
    vector<string_type> matches{
        TESTING_STR_LIT("-4"),
        TESTING_STR_LIT("8-10"),
        TESTING_STR_LIT("8-"),
        TESTING_STR_LIT("1-6")
    };
    testing_assert(!tst::does_test_case_match_number_exprs(tc, matches));
    matches.push_back(TESTING_STR_LIT("7"));
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("4-7");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("-7");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("7-7");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("7-");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("7-20");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("-20");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("-8");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("6-");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
    matches.back() = TESTING_STR_LIT("1-");
    testing_assert(tst::does_test_case_match_number_exprs(tc, matches));
}
struct testing_file_desc {
    string name;
    int fd{-1};
    testing_file_desc() = default;
    testing_file_desc(
        string const& nm,
        int f
    ) : name{nm}, fd{f} {}
    void attach(
        string const& n,
        int f
    ) {
        name = n;
        fd = f;
    }
    void release() {
        fd = -1;
    }
};

struct test_redirect_arg {
    bool fail_dup;
    bool fail_pipe;
    bool fail_dup2;
    bool is_stdout;
};

struct testing_redirect_flush_call {
    size_t seq;
    ostream* p_os;
    wostream* p_wos;
};

struct testing_redirect_dup_call {
    size_t seq;
    int fd;
};

struct testing_redirect_pipe_call {
    size_t seq;
    int fd1;
    int fd2;
};

struct testing_redirect_dup2_call {
    size_t seq;
    int fd1;
    int fd2;
};

void test_redirect_output() {
    vector<test_redirect_arg> args;
    for (auto idx_d = 0u; idx_d < 2; ++idx_d) {
        for (auto idx_p = 0u; idx_p < 2; ++idx_p) {
            for (auto idx_d2 = 0u; idx_d2 < 2; ++idx_d2) {
                for (auto idx_o = 0u; idx_o < 2; ++idx_o) {
                    args.push_back({!!idx_d, !!idx_p, !!idx_d2, !!idx_o});
                }
            }
        }
    }

    for (auto const& arg : args) {
        testing_file_desc saved;
        testing_file_desc tmp;

        auto seq = 0u;
        vector<testing_redirect_flush_call> flush_calls;
        vector<testing_redirect_dup_call> dup_calls;
        vector<testing_redirect_pipe_call> pipe_calls;
        vector<testing_redirect_dup2_call> dup2_calls;

        auto const saved_fd = 102;

        auto did_throw_exception = false;

        try {
            tst::redirect_output<testing_file_desc>(
                arg.is_stdout ? STDOUT_FILENO : STDERR_FILENO,
                saved,
                tmp,
                tst::redirect_output_deps{
                    [&](
                        ostream& os,
                        wostream& wos
                    ) {
                        flush_calls.push_back(testing_redirect_flush_call{++seq, &os, &wos});
                    },
                    [&](int fd) {
                        dup_calls.push_back({++seq, fd});
                        return arg.fail_dup ? -1 : saved_fd;
                    },
                    [&](int p[]) {
                        p[0] = 123;
                        p[1] = 456;
                        pipe_calls.push_back({++seq, p[0], p[1]});
                        return arg.fail_pipe ? -1 : 0;
                    },
                    [&](
                        int fd1,
                        int fd2
                    ) {
                        dup2_calls.push_back({++seq, fd1, fd2});
                        return arg.fail_dup2 ? -1 : 0;
                    }
                }
            );
        } catch (exception const& ex) {
            did_throw_exception = true;
            if (arg.fail_dup) {
                testing_assert(string{ex.what()}.find("failed to dup descriptor") != string::npos);
            } else if (arg.fail_pipe) {
                testing_assert(
                    string{ex.what()}.find("failed to create redirection pipe") != string::npos
                );
            } else if (arg.fail_dup2) {
                testing_assert(string{ex.what()}.find("failed to dup2 descriptor") != string::npos);
            } else {
                cerr << "Unexpected exception: " << ex.what() << endl;
                testing_assert(false && "unexpected exception");
            }
        }

        if (arg.fail_dup || arg.fail_pipe || arg.fail_dup) {
            testing_assert(did_throw_exception);
        }

        testing_assert(flush_calls.size() == 1);
        testing_assert(flush_calls[0].seq == 1);
        testing_assert(flush_calls[0].p_os == (arg.is_stdout ? &cout : &cerr));
        testing_assert(flush_calls[0].p_wos == (arg.is_stdout ? &wcout : &wcerr));

        string const fd_name{arg.is_stdout ? "stdout" : "stderr"};
        int const fd_num = (arg.is_stdout ? STDOUT_FILENO : STDERR_FILENO);

        testing_assert(saved.name == fd_name + "-saved");

        testing_assert(dup_calls.size() == 1);
        testing_assert(dup_calls[0].seq == 2);
        testing_assert(dup_calls[0].fd == fd_num);
        if (arg.fail_dup) {
            testing_assert(saved.fd == -1);
            testing_assert(pipe_calls.empty());
            testing_assert(dup2_calls.empty());
            testing_assert(tmp.fd == -1);
        } else {
            testing_assert(saved.fd == saved_fd);
            testing_assert(pipe_calls.size() == 1);
            testing_assert(pipe_calls[0].seq == 3);
            if (arg.fail_pipe) {
                testing_assert(dup2_calls.empty());
                testing_assert(tmp.fd == -1);
            } else {
                testing_assert(tmp.fd == pipe_calls[0].fd1);
                testing_assert(tmp.name == fd_name + "-r");
                testing_assert(dup2_calls.size() == 1);
                testing_assert(dup2_calls[0].seq == 4);
                testing_assert(dup2_calls[0].fd1 == pipe_calls[0].fd2);
            }
        }
    }
}

void test_restore_output() {
    static struct {
        bool is_stdout;
        bool fail_dup2;
    } const args[] = {
        {false, false},
        {false, true},
        {true, false},
        {true, true}
    };

    for (auto const& arg : args) {
        vector<testing_redirect_flush_call> flush_calls;
        vector<testing_redirect_dup2_call> dup2_calls;

        auto seq = 0u;

        auto const saved_fd = 11;
        testing_file_desc saved{"saved", saved_fd};

        auto did_throw_exception = false;

        try {
            restore_output(
                arg.is_stdout ? STDOUT_FILENO : STDERR_FILENO,
                saved,
                tst::redirect_output_deps{
                    [&](
                        ostream& os,
                        wostream& wos
                    ) {
                        flush_calls.push_back(testing_redirect_flush_call{++seq, &os, &wos});
                    },
                    nullptr,
                    nullptr,
                    [&](
                        int fd1,
                        int fd2
                    ) {
                        dup2_calls.push_back({++seq, fd1, fd2});
                        return arg.fail_dup2 ? -1 : 0;
                    }
                }
            );
        } catch (exception const& ex) {
            did_throw_exception = true;
            testing_assert(
                string{ex.what()}.find("failed to restore original descriptor") != string::npos
            );
        }

        if (arg.fail_dup2) {
            testing_assert(did_throw_exception);
        }

        testing_assert(flush_calls.size() == 1);
        testing_assert(flush_calls[0].seq == 1);
        testing_assert(flush_calls[0].p_os == (arg.is_stdout ? &cout : &cerr));
        testing_assert(flush_calls[0].p_wos == (arg.is_stdout ? &wcout : &wcerr));

        testing_assert(dup2_calls.size() == 1);
        testing_assert(dup2_calls[0].seq == 2);
        testing_assert(dup2_calls[0].fd1 == saved_fd);
        testing_assert(dup2_calls[0].fd2 == (arg.is_stdout ? STDOUT_FILENO : STDERR_FILENO));

        if (arg.fail_dup2) {
            testing_assert(saved.fd >= saved_fd);  // release should not have been called
        } else {
            testing_assert(saved.fd < 0);
        }
    }
}

struct read_call {
    size_t seq;
    int fd;
    void const* buf;
};

struct write_call {
    size_t seq;
    void const* buf;
};

struct redirect_output_call {
    size_t seq;
    int fd;
    int saved_fd;
    int tmp_fd;
};

struct restore_output_call {
    size_t seq;
    int fd;
};

void test_with_redirection() {
    vector<read_call> read_calls;
    vector<write_call> write_calls;
    vector<redirect_output_call> redirect_calls;
    vector<size_t> create_thread_calls;
    vector<restore_output_call> restore_calls;
    vector<size_t> join_thread_calls;
    size_t ctr[2] = {0};
    size_t seq{};
    auto saved_fd_offset = 100;
    auto tmp_fd_offset = 101;

    auto const res = tst::with_redirection_t<int, testing_file_desc>(
        []{ return 11; },
        {
            [&](
                int fd,
                void* buf,
                size_t buf_len_bytes
            ) {
                read_calls.push_back(read_call{++seq, fd, buf});
                switch (ctr[fd - tmp_fd_offset - 1]++) {
                    case 0:
                        return 13;
                    case 1:
                        return 22;
                    default:
                        return 0;
                }
            },
            [&](
                ostream& os,
                char const* buf,
                streamsize n
            ) -> ostream& {
                os << "... ";
                write_calls.push_back(write_call{++seq, buf});
                return os;
            },
            [&](
                int fd,
                testing_file_desc& out_saved,
                testing_file_desc& out_tmp
            ) {
                redirect_output_call rc{++seq, fd, fd + saved_fd_offset, fd + tmp_fd_offset};
                out_saved.attach("saved", rc.saved_fd);
                out_tmp.attach("tmp", rc.tmp_fd);
                redirect_calls.push_back(rc);
            },
            [&](
                int fd,
                testing_file_desc& saved
            ) {
                restore_calls.push_back(restore_output_call{++seq, saved.fd});
            },
            [&](function<void()> f) {
                create_thread_calls.push_back(++seq);
                f();
                return 1000;
            },
            [&](int) { join_thread_calls.push_back(++seq); }
        }
    );

    testing_assert(res.status == 11);

    testing_assert(redirect_calls.size() == 2);
    testing_assert(redirect_calls[0].seq == 1);
    testing_assert(redirect_calls[0].fd == STDOUT_FILENO);
    testing_assert(redirect_calls[0].saved_fd == STDOUT_FILENO + saved_fd_offset);
    testing_assert(redirect_calls[0].tmp_fd == STDOUT_FILENO + tmp_fd_offset);
    testing_assert(redirect_calls[1].seq == 2);
    testing_assert(redirect_calls[1].fd == STDERR_FILENO);
    testing_assert(redirect_calls[1].saved_fd == STDERR_FILENO + saved_fd_offset);
    testing_assert(redirect_calls[1].tmp_fd == STDERR_FILENO + tmp_fd_offset);

    testing_assert(create_thread_calls.size() == 2);
    testing_assert(create_thread_calls[0] == 3);

    testing_assert(read_calls.size() == 6);
    for (auto i = 0u; i < 3; ++i) {
        testing_assert(read_calls[i].seq == 4 + (i * 2));
        testing_assert(read_calls[i].fd == STDOUT_FILENO + tmp_fd_offset);
    }
    testing_assert(write_calls.size() == 4);
    for (auto i = 0u; i < 2; ++i) {
        testing_assert(write_calls[i].seq == 5 + (i * 2));
        testing_assert(write_calls[i].buf == read_calls[i].buf);
    }

    testing_assert(create_thread_calls[1] == 9);
    for (auto i = 3u; i < read_calls.size(); ++i) {
        testing_assert(read_calls[i].seq == 10 + ((i - 3) * 2));
        testing_assert(read_calls[i].fd == STDERR_FILENO + tmp_fd_offset);
    }
    for (auto i = 2u; i < write_calls.size(); ++i) {
        testing_assert(write_calls[i].seq == 11 + ((i - 2) * 2));
        testing_assert(write_calls[i].buf == read_calls[i].buf);
    }
    testing_assert(join_thread_calls.size() == 2);
    testing_assert(join_thread_calls[0] == 16);
    testing_assert(join_thread_calls[1] == 18);

    testing_assert(restore_calls.size() == 2);
    testing_assert(restore_calls[0].seq == 15);
    testing_assert(restore_calls[0].fd == STDOUT_FILENO + saved_fd_offset);
    testing_assert(restore_calls[1].seq == 17);
    testing_assert(restore_calls[1].fd == STDERR_FILENO + saved_fd_offset);

    testing_assert(res.stdout == TESTING_STR_LIT("... ... "));
    testing_assert(res.stderr == TESTING_STR_LIT("... ... "));
}

void test_check_args() {
    {
        tst::check_args(
            vector<string_type>{
                TESTING_STR_LIT("-mabc,123"),
                TESTING_STR_LIT("--xyz"),
                TESTING_STR_LIT("-n-5,8-19"),
                TESTING_STR_LIT("-prj"),
                TESTING_STR_LIT("-n"),
                TESTING_STR_LIT("1-5,8-6")
            }            
        );
    }

    try {
        tst::check_args(vector<string_type>{TESTING_STR_LIT("-nabc,123")});
        testing_assert(false);
    } catch (exception const& ex) {
        testing_assert(
            string{ex.what()}.find(
                "bad value for argument 'n' or exclusive argument combined with other arguments"
            ) != string::npos
        );
    }

    tst::check_args(vector<string_type>{TESTING_STR_LIT("-mnabc,123")});

    tst::check_args(vector<string_type>{TESTING_STR_LIT("-cehljprt")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-ehljprtc")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-hljprtce")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-ljprtceh")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-jprtcehl")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-prtcehlj")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-rtcehljp")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-tcehljpr")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-cehljprt")});

    tst::check_args(vector<string_type>{TESTING_STR_LIT("-dcehljprt")});
    tst::check_args(vector<string_type>{TESTING_STR_LIT("-ocehljprt")});

    try {
        tst::check_args(vector<string_type>{TESTING_STR_LIT("-w")});
        testing_assert(false);
    } catch (exception const& ex) {
        testing_assert(string{ex.what()}.find("unrecognized argument 'w'") != string::npos);
    }

    try {
        tst::check_args(vector<string_type>{TESTING_STR_LIT("-cw")});
        testing_assert(false);
    } catch (exception const& ex) {
        testing_assert(
            string{ex.what()}.find(
                "unrecognized argument 'w' or exclusive argument 'c' combined with other arguments"
            ) != string::npos
        );
    }
}

struct list_opts {
    bool use_color;
    bool use_emojis_short;
    bool use_emojis_long;
    bool extra_args;
};

void test_main() {
    static char_type const prog_name[] = TESTING_STR_LIT("prog-name");

    auto& os = TUXX_REPORT_OSTRM;

    {
        test_main_ctx t_ctx{"no arguments provided", vector<string_type>{}};
        os << t_ctx << endl;
        auto const res = t_ctx.run_main();
        testing_assert(res);
        testing_assert(t_ctx.out_os.str().empty());
        testing_assert(
            t_ctx.err_os.str().find(
                TESTING_STR_LIT("ERROR: at least one argument ")
            ) != string_type::npos
        );
        testing_assert(t_ctx.get_arg_value_calls.empty());
        testing_assert(t_ctx.get_hw_concurrency_calls == 0);
        testing_assert(t_ctx.is_arg_specified_calls.empty());
        testing_assert(t_ctx.is_atty_calls.empty());
        testing_assert(t_ctx.list_tests_calls.empty());
        testing_assert(t_ctx.make_delim_reporter_calls.empty());
        testing_assert(t_ctx.make_json_reporter_calls.empty());
        testing_assert(t_ctx.make_junit_reporter_calls.empty());
        testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
        testing_assert(t_ctx.with_redirection_calls == 0);
        testing_assert(t_ctx.run_test_cases_calls.empty());
        testing_assert(t_ctx.reporter_finish_calls.empty());
    }

    {
        test_main_ctx t_ctx{"check_args throws", vector<string_type>{prog_name}};
        t_ctx.deps.check_args_fn = [](vector<string_type> const&) {
            throw invalid_argument{"--123--"};
        };
        os << t_ctx << endl;
        auto const res = t_ctx.run_main();
        testing_assert(res);
        testing_assert(t_ctx.out_os.str().empty());
        testing_assert(t_ctx.err_os.str() == TESTING_STR_LIT("ERROR: --123--\n"));
        testing_assert(t_ctx.usage_calls.size() == 1);
        testing_assert(t_ctx.usage_calls[0].prog_name == prog_name);
        testing_assert(t_ctx.usage_calls[0].p_os == &t_ctx.out_os);
        testing_assert(t_ctx.get_arg_value_calls.empty());
        testing_assert(t_ctx.is_arg_specified_calls.empty());
        testing_assert(t_ctx.list_tests_calls.empty());
        testing_assert(t_ctx.get_hw_concurrency_calls == 0);
        testing_assert(t_ctx.is_atty_calls.empty());
        testing_assert(t_ctx.make_delim_reporter_calls.empty());
        testing_assert(t_ctx.make_json_reporter_calls.empty());
        testing_assert(t_ctx.make_junit_reporter_calls.empty());
        testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
        testing_assert(t_ctx.with_redirection_calls == 0);
        testing_assert(t_ctx.run_test_cases_calls.empty());
        testing_assert(t_ctx.reporter_finish_calls.empty());
    }

    {
        // Invalid 'm'|'match' arguments
        static char_type const* test_args[] = {TESTING_STR_LIT("-m"), TESTING_STR_LIT("--match")};
        for (auto const& test_arg : test_args) {
            test_main_ctx t_ctx{"match", vector<string_type>{prog_name, test_arg}};
            os << t_ctx << endl;
            auto const res = t_ctx.run_main();
            testing_assert(res);
            testing_assert(t_ctx.out_os.str().empty());
            testing_assert(
                t_ctx.err_os.str() 
                    == TESTING_STR_LIT("ERROR: value required for '-m|--match' argument\n")
            );
            testing_assert(t_ctx.get_arg_value_calls.size() == 1);
            auto const p_match_call = t_ctx.get_get_arg_value_call('m', "match");
            testing_assert(p_match_call);
            testing_assert(p_match_call->p_args == &t_ctx.args);
            testing_assert(t_ctx.usage_calls.size() == 1);
            testing_assert(t_ctx.usage_calls[0].prog_name == prog_name);
            testing_assert(t_ctx.usage_calls[0].p_os == &t_ctx.out_os);
            testing_assert(t_ctx.args.empty());

            testing_assert(t_ctx.is_arg_specified_calls.empty());
            testing_assert(t_ctx.list_tests_calls.empty());
            testing_assert(t_ctx.get_hw_concurrency_calls == 0);
            testing_assert(t_ctx.is_atty_calls.empty());
            testing_assert(t_ctx.make_delim_reporter_calls.empty());
            testing_assert(t_ctx.make_json_reporter_calls.empty());
            testing_assert(t_ctx.make_junit_reporter_calls.empty());
            testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
            testing_assert(t_ctx.with_redirection_calls == 0);
            testing_assert(t_ctx.run_test_cases_calls.empty());
            testing_assert(t_ctx.reporter_finish_calls.empty());
        }
    }

    {
        static char_type const* test_args[] = {TESTING_STR_LIT("-h"), TESTING_STR_LIT("--help")};
        for (auto const& test_arg : test_args) {
            test_main_ctx t_ctx{"help args", vector<string_type>{prog_name, test_arg}};
            os << t_ctx << endl;
            t_ctx.run_main();
            testing_assert(t_ctx.out_os.str().empty());

            // Needs to be checked even before -h|--help
            auto const p_match_call = t_ctx.get_get_arg_value_call('m', "match");
            testing_assert(p_match_call);
            testing_assert(p_match_call->p_args == &t_ctx.args);

            auto const p_help_call = t_ctx.get_is_arg_specified_call('h', "help");
            testing_assert(p_help_call);
            testing_assert(p_help_call->p_args == &t_ctx.args);
            testing_assert(t_ctx.usage_calls.size() == 1);
            testing_assert(t_ctx.usage_calls[0].prog_name == prog_name);
            testing_assert(t_ctx.usage_calls[0].p_os == &t_ctx.out_os);
            testing_assert(t_ctx.args.empty());

            testing_assert(!t_ctx.get_is_arg_specified_call('l', "list"));
            testing_assert(!t_ctx.get_get_arg_value_call('d', "delim"));
            testing_assert(!t_ctx.get_is_arg_specified_call('j', "json"));
            testing_assert(!t_ctx.get_is_arg_specified_call('j', "json"));
            testing_assert(!t_ctx.get_get_arg_value_call(0, "junit"));
            testing_assert(!t_ctx.get_is_arg_specified_call('r', "randomized"));
            testing_assert(!t_ctx.get_is_arg_specified_call('c', "capture"));
            testing_assert(!t_ctx.get_get_arg_value_call('p', "parallel"));
            testing_assert(!t_ctx.get_get_arg_value_call('n', "number"));
            testing_assert(!t_ctx.get_is_arg_specified_call('t', "text"));
            testing_assert(t_ctx.list_tests_calls.empty());
            testing_assert(t_ctx.get_hw_concurrency_calls == 0);
            testing_assert(t_ctx.is_atty_calls.empty());
            testing_assert(t_ctx.make_delim_reporter_calls.empty());
            testing_assert(t_ctx.make_json_reporter_calls.empty());
            testing_assert(t_ctx.make_junit_reporter_calls.empty());
            testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
            testing_assert(t_ctx.make_custom_reporter_calls.empty());
            testing_assert(t_ctx.with_redirection_calls == 0);
            testing_assert(t_ctx.run_test_cases_calls.empty());
            testing_assert(t_ctx.reporter_finish_calls.empty());
        }
    }

    {
        static char_type const* test_args[] = {TESTING_STR_LIT("-l"), TESTING_STR_LIT("--list")};
        vector<list_opts> opts;
        for (auto idx_c = 0u; idx_c < 2; ++idx_c) {
            for (auto idx_es = 0u; idx_es < 2; ++idx_es) {
                for (auto idx_el = 0u; idx_el < 2; ++idx_el) {
                    for (auto idx_ea = 0u; idx_ea < 2; ++idx_ea) {
                        opts.push_back(list_opts{!!idx_c, !!idx_es, !!idx_el, !!idx_ea});
                    }
                }
            }
        }
        for (auto const& opt : opts) {
            for (auto const& test_arg : test_args) {
                vector<string_type> args{prog_name, test_arg};
                if (opt.extra_args) {
                    args.push_back(TESTING_STR_LIT("-j"));
                    args.push_back(TESTING_STR_LIT("--parallel"));
                }
                if (!opt.use_color) {
                    args.push_back(TESTING_STR_LIT("--no-color"));
                }
                if (opt.use_emojis_short) {
                    args.push_back(TESTING_STR_LIT("-e"));
                }
                if (opt.use_emojis_long) {
                    args.push_back(TESTING_STR_LIT("--emojis"));
                }
                test_main_ctx t_ctx{"list args", args};
                os << t_ctx << endl;
                auto const res = t_ctx.run_main();
                testing_assert(res == 0);
                testing_assert(t_ctx.out_os.str().empty());
                auto const p_match_call = t_ctx.get_get_arg_value_call('m', "match");
                testing_assert(p_match_call);
                testing_assert(p_match_call->p_args == &t_ctx.args);
                auto const p_help_call = t_ctx.get_is_arg_specified_call('h', "help");
                testing_assert(p_help_call);
                testing_assert(p_help_call->p_args == &t_ctx.args);
                auto const p_no_color_call = t_ctx.get_is_arg_specified_call(0, "no-color");
                testing_assert(p_no_color_call);
                testing_assert(p_no_color_call->p_args == &t_ctx.args);
                auto const p_emojis_call = t_ctx.get_is_arg_specified_call('e', "emojis");
                testing_assert(p_emojis_call);
                testing_assert(p_emojis_call->p_args == &t_ctx.args);
                auto const p_list_call = t_ctx.get_is_arg_specified_call('l', "list");
                testing_assert(p_list_call);
                testing_assert(p_list_call->p_args == &t_ctx.args);
                testing_assert(t_ctx.is_atty_calls.size() == 1);
                testing_assert(t_ctx.is_atty_calls[0] == STDOUT_FILENO);
                testing_assert(t_ctx.is_arg_specified_calls.back().p_args == &t_ctx.args);
                testing_assert(t_ctx.is_arg_specified_calls.back().short_arg == 'l');
                testing_assert(strcmp(t_ctx.is_arg_specified_calls.back().long_arg, "list") == 0);
                testing_assert(t_ctx.usage_calls.empty());
                if (opt.extra_args) {
                    testing_assert(
                        t_ctx.err_os.str().find(
                            TESTING_STR_LIT("WARNING: ignoring argument(s) '-j'")
                        ) != string_type::npos
                    );
                    testing_assert(
                        t_ctx.err_os.str().find(
                            TESTING_STR_LIT("WARNING: ignoring argument(s) '--parallel'")
                        ) != string_type::npos
                    );
                }
                testing_assert(!t_ctx.get_get_arg_value_call('d', "delim"));
                testing_assert(!t_ctx.get_is_arg_specified_call('j', "json"));
                testing_assert(!t_ctx.get_is_arg_specified_call('j', "json"));
                testing_assert(!t_ctx.get_get_arg_value_call(0, "junit"));
                testing_assert(!t_ctx.get_is_arg_specified_call('r', "randomized"));
                testing_assert(!t_ctx.get_is_arg_specified_call('c', "capture"));
                testing_assert(!t_ctx.get_get_arg_value_call('p', "parallel"));
                testing_assert(!t_ctx.get_get_arg_value_call('n', "number"));
                testing_assert(!t_ctx.get_is_arg_specified_call('t', "text"));
                testing_assert(t_ctx.list_tests_calls.size() == 1);
                testing_assert(t_ctx.list_tests_calls[0].p_tests == &t_ctx.tests);
                testing_assert(t_ctx.list_tests_calls[0].p_os == &t_ctx.out_os);
                testing_assert(t_ctx.list_tests_calls[0].use_color == opt.use_color);
                testing_assert(
                    t_ctx.list_tests_calls[0].use_emojis == (
                        opt.use_emojis_short || opt.use_emojis_long
                    )
                );
                testing_assert(t_ctx.get_hw_concurrency_calls == 0);
                testing_assert(t_ctx.is_atty_calls.size() == 1);
                testing_assert(t_ctx.is_atty_calls[0] == STDOUT_FILENO);
                testing_assert(t_ctx.make_delim_reporter_calls.empty());
                testing_assert(t_ctx.make_json_reporter_calls.empty());
                testing_assert(t_ctx.make_junit_reporter_calls.empty());
                testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                testing_assert(t_ctx.make_custom_reporter_calls.empty());
                testing_assert(t_ctx.with_redirection_calls == 0);
                testing_assert(t_ctx.run_test_cases_calls.empty());
                testing_assert(t_ctx.reporter_finish_calls.empty());
            }
        }
    }

    {
        static char_type const* text_args[] = {TESTING_STR_LIT("-t"), TESTING_STR_LIT("--text")};
        static char_type const* rep_args[] = {
            TESTING_STR_LIT("-j"),
            TESTING_STR_LIT("--json"),
            TESTING_STR_LIT("--junit"),
            TESTING_STR_LIT("-d"),
            TESTING_STR_LIT("--delim")
        };
        for (auto const text_arg : text_args) {
            for (auto const rep_arg : rep_args) {
                test_main_ctx t_ctx{
                    string("force default reporter conflicts [") + simple_narrow(rep_arg) + ']',
                    vector<string_type>{prog_name, text_arg, rep_arg}
                };
                os << t_ctx << endl;
                auto const res = t_ctx.run_main();
                testing_assert(res);
                testing_assert(t_ctx.usage_calls.size() == 1);
                testing_assert(t_ctx.list_tests_calls.empty());
                testing_assert(t_ctx.get_hw_concurrency_calls == 0);
                testing_assert(t_ctx.make_delim_reporter_calls.empty());
                testing_assert(t_ctx.make_json_reporter_calls.empty());
                testing_assert(t_ctx.make_junit_reporter_calls.empty());
                testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                testing_assert(t_ctx.make_custom_reporter_calls.empty());
                testing_assert(t_ctx.with_redirection_calls == 0);
                testing_assert(t_ctx.run_test_cases_calls.empty());
                testing_assert(t_ctx.reporter_finish_calls.empty());
                testing_assert(
                    t_ctx.err_os.str().find(
                        TESTING_STR_LIT("ERROR: cannot specify both the default and")
                    ) == 0
                );
            }
        }
    }

    {
        static char_type const* text_args[] = {TESTING_STR_LIT("-j"), TESTING_STR_LIT("--json")};
        static char_type const* rep_args[] = {
            TESTING_STR_LIT("--junit"),
            TESTING_STR_LIT("-d"),
            TESTING_STR_LIT("--delim")
        };
        for (auto const text_arg : text_args) {
            for (auto const rep_arg : rep_args) {
                test_main_ctx t_ctx{
                    string("json reporter conflicts [") + simple_narrow(rep_arg) + ']',
                    vector<string_type>{prog_name, text_arg, rep_arg}
                };
                os << t_ctx << endl;
                auto const res = t_ctx.run_main();
                testing_assert(res);
                testing_assert(t_ctx.usage_calls.size() == 1);
                testing_assert(t_ctx.list_tests_calls.empty());
                testing_assert(t_ctx.get_hw_concurrency_calls == 0);
                testing_assert(t_ctx.make_delim_reporter_calls.empty());
                testing_assert(t_ctx.make_json_reporter_calls.empty());
                testing_assert(t_ctx.make_junit_reporter_calls.empty());
                testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                testing_assert(t_ctx.make_custom_reporter_calls.empty());
                testing_assert(t_ctx.with_redirection_calls == 0);
                testing_assert(t_ctx.run_test_cases_calls.empty());
                testing_assert(t_ctx.reporter_finish_calls.empty());
                testing_assert(
                    t_ctx.err_os.str().find(
                        TESTING_STR_LIT("ERROR: cannot specify both json")
                    ) == 0
                );
            }
        }
    }

    {
        static char_type const* rep_args[] = {
            TESTING_STR_LIT("-d"),
            TESTING_STR_LIT("--delim")
        };
        for (auto const rep_arg : rep_args) {
            test_main_ctx t_ctx{
                string("--junit reporter conflicts [") + simple_narrow(rep_arg) + ']',
                vector<string_type>{prog_name, TESTING_STR_LIT("--junit"), rep_arg}
            };
            os << t_ctx << endl;
            auto const res = t_ctx.run_main();
            testing_assert(res);
            testing_assert(t_ctx.usage_calls.size() == 1);
            testing_assert(t_ctx.list_tests_calls.empty());
            testing_assert(t_ctx.get_hw_concurrency_calls == 0);
            testing_assert(t_ctx.make_delim_reporter_calls.empty());
            testing_assert(t_ctx.make_json_reporter_calls.empty());
            testing_assert(t_ctx.make_junit_reporter_calls.empty());
            testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
            testing_assert(t_ctx.make_custom_reporter_calls.empty());
            testing_assert(t_ctx.with_redirection_calls == 0);
            testing_assert(t_ctx.run_test_cases_calls.empty());
            testing_assert(t_ctx.reporter_finish_calls.empty());
            testing_assert(
                t_ctx.err_os.str().find(
                    TESTING_STR_LIT("ERROR: cannot specify both junit")
                ) == 0
            );
        }
    }

    {
        static char_type const* delim_args[] = {
            TESTING_STR_LIT("-d"),
            TESTING_STR_LIT("--delim")
        };
        for (auto const delim_arg : delim_args) {
            {
                test_main_ctx t_ctx{
                    "missing delim argument",
                    vector<string_type>{prog_name, delim_arg}
                };
                os << t_ctx << endl;
                auto const res = t_ctx.run_main();
                testing_assert(res);
                testing_assert(t_ctx.usage_calls.size() == 1);
                testing_assert(t_ctx.list_tests_calls.empty());
                testing_assert(t_ctx.get_hw_concurrency_calls == 0);
                testing_assert(t_ctx.make_delim_reporter_calls.empty());
                testing_assert(t_ctx.make_json_reporter_calls.empty());
                testing_assert(t_ctx.make_junit_reporter_calls.empty());
                testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                testing_assert(t_ctx.make_custom_reporter_calls.empty());
                testing_assert(t_ctx.with_redirection_calls == 0);
                testing_assert(t_ctx.run_test_cases_calls.empty());
                testing_assert(t_ctx.reporter_finish_calls.empty());
                testing_assert(
                    t_ctx.err_os.str().find(
                        TESTING_STR_LIT("ERROR: delimiter character must be specified")
                    ) == 0
                );
            }

            {
                test_main_ctx t_ctx{
                    "bad delim argument",
                    vector<string_type>{prog_name, delim_arg, TESTING_STR_LIT("xyz")}
                };
                os << t_ctx << endl;
                auto const res = t_ctx.run_main();
                testing_assert(res);
                testing_assert(t_ctx.usage_calls.size() == 1);
                testing_assert(t_ctx.list_tests_calls.empty());
                testing_assert(t_ctx.get_hw_concurrency_calls == 0);
                testing_assert(t_ctx.make_delim_reporter_calls.empty());
                testing_assert(t_ctx.make_json_reporter_calls.empty());
                testing_assert(t_ctx.make_junit_reporter_calls.empty());
                testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                testing_assert(t_ctx.make_custom_reporter_calls.empty());
                testing_assert(t_ctx.with_redirection_calls == 0);
                testing_assert(t_ctx.run_test_cases_calls.empty());
                testing_assert(t_ctx.reporter_finish_calls.empty());
                testing_assert(
                    t_ctx.err_os.str().find(
                        TESTING_STR_LIT("ERROR: -d|--delim delimiter is invalid")
                    ) == 0
                );
            }
        }
    }

    {
        static char_type const* rep_args[] = {
            TESTING_STR_LIT("-p"),
            TESTING_STR_LIT("--parallel")
        };
        for (auto const rep_arg : rep_args) {
            test_main_ctx t_ctx{
                "bad parallel argument",
                vector<string_type>{prog_name, rep_arg, TESTING_STR_LIT("xyz")}
            };
            os << t_ctx << endl;
            auto const res = t_ctx.run_main();
            testing_assert(res);
            testing_assert(t_ctx.usage_calls.size() == 1);
            testing_assert(t_ctx.list_tests_calls.empty());
            testing_assert(t_ctx.get_hw_concurrency_calls == 0);
            testing_assert(t_ctx.make_delim_reporter_calls.empty());
            testing_assert(t_ctx.make_json_reporter_calls.empty());
            testing_assert(t_ctx.make_junit_reporter_calls.empty());
            testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
            testing_assert(t_ctx.make_custom_reporter_calls.empty());
            testing_assert(t_ctx.with_redirection_calls == 0);
            testing_assert(t_ctx.run_test_cases_calls.empty());
            testing_assert(t_ctx.reporter_finish_calls.empty());
            testing_assert(
                t_ctx.err_os.str().find(
                    TESTING_STR_LIT("ERROR: a numeric value must be provided")
                ) == 0
            );
        }
    }

    {
        static char_type const* rep_args[] = {
            TESTING_STR_LIT("-n"),
            TESTING_STR_LIT("--number")
        };
        for (auto const rep_arg : rep_args) {
            test_main_ctx t_ctx{
                "missing number argument",
                vector<string_type>{prog_name, rep_arg}
            };
            os << t_ctx << endl;
            auto const res = t_ctx.run_main();
            testing_assert(res);
            testing_assert(t_ctx.usage_calls.size() == 1);
            testing_assert(t_ctx.list_tests_calls.empty());
            testing_assert(t_ctx.get_hw_concurrency_calls == 0);
            testing_assert(t_ctx.make_delim_reporter_calls.empty());
            testing_assert(t_ctx.make_json_reporter_calls.empty());
            testing_assert(t_ctx.make_junit_reporter_calls.empty());
            testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
            testing_assert(t_ctx.make_custom_reporter_calls.empty());
            testing_assert(t_ctx.with_redirection_calls == 0);
            testing_assert(t_ctx.run_test_cases_calls.empty());
            testing_assert(t_ctx.reporter_finish_calls.empty());
            testing_assert(
                t_ctx.err_os.str().find(
                    TESTING_STR_LIT("ERROR: value required for '-n|--number'")
                ) == 0
            );
        }
    }

    {
        static char_type const* rep_args[] = {
            TESTING_STR_LIT("-m"),
            TESTING_STR_LIT("--match")
        };
        for (auto const rep_arg : rep_args) {
            test_main_ctx t_ctx{
                "missing match argument",
                vector<string_type>{prog_name, rep_arg}
            };
            os << t_ctx << endl;
            auto const res = t_ctx.run_main();
            testing_assert(res);
            testing_assert(t_ctx.usage_calls.size() == 1);
            testing_assert(t_ctx.list_tests_calls.empty());
            testing_assert(t_ctx.get_hw_concurrency_calls == 0);
            testing_assert(t_ctx.make_delim_reporter_calls.empty());
            testing_assert(t_ctx.make_json_reporter_calls.empty());
            testing_assert(t_ctx.make_junit_reporter_calls.empty());
            testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
            testing_assert(t_ctx.make_custom_reporter_calls.empty());
            testing_assert(t_ctx.with_redirection_calls == 0);
            testing_assert(t_ctx.run_test_cases_calls.empty());
            testing_assert(t_ctx.reporter_finish_calls.empty());
            testing_assert(
                t_ctx.err_os.str().find(
                    TESTING_STR_LIT("ERROR: value required for '-m|--match'")
                ) == 0
            );
        }
    }

    string const fn_name{__FUNCTION__};

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

    {
        static struct {
            size_t low_1;
            size_t high_1;
            size_t low_2;
            size_t high_2;

            bool is_test_in_range(size_t id) const {
                return ((id >= low_1) && (id <= high_1)) || ((id >= low_2) && (id <= high_2));
            }
        } const num_ranges[] = {
            {0, numeric_limits<size_t>::max(), 0, numeric_limits<size_t>::max()},
            {4, 4, 0, numeric_limits<size_t>::max()},
            {0, 4, 0, numeric_limits<size_t>::max()},
            {3, numeric_limits<size_t>::max(), 0, numeric_limits<size_t>::max()},
            {0, 4, 6, numeric_limits<size_t>::max()},
            {2, 4, 8, 12},
            {3, 6, 10, numeric_limits<size_t>::max()},
            {3, 6, 10, 10}
        };

        static char_type const* n_args[] = {
            TESTING_STR_LIT("-n"),
            TESTING_STR_LIT("--number"),
            TESTING_STR_LIT("-n#")     // '#' indicates arg value goes with the -n.
        };
        for (auto const n_arg : n_args) {
            for (auto const n_range : num_ranges) {
                string_type n_arg_switch = n_arg;
                string_type n_arg_val;
                if (n_range.low_1) {
                    ostringstream_type oss;
                    oss << n_range.low_1;
                    n_arg_val += oss.str();
                }
                n_arg_val += '-';
                if (n_range.high_1 != numeric_limits<size_t>::max()) {
                    ostringstream_type oss;
                    oss << n_range.high_1;
                    n_arg_val += oss.str();
                }
                if (n_range.low_2 || (n_range.high_2 != numeric_limits<size_t>::max())) {
                    n_arg_val += ',';
                    if (n_range.low_2) {
                        ostringstream_type oss;
                        oss << n_range.low_2;
                        n_arg_val += oss.str();
                    }
                    n_arg_val += '-';
                    if (n_range.high_2 != numeric_limits<size_t>::max()) {
                        ostringstream_type oss;
                        oss << n_range.high_2;
                        n_arg_val += oss.str();
                    }
                }
                vector<string_type> prog_args{prog_name};
                string test_name = "numeric filters [";
                if (n_arg_switch[n_arg_switch.length() - 1] == '#') {
                    prog_args.push_back(
                        n_arg_switch.substr(0, n_arg_switch.length() - 1) + n_arg_val
                    );
                    test_name += simple_narrow(prog_args.back()) + ']';
                } else {
                    test_name += simple_narrow(n_arg_switch + TESTING_STR_LIT(' ') + n_arg_val)
                        + ']';
                    prog_args.push_back(n_arg_switch);
                    prog_args.push_back(n_arg_val);
                }
                test_main_ctx t_ctx{test_name, prog_args};
                t_ctx.tests = tests;
                os << t_ctx << endl;
                int res{};
                try {
                    res = t_ctx.run_main();
                    testing_assert(n_arg_val != TESTING_STR_LIT("-"));
                } catch (exception const& ex) {
                    testing_assert(n_arg_val == TESTING_STR_LIT("-"));
                    continue;
                }
                testing_assert(res == 0);
                // Only tests that match the specified range should remain...
                for (auto const& tst : t_ctx.tests) {
                    testing_assert(n_range.is_test_in_range(tst.id));
                }
            }
        }
    }

    {
        static char_type const* m_args[] = {
            TESTING_STR_LIT("-m"),
            TESTING_STR_LIT("--match"),
            TESTING_STR_LIT("-m#")     // '#' indicates arg value goes with the -n.
        };
        static struct {
            string_type prefix_1;
            string_type prefix_2;
            bool does_test_case_match(string_type const& name) const {
                return (name.find(prefix_1) == 0) || (
                    !prefix_2.empty() && (name.find(prefix_2) == 0)
                );
            }
        } const matches[] = {
            {TESTING_STR_LIT("test-")},
            {TESTING_STR_LIT("a-")},
            {TESTING_STR_LIT("b-")},
            {TESTING_STR_LIT("test-"), TESTING_STR_LIT("a-")},
            {TESTING_STR_LIT("xyz-"), TESTING_STR_LIT("b-")}
        };
        for (auto const& m_arg : m_args) {
            for (auto const& match : matches) {
                string_type m_arg_switch = m_arg;
                string_type m_arg_val = match.prefix_1;
                if (!match.prefix_2.empty()) {
                    m_arg_val += TESTING_STR_LIT(',') + match.prefix_2;
                }
                vector<string_type> prog_args{prog_name};
                string test_name = "prefix match filters [";
                if (m_arg_switch[m_arg_switch.length() - 1] == '#') {
                    prog_args.push_back(
                        m_arg_switch.substr(0, m_arg_switch.length() - 1) + m_arg_val
                    );
                    test_name += simple_narrow(prog_args.back()) + ']';
                } else {
                    test_name += simple_narrow(m_arg_switch + TESTING_STR_LIT(' ') + m_arg_val)
                        + ']';
                    prog_args.push_back(m_arg_switch);
                    prog_args.push_back(m_arg_val);
                }
                test_main_ctx t_ctx{test_name, prog_args};
                t_ctx.tests = tests;
                os << t_ctx << endl;
                auto const res = t_ctx.run_main();
                testing_assert(res == 0);
                // Only tests that match the specified range should remain...
                for (auto const& tst : t_ctx.tests) {
                    testing_assert(match.does_test_case_match(tst.name));
                }
            }
        }
    }

    {
        // Several generalized arguments tests...
        test_main_args::combinations(
            [&](test_main_args const& arg) {
                static struct {
                    int run_test_cases_res;
                    bool is_atty_res;
                    size_t hw_concurrency;
                } const opts[] = {
                    {0, false, 1},
                    {0, false, 1},
                    {0, false, 8},
                    {0, true, 1},
                    {0, true, 8},
                    {1, false, 1},
                    {1, false, 8},
                    {1, true, 1},
                    {1, true, 8}
                };

                auto const is_randomized = arg.randomized_long || arg.randomized_short;

                for (auto const& opt : opts) {
                    auto const concurrency_req = (arg.parallel_long || arg.parallel_short)
                        ? (arg.parallel_arg ? test_main_args::parallel_value : opt.hw_concurrency)
                        : 1;

                    {
                        test_main_ctx t_ctx{
                            fn_name + " - default reporter",
                            arg,
                            opt.run_test_cases_res,
                            opt.is_atty_res,
                            opt.hw_concurrency,
                            vector<string_type>{prog_name}
                        };
                        t_ctx.tests = tests;
                        os << t_ctx << endl;
                        auto const res = t_ctx.run_main();
                        auto const did_capture_output = arg.capture_long || arg.capture_short;
                        t_ctx.common_reporter_asserts(
                            res,
                            concurrency_req,
                            is_randomized,
                            did_capture_output
                        );
                        testing_assert(t_ctx.make_plain_text_reporter_calls.size() == 1);
                        auto const& call = t_ctx.make_plain_text_reporter_calls[0];
                        if (did_capture_output) {
                            testing_assert(call.p_os != &t_ctx.out_os);
                        } else {
                            testing_assert(call.p_os == &t_ctx.out_os);
                        }
                        testing_assert(call.p_tests == &t_ctx.tests);
                        testing_assert(call.use_color == (opt.is_atty_res && arg.use_color));
                        testing_assert(call.use_emojis == (arg.emojis_short || arg.emojis_long));
                        testing_assert(t_ctx.make_json_reporter_calls.empty());
                        testing_assert(t_ctx.make_junit_reporter_calls.empty());
                        testing_assert(t_ctx.make_delim_reporter_calls.empty());
                        testing_assert(t_ctx.make_custom_reporter_calls.empty());
                    }

                    // We always capture output for the rest of the built in reporters.

                    static struct {
                        char const* desc;
                        char_type const* arg;
                    } const json_args[] = {
                        {"json reporter (short)", TESTING_STR_LIT("-j")},
                        {"json reporter (long)", TESTING_STR_LIT("--json")},
                    };

                    for (auto const json_arg : json_args) {
                        test_main_ctx t_ctx{
                            fn_name + " - " + json_arg.desc,
                            arg,
                            opt.run_test_cases_res,
                            opt.is_atty_res,
                            opt.hw_concurrency,
                            vector<string_type>{prog_name, json_arg.arg}
                        };
                        t_ctx.tests = tests;
                        os << t_ctx << endl;
                        auto const res = t_ctx.run_main();
                        t_ctx.common_reporter_asserts(res, concurrency_req, is_randomized);
                        testing_assert(t_ctx.make_json_reporter_calls.size() == 1);
                        auto const& call = t_ctx.make_json_reporter_calls[0];
                        testing_assert(call.p_os != &t_ctx.out_os);
                        testing_assert(call.n_tests == t_ctx.tests.size());
                        testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                        testing_assert(t_ctx.make_junit_reporter_calls.empty());
                        testing_assert(t_ctx.make_delim_reporter_calls.empty());
                        testing_assert(t_ctx.make_custom_reporter_calls.empty());
                    }

                    static char_type const report_name[] = TESTING_STR_LIT("test-report");
                    static struct {
                        char const* desc;
                        char_type const* report_name;
                    } const junit_args[] = {
                        {"junit reporter (no report name)", TESTING_STR_LIT("")},
                        {"junit reporter (report name)", report_name},
                    };

                    for (auto const& junit_arg : junit_args) {
                        vector<string_type> prog_args{prog_name, TESTING_STR_LIT("--junit")};
                        if (junit_arg.report_name[0]) {
                            prog_args.push_back(junit_arg.report_name);
                        }
                        test_main_ctx t_ctx{
                            fn_name + " - " + junit_arg.desc,
                            arg,
                            opt.run_test_cases_res,
                            opt.is_atty_res,
                            opt.hw_concurrency,
                            prog_args
                        };
                        t_ctx.tests = tests;
                        os << t_ctx << endl;
                        auto const res = t_ctx.run_main();
                        t_ctx.common_reporter_asserts(res, concurrency_req, is_randomized);
                        testing_assert(t_ctx.make_junit_reporter_calls.size() == 1);
                        auto const& call = t_ctx.make_junit_reporter_calls[0];
                        testing_assert(call.p_os != &t_ctx.out_os);
                        testing_assert(call.prog_name == prog_name);
                        testing_assert(call.n_tests == t_ctx.tests.size());
                        testing_assert(call.test_report_name == junit_arg.report_name);
                        testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                        testing_assert(t_ctx.make_json_reporter_calls.empty());
                        testing_assert(t_ctx.make_delim_reporter_calls.empty());
                        testing_assert(t_ctx.make_custom_reporter_calls.empty());
                    }

                    static struct {
                        char const* desc;
                        char_type const* arg;
                        bool is_single_arg;
                    } const delim_args[] = {
                        {"deliminated reporter (short arg)", TESTING_STR_LIT("-d"), false},
                        {"deliminated reporter (short arg, same)", TESTING_STR_LIT("-d:"), true},
                        {"deliminated reporter (long arg)", TESTING_STR_LIT("--delim"), false},
                    };  

                    for (auto const& delim_arg : delim_args) {
                        vector<string_type> prog_args{prog_name, delim_arg.arg};
                        if (!delim_arg.is_single_arg) {
                            prog_args.push_back(TESTING_STR_LIT(":"));
                        }
                        test_main_ctx t_ctx{
                            fn_name + " - " + delim_arg.desc,
                            arg,
                            opt.run_test_cases_res,
                            opt.is_atty_res,
                            opt.hw_concurrency,
                            prog_args
                        };
                        t_ctx.tests = tests;
                        os << t_ctx << endl;
                        auto const res = t_ctx.run_main();
                        t_ctx.common_reporter_asserts(res, concurrency_req, is_randomized);
                        testing_assert(t_ctx.make_delim_reporter_calls.size() == 1);
                        auto const& call = t_ctx.make_delim_reporter_calls[0];
                        testing_assert(call.p_os != &t_ctx.out_os);
                        testing_assert(call.delim == ':');
                        testing_assert(t_ctx.make_plain_text_reporter_calls.empty());
                        testing_assert(t_ctx.make_json_reporter_calls.empty());
                        testing_assert(t_ctx.make_junit_reporter_calls.empty());
                        testing_assert(t_ctx.make_custom_reporter_calls.empty());
                    }
                }
            }
        );
    }
}

struct main_call {
    vector<string_type> const* p_args;
    vector<test_case_instance> const* p_tests;
    report_ostream_type const* p_out_os;
    report_ostream_type const* p_err_os;
};

void test_wrap_main() {
    static char_type const prog_name[] = TESTING_STR_LIT("prog-name");
    vector<string_type> args{prog_name};
    vector<test_case_instance> tests;
    ostringstream_type out_os;
    ostringstream_type err_os;

    vector<main_call> main_calls;
    auto main_ret = 0;
    auto throw_std_exception = false;
    auto throw_unknown = false;

    auto const main_fn = [&](
        vector<string_type>& args,
        vector<test_case_instance>& tests,
        report_ostream_type& out_strm,
        report_ostream_type& err_strm
    ) {
        main_calls.push_back(main_call{&args, &tests, &out_os, &err_os});
        if (throw_std_exception) {
            throw runtime_error{"abcdefg"};
        }
        if (throw_unknown) {
            throw 5;
        }
        return main_ret;
    };

    {
        auto const res = tst::wrap_main(args, tests, out_os, err_os, main_fn);
        testing_assert(res == 0);
        testing_assert(main_calls.size() == 1);
        auto const& call = main_calls[0];
        testing_assert(call.p_args == &args);
        testing_assert(call.p_tests == &tests);
        testing_assert(call.p_out_os == &out_os);
        testing_assert(call.p_err_os == &err_os);
        testing_assert(out_os.str().empty());
        testing_assert(err_os.str().empty());
    }

    main_calls.clear();
    out_os.str(string_type{});
    err_os.str(string_type{});

    {
        main_ret = 7;
        auto const res = tst::wrap_main(args, tests, out_os, err_os, main_fn);
        testing_assert(res == 7);
        testing_assert(main_calls.size() == 1);
        auto const& call = main_calls[0];
        testing_assert(call.p_args == &args);
        testing_assert(call.p_tests == &tests);
        testing_assert(call.p_out_os == &out_os);
        testing_assert(call.p_err_os == &err_os);
        testing_assert(out_os.str().empty());
        testing_assert(err_os.str().empty());
    }

    main_ret = 0;
    main_calls.clear();
    out_os.str(string_type{});
    err_os.str(string_type{});

    {
        throw_std_exception = true;
        auto const res = tst::wrap_main(args, tests, out_os, err_os, main_fn);
        testing_assert(res);
        testing_assert(main_calls.size() == 1);
        auto const& call = main_calls[0];
        testing_assert(call.p_args == &args);
        testing_assert(call.p_tests == &tests);
        testing_assert(call.p_out_os == &out_os);
        testing_assert(call.p_err_os == &err_os);
        testing_assert(out_os.str().empty());
        testing_assert(
            err_os.str().find(
                TESTING_STR_LIT("ERROR - unhandled exception: abcdefg")
            ) == 0
        );
    }

    throw_std_exception = false;
    main_calls.clear();
    out_os.str(string_type{});
    err_os.str(string_type{});

    {
        throw_unknown = true;
        auto const res = tst::wrap_main(args, tests, out_os, err_os, main_fn);
        testing_assert(res);
        testing_assert(main_calls.size() == 1);
        auto const& call = main_calls[0];
        testing_assert(call.p_args == &args);
        testing_assert(call.p_tests == &tests);
        testing_assert(call.p_out_os == &out_os);
        testing_assert(call.p_err_os == &err_os);
        testing_assert(out_os.str().empty());
        testing_assert(err_os.str().find(TESTING_STR_LIT("ERROR - unknown exception")) == 0);
    }
}

struct write_duration_call {
    report_ostream_type const* p_os;
    chrono::steady_clock::duration const* p_duration;
    bool use_color;
    bool braces;
};

struct test_reporter_plain_text_ctx {
    struct write_test_case_line_prefix_call {
        test_case_instance const* p_tc;
        report_ostream_type const* p_os;
        size_t max_test_case_id;
        bool use_color;
        bool use_emojis;
        size_t width;
    };

    struct write_with_color_call {
        report_ostream_type const* p_os;
        string_type value;
        tst::color col;
        bool use_color;
    };

    ostringstream_type rpt_os;
    vector<write_test_case_line_prefix_call> write_test_case_line_prefix_calls;
    vector<write_duration_call> write_duration_calls;
    vector<write_with_color_call> write_with_color_calls;

    unique_ptr<tst::test_case_reporter_plain_text> p_rep;

    test_reporter_plain_text_ctx(
        vector<test_case_instance> const& tests,
        size_t concurrency,
        bool use_color,
        bool use_emojis
    ) {
        p_rep.reset(
            new tst::test_case_reporter_plain_text{
                rpt_os,
                tests,
                concurrency,
                use_color,
                use_emojis,
                tst::test_case_reporter_plain_text::dependencies{
                    [this](
                        test_case_instance const& tc,
                        report_ostream_type& os,
                        size_t max_test_case_id,
                        bool use_color,
                        bool use_emojis,
                        size_t width
                    ) -> report_ostream_type& {
                        write_test_case_line_prefix_calls.push_back(
                            write_test_case_line_prefix_call{
                                &tc,
                                &os,
                                max_test_case_id,
                                use_color,
                                use_emojis,
                                width
                            }
                        );
                        os << "wtclp" << tc.name;
                        if (!tc.arg.empty()) {
                            os << '(' << tc.arg << ')';
                        }
                        return os;
                    },
                    [this](
                        report_ostream_type& os,
                        chrono::steady_clock::duration const& elapsed,
                        bool use_color,
                        bool braces
                    ) -> report_ostream_type& {
                        write_duration_calls.push_back(
                            write_duration_call{&os, &elapsed, use_color, braces}
                        );
                        return os << "wd";
                    },
                    [this](
                        report_ostream_type& os,
                        string_type const& value,
                        tst::color col,
                        bool use_color
                    ) -> report_ostream_type& {
                        write_with_color_calls.push_back(
                            write_with_color_call{&os, value, col, use_color}
                        );
                        return os << "wwc" << value;
                    }
                }
            }
        );
    }

    void clear_outputs() {
        rpt_os.str(string_type{});
        write_test_case_line_prefix_calls.clear();
        write_duration_calls.clear();
        write_with_color_calls.clear();
    }
};

void test_reporter_plain_text() {
    static struct {
        bool use_color;
        bool use_emojis;
        size_t concurrency;
    } const args[] = {
        {false, false, 1},
        {false, false, 8},
        {false, true, 1},
        {false, true, 8},
        {true, false, 1},
        {true, false, 8},
        {true, true, 1},
        {true, true, 8}
    };

    vector<test_case_instance> tests{
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-1"), 1, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-2"), 2, nullptr},
        test_case_instance{
            __FILE__,
            __LINE__,
            TESTING_STR_LIT("test-3"),
            3,
            nullptr, TESTING_STR_LIT("aabb")
        },
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-4"), 4, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-5"), 15, nullptr},
        test_case_instance{__FILE__, __LINE__, TESTING_STR_LIT("test-6"), 6, nullptr},
        test_case_instance{
            __FILE__,
            __LINE__,
            TESTING_STR_LIT("test-7"),
            7,
            nullptr, TESTING_STR_LIT("11")
        },
    };

    for (auto const& arg : args) {
        test_reporter_plain_text_ctx t_ctx{tests, arg.concurrency, arg.use_color, arg.use_emojis};

        auto& rep = *t_ctx.p_rep;

        testing_assert(&rep.os == &t_ctx.rpt_os);
        testing_assert(rep.max_test_case_id == 15);
        testing_assert(rep.max_test_case_name_len == 12);
        testing_assert(rep.n_total_tests == tests.size());
        testing_assert(rep.concurrency == arg.concurrency);
        testing_assert(rep.use_color == arg.use_color);
        testing_assert(rep.use_emojis == arg.use_emojis);
        testing_assert(rep.n_total_asserts == 0);

        {
            rep.start();
            testing_assert(t_ctx.rpt_os.str() == TESTING_STR_LIT("\n"));
            testing_assert(t_ctx.write_test_case_line_prefix_calls.empty());
            testing_assert(t_ctx.write_duration_calls.empty());
            testing_assert(t_ctx.write_with_color_calls.empty());
            t_ctx.clear_outputs();
        }

        {
            rep.test_case_starting(tests[2]);
            testing_assert(t_ctx.write_duration_calls.empty());
            testing_assert(t_ctx.write_with_color_calls.empty());

            if (arg.concurrency > 1) {
                testing_assert(t_ctx.write_test_case_line_prefix_calls.empty());
                testing_assert(t_ctx.rpt_os.str().empty());
            } else {
                testing_assert(t_ctx.write_test_case_line_prefix_calls.size() == 1);
                auto const& call = t_ctx.write_test_case_line_prefix_calls[0];
                testing_assert(call.p_tc == &tests[2]);
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.max_test_case_id == rep.max_test_case_id);
                testing_assert(call.use_color == arg.use_color);
                testing_assert(call.use_emojis == arg.use_emojis);
                testing_assert(call.width == rep.max_test_case_name_len);
                testing_assert(t_ctx.rpt_os.str() == TESTING_STR_LIT("wtclptest-3(aabb)"));
            }
            t_ctx.clear_outputs();

            rep.test_case_assert(tests[1], __FILE__, __LINE__, "", "", string_type{}, "");
            testing_assert(rep.n_total_asserts == 1);
            rep.test_case_assert(tests[1], __FILE__, __LINE__, "", "", string_type{}, "");
            testing_assert(rep.n_total_asserts == 2);
            rep.test_case_assert(tests[1], __FILE__, __LINE__, "", "", string_type{}, "");
            testing_assert(rep.n_total_asserts == 3);
            testing_assert(t_ctx.rpt_os.str().empty());
            t_ctx.clear_outputs();
        }

        chrono::steady_clock::duration const elapsed{};

        {
            auto const& test = tests[1];
            rep.test_case_passed(test, elapsed);
            testing_assert(t_ctx.write_duration_calls.size() == 1);
            auto const& wd_call = t_ctx.write_duration_calls[0];
            testing_assert(wd_call.p_os == &t_ctx.rpt_os);
            testing_assert(wd_call.p_duration == &elapsed);
            testing_assert(wd_call.use_color == arg.use_color);
            testing_assert(wd_call.braces);
            if (arg.concurrency > 1) {
                testing_assert(t_ctx.write_test_case_line_prefix_calls.size() == 1);
                auto const& call = t_ctx.write_test_case_line_prefix_calls[0];
                testing_assert(call.p_tc == &test);
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.max_test_case_id == rep.max_test_case_id);
                testing_assert(call.use_color == arg.use_color);
                testing_assert(call.use_emojis == arg.use_emojis);
                testing_assert(call.width == rep.max_test_case_name_len);
            } else {
                testing_assert(t_ctx.write_test_case_line_prefix_calls.empty());
            }
            if (arg.use_emojis) {
                testing_assert(t_ctx.write_with_color_calls.empty());
                testing_assert(
                    t_ctx.rpt_os.str() == (
                        (arg.concurrency > 1)
                            ? TESTING_STR_LIT("wtclptest-2 ") TUXX_EMOJI_PASSED
                                TESTING_STR_LIT(" wd\n")
                            : TESTING_STR_LIT(" ") TUXX_EMOJI_PASSED TESTING_STR_LIT(" wd\n")
                    )
                );
            } else {
                testing_assert(
                    t_ctx.rpt_os.str() == (
                        (arg.concurrency > 1)
                            ? TESTING_STR_LIT("wtclptest-2 wwcPASSED wd\n")
                            : TESTING_STR_LIT(" wwcPASSED wd\n")
                    )
                );
                testing_assert(t_ctx.write_with_color_calls.size() == 1);
                auto const& call = t_ctx.write_with_color_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.use_color == arg.use_color);
                testing_assert(call.value == TESTING_STR_LIT("PASSED"));
                testing_assert(call.col == TUXX_COLOR_SUCCESS);
            }
            t_ctx.clear_outputs();
        }

        {
            auto const& test = tests[1];
            test_case_failure_error const err{"the-file", 23, TESTING_STR_LIT("reason")};
            rep.test_case_failed(test, err, elapsed);
            testing_assert(t_ctx.write_duration_calls.size() == 1);
            auto const& wd_call = t_ctx.write_duration_calls[0];
            testing_assert(wd_call.p_os == &t_ctx.rpt_os);
            testing_assert(wd_call.p_duration == &elapsed);
            testing_assert(wd_call.use_color == arg.use_color);
            testing_assert(wd_call.braces);

            if (arg.concurrency > 1) {
                testing_assert(t_ctx.write_test_case_line_prefix_calls.size() == 1);
                auto const& call = t_ctx.write_test_case_line_prefix_calls[0];
                testing_assert(call.p_tc == &test);
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.max_test_case_id == rep.max_test_case_id);
                testing_assert(call.use_color == arg.use_color);
                testing_assert(call.use_emojis == arg.use_emojis);
                testing_assert(call.width == rep.max_test_case_name_len);
            } else {
                testing_assert(t_ctx.write_test_case_line_prefix_calls.empty());
            }

            ostringstream_type oss_expected;
            if (arg.concurrency > 1) {
                oss_expected << "wtclptest-2";
            }
            if (arg.use_emojis) {
                testing_assert(t_ctx.write_with_color_calls.size() == 2);
                oss_expected << ' ' << TUXX_EMOJI_FAILED << " wd\n";
            } else {
                testing_assert(t_ctx.write_with_color_calls.size() == 3);
                // We'll just check the first call here. The other two calls are common whether or
                // not emojis are being used.
                auto const& call = t_ctx.write_with_color_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.use_color == arg.use_color);
                testing_assert(call.value == TESTING_STR_LIT("FAILED"));
                testing_assert(call.col == TUXX_COLOR_FAIL);

                // Remove the first call...
                t_ctx.write_with_color_calls.erase(t_ctx.write_with_color_calls.begin());

                oss_expected << " wwcFAILED wd\n";
            }

            ostringstream_type oss;
            oss << err.file << ':' << err.line;
            
            oss_expected << "\twwc" << oss.str() << "\twwc" << err.detail << '\n';

            testing_assert(t_ctx.rpt_os.str() == oss_expected.str());

            auto const& wwc_call_1 = t_ctx.write_with_color_calls[0];
            testing_assert(wwc_call_1.p_os == &t_ctx.rpt_os);
            testing_assert(wwc_call_1.use_color == arg.use_color);
            testing_assert(wwc_call_1.value == oss.str());
            testing_assert(wwc_call_1.col == TUXX_COLOR_FILE_REF);

            auto const& wwc_call_2 = t_ctx.write_with_color_calls[1];
            testing_assert(wwc_call_2.p_os == &t_ctx.rpt_os);
            testing_assert(wwc_call_2.use_color == arg.use_color);
            testing_assert(wwc_call_2.value == err.detail);
            testing_assert(wwc_call_2.col == TUXX_COLOR_FAIL);

            t_ctx.clear_outputs();
        }

        {
            auto const& tc_fail_1 = tests[1];
            auto const& tc_fail_2 = tests[5];
            vector<vector<test_case_instance>> const failures_list{{}, {tc_fail_1, tc_fail_2}};
            // We reproduce the output exactly and ensure the reporter generates the same output.
            for (auto const& failures : failures_list) {
                rep.end_test_cases(elapsed, failures);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& wd_call = t_ctx.write_duration_calls[0];
                testing_assert(!wd_call.braces);
                testing_assert(wd_call.p_duration == &elapsed);
                testing_assert(wd_call.p_os == &t_ctx.rpt_os);
                testing_assert(wd_call.use_color == arg.use_color);
                ostringstream_type oss_expected;
                auto const width = 12u;
                oss_expected
                    << "\nCONCURRENCY:          " << setw(width) << setfill(TESTING_STR_LIT(' '))
                        << right << rep.concurrency
                    << "\nTOTAL TEST CASES RUN: " << setw(width) << setfill(TESTING_STR_LIT(' '))
                        << right << rep.n_total_tests
                    << "\nTOTAL ASSERTS:        " << setw(width) << setfill(TESTING_STR_LIT(' '))
                        << right << rep.n_total_asserts
                    << "\nELAPSED:              wd"
                    << "\nwwcPASSED:               " << setw(width) << setfill(TESTING_STR_LIT(' '))
                        << right << (rep.n_total_tests - failures.size()) << '\n';
                testing_assert(t_ctx.write_with_color_calls.size() >= 1);
                auto const& wwc_call_1 = t_ctx.write_with_color_calls[0];
                testing_assert(wwc_call_1.p_os == &t_ctx.rpt_os);
                testing_assert(wwc_call_1.use_color == arg.use_color);
                testing_assert(wwc_call_1.value == TESTING_STR_LIT("PASSED"));
                testing_assert(wwc_call_1.col == TUXX_COLOR_SUCCESS);
                if (failures.empty()) {
                    oss_expected << '\n';
                    if (arg.use_emojis) {
                        testing_assert(t_ctx.write_with_color_calls.size() == 1);
                        oss_expected << TUXX_EMOJI_SUCCESS;
                    } else {
                        testing_assert(t_ctx.write_with_color_calls.size() >= 2);
                        auto const& wwc_call_success = t_ctx.write_with_color_calls[1];
                        testing_assert(wwc_call_success.p_os == &t_ctx.rpt_os);
                        testing_assert(wwc_call_success.use_color == arg.use_color);
                        testing_assert(wwc_call_success.value == TESTING_STR_LIT("SUCCESS"));
                        testing_assert(wwc_call_success.col == TUXX_COLOR_SUCCESS);
                        oss_expected << "wwcSUCCESS";
                    }
                } else {
                    testing_assert(t_ctx.write_with_color_calls.size() >= 3);
                    auto const& wwc_call_fail = t_ctx.write_with_color_calls[1];
                    testing_assert(wwc_call_fail.p_os == &t_ctx.rpt_os);
                    testing_assert(wwc_call_fail.use_color == arg.use_color);
                    testing_assert(wwc_call_fail.value == TESTING_STR_LIT("FAILED"));
                    testing_assert(wwc_call_fail.col == TUXX_COLOR_FAIL);
                    auto const& wwc_call_failure = t_ctx.write_with_color_calls[2];
                    testing_assert(wwc_call_failure.p_os == &t_ctx.rpt_os);
                    testing_assert(wwc_call_failure.use_color == arg.use_color);
                    testing_assert(wwc_call_failure.value == TESTING_STR_LIT("FAILURES"));
                    testing_assert(wwc_call_failure.col == TUXX_COLOR_FAIL);
                    oss_expected << "wwcFAILED:               "
                        << setw(width) << setfill(TESTING_STR_LIT(' ')) << right << failures.size()
                        << '\n';
                    oss_expected << "\nwwcFAILURES:\n";
                    testing_assert(
                        t_ctx.write_test_case_line_prefix_calls.size() == failures.size()
                    );
                    auto const& wtclp_call_1 = t_ctx.write_test_case_line_prefix_calls[0];
                    testing_assert(wtclp_call_1.max_test_case_id == rep.max_test_case_id);
                    testing_assert(wtclp_call_1.p_os == &t_ctx.rpt_os);
                    testing_assert(wtclp_call_1.p_tc == &failures[0]);
                    testing_assert(wtclp_call_1.use_color == arg.use_color);
                    testing_assert(wtclp_call_1.use_emojis == arg.use_emojis);
                    testing_assert(wtclp_call_1.width == rep.max_test_case_name_len);
                    auto const& wtclp_call_2 = t_ctx.write_test_case_line_prefix_calls[1];
                    testing_assert(wtclp_call_2.max_test_case_id == rep.max_test_case_id);
                    testing_assert(wtclp_call_2.p_os == &t_ctx.rpt_os);
                    testing_assert(wtclp_call_2.p_tc == &failures[1]);
                    testing_assert(wtclp_call_2.use_color == arg.use_color);
                    testing_assert(wtclp_call_2.use_emojis == arg.use_emojis);
                    testing_assert(wtclp_call_2.width == rep.max_test_case_name_len);

                    for (auto const& tc : failures) {
                        oss_expected << "\twtclp" << tc.name;
                        if (!tc.arg.empty()) {
                            oss_expected << '(' << tc.arg << ')';
                        }
                        if (arg.use_emojis) {
                            oss_expected << ' ' << TUXX_EMOJI_FAILED;
                        }
                        oss_expected << '\n';
                    }
                    oss_expected << '\n';
                    if (arg.use_emojis) {
                        testing_assert(t_ctx.write_with_color_calls.size() == 3);
                        oss_expected << TUXX_EMOJI_FAILURE;
                    } else {
                        testing_assert(t_ctx.write_with_color_calls.size() == 4);
                        auto const& wwc_call_fail = t_ctx.write_with_color_calls[3];
                        testing_assert(wwc_call_fail.p_os == &t_ctx.rpt_os);
                        testing_assert(wwc_call_fail.use_color == arg.use_color);
                        testing_assert(wwc_call_fail.value == TESTING_STR_LIT("FAILURE"));
                        testing_assert(wwc_call_fail.col == TUXX_COLOR_FAIL);
                        oss_expected << "wwcFAILURE";
                    }
                }
                oss_expected << "\n\n";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                t_ctx.clear_outputs();
            }

            {
                rep.finish(string_type{}, string_type{});
                testing_assert(t_ctx.rpt_os.str().empty());
                t_ctx.clear_outputs();
            }

            {
                rep.finish(TESTING_STR_LIT("stdout-text"), TESTING_STR_LIT("stderr-text"));
                testing_assert(
                    t_ctx.rpt_os.str() ==
                        TESTING_STR_LIT(
                            "\nSTDOUT TEXT:\nstdout-text\n\nSTDERR TEXT:\nstderr-text\n"
                        )
                );
                t_ctx.clear_outputs();
            }

            {
                rep.finish(TESTING_STR_LIT("stdout-text"), string_type{});
                testing_assert(
                    t_ctx.rpt_os.str() == TESTING_STR_LIT("\nSTDOUT TEXT:\nstdout-text\n")
                );
                t_ctx.clear_outputs();
            }

            {
                rep.finish(string_type{}, TESTING_STR_LIT("stderr-text"));
                testing_assert(
                    t_ctx.rpt_os.str() == TESTING_STR_LIT("\nSTDERR TEXT:\nstderr-text\n")
                );
                t_ctx.clear_outputs();
            }
        }
    }
}

void test_reporter_delim() {
    char_type const delim[] = TESTING_STR_LIT(",:|\t");
    for (auto p_ch = delim; *p_ch; ++p_ch) {
        auto const d = *p_ch;
        ostringstream_type rpt_os;
        ostringstream_type oss_expected;
        size_t n_write_duration_calls{};
        write_duration_call wd_call{nullptr, nullptr, true, true};
        tst::test_case_reporter_delim rep(
            rpt_os,
            d,
            [&](
                report_ostream_type& os,
                chrono::steady_clock::duration const& elapsed,
                bool use_color,
                bool braces
            ) -> report_ostream_type& {
                os << "wd";
                ++n_write_duration_calls;
                wd_call.p_os = &os;
                wd_call.p_duration = &elapsed;
                wd_call.use_color = use_color;
                wd_call.braces = braces;
                return os;
            }
        );

        testing_assert(&rep.os == &rpt_os);
        testing_assert(rep.delim == d);

        {
            rep.start();
            oss_expected << "ID" << d << "NAME" << d << "ARG" << d << "STATUS" << d << "ELAPSED"
                << d << "DETAIL\n";
            testing_assert(rpt_os.str() == oss_expected.str());
            testing_assert(n_write_duration_calls == 0);
            rpt_os.str(string_type{});
            oss_expected.str(string_type{});
            wd_call.p_os = nullptr;
            wd_call.p_duration = nullptr;
            wd_call.use_color = wd_call.braces = true;
        }

        string const file = string{"the"} + static_cast<char>(d) + "file";
        chrono::steady_clock::duration const elapsed{};

        test_case_instance const tc{
            file.c_str(),
            34,
            string_type{TESTING_STR_LIT("test")} + d + TESTING_STR_LIT("1"),
            1,
            nullptr
        };
        test_case_instance const tc_arg{
            file.c_str(),
            53,
            string_type{TESTING_STR_LIT("test")} + d + TESTING_STR_LIT("2"),
            2,
            nullptr,
            string_type{TESTING_STR_LIT("arg")} + d + TESTING_STR_LIT("abc")
        };

        {
            // Should be a no-op.
            rep.test_case_starting(tc);
            testing_assert(rpt_os.str().empty());
            testing_assert(n_write_duration_calls == 0);
            testing_assert(!wd_call.p_os);
            testing_assert(!wd_call.p_duration);
            testing_assert(wd_call.use_color);
            testing_assert(wd_call.braces);
            oss_expected.str(string_type{});
        }

        {
            // PASS - Test case with no argument
            rep.test_case_passed(tc, elapsed);
            oss_expected << tc.id << d << "test<<delim>>1" << d << d << "PASS" << d << "wd"
                << d << '\n';
            testing_assert(rpt_os.str() == oss_expected.str());
            testing_assert(n_write_duration_calls == 1);
            testing_assert(wd_call.p_os == &rpt_os);
            testing_assert(wd_call.p_duration == &elapsed);
            testing_assert(!wd_call.use_color);
            testing_assert(!wd_call.braces);
            rpt_os.str(string_type{});
            oss_expected.str(string_type{});
            n_write_duration_calls = 0;
            wd_call.p_os = nullptr;
            wd_call.p_duration = nullptr;
            wd_call.use_color = wd_call.braces = true;
        }

        {
            // PASS - Test case with an argument
            rep.test_case_passed(tc_arg, elapsed);
            oss_expected << tc_arg.id << d << "test<<delim>>2" << d << "arg<<delim>>abc"
                << d << "PASS" << d << "wd" << d << '\n';
            testing_assert(rpt_os.str() == oss_expected.str());
            testing_assert(n_write_duration_calls == 1);
            testing_assert(wd_call.p_os == &rpt_os);
            testing_assert(wd_call.p_duration == &elapsed);
            testing_assert(!wd_call.use_color);
            testing_assert(!wd_call.braces);
            rpt_os.str(string_type{});
            oss_expected.str(string_type{});
            n_write_duration_calls = 0;
            wd_call.p_os = nullptr;
            wd_call.p_duration = nullptr;
            wd_call.use_color = wd_call.braces = true;
        }

        test_case_failure_error const err{
            file.c_str(),
            37,
            string_type{TESTING_STR_LIT("this")} + d + TESTING_STR_LIT("is") + d
                + TESTING_STR_LIT("the error")
        };

        {
            // FAIL - Test case with no argument
            rep.test_case_failed(tc, err, elapsed);
            oss_expected << tc.id << d << "test<<delim>>1" << d << d << "FAIL" << d << "wd"
                << d << "the<<delim>>file:" << err.line << " - this<<delim>>is<<delim>>the error\n";
            testing_assert(rpt_os.str() == oss_expected.str());
            testing_assert(n_write_duration_calls == 1);
            testing_assert(wd_call.p_os == &rpt_os);
            testing_assert(wd_call.p_duration == &elapsed);
            testing_assert(!wd_call.use_color);
            testing_assert(!wd_call.braces);
            rpt_os.str(string_type{});
            oss_expected.str(string_type{});
            n_write_duration_calls = 0;
            wd_call.p_os = nullptr;
            wd_call.p_duration = nullptr;
            wd_call.use_color = wd_call.braces = true;
        }

        {
            // FAIL - Test case with an argument
            rep.test_case_failed(tc_arg, err, elapsed);
            oss_expected << tc_arg.id << d << "test<<delim>>2" << d << "arg<<delim>>abc" << d
                << "FAIL" << d << "wd" << d << "the<<delim>>file:" << err.line
                << " - this<<delim>>is<<delim>>the error\n";
            testing_assert(rpt_os.str() == oss_expected.str());
            testing_assert(n_write_duration_calls == 1);
            testing_assert(wd_call.p_os == &rpt_os);
            testing_assert(wd_call.p_duration == &elapsed);
            testing_assert(!wd_call.use_color);
            testing_assert(!wd_call.braces);
            rpt_os.str(string_type{});
            oss_expected.str(string_type{});
            n_write_duration_calls = 0;
            wd_call.p_os = nullptr;
            wd_call.p_duration = nullptr;
            wd_call.use_color = wd_call.braces = true;
        }

        {
            // This should be a no-op...
            rep.test_case_assert(
                tc,
                "the-file",
                12,
                "assert_hello",
                "a != b",
                TESTING_STR_LIT("the-message"),
                "ex_info"
            );
            testing_assert(rpt_os.str().empty());
            testing_assert(n_write_duration_calls == 0);
            testing_assert(!wd_call.p_os);
            testing_assert(!wd_call.p_duration);
            testing_assert(wd_call.use_color);
            testing_assert(wd_call.braces);
            oss_expected.str(string_type{});
        }

        {
            // This should be a no-op...
            vector<test_case_instance> failures{tc, tc_arg};
            rep.end_test_cases(elapsed, failures);
            testing_assert(rpt_os.str().empty());
            testing_assert(n_write_duration_calls == 0);
            testing_assert(!wd_call.p_os);
            testing_assert(!wd_call.p_duration);
            testing_assert(wd_call.use_color);
            testing_assert(wd_call.braces);
            oss_expected.str(string_type{});
        }

        {
            rep.finish(string_type{}, string_type{});
            testing_assert(rpt_os.str().empty());
            testing_assert(n_write_duration_calls == 0);
            testing_assert(!wd_call.p_os);
            testing_assert(!wd_call.p_duration);
            testing_assert(wd_call.use_color);
            testing_assert(wd_call.braces);
            oss_expected.str(string_type{});
        }

        {
            rep.finish(TESTING_STR_LIT("stdout-data"), string_type{});
            testing_assert(rpt_os.str() == TESTING_STR_LIT("\nSTDOUT TEXT:\nstdout-data\n"));
            testing_assert(n_write_duration_calls == 0);
            testing_assert(!wd_call.p_os);
            testing_assert(!wd_call.p_duration);
            testing_assert(wd_call.use_color);
            testing_assert(wd_call.braces);
            rpt_os.str(string_type{});
            oss_expected.str(string_type{});
        }

        {
            rep.finish(string_type{}, TESTING_STR_LIT("stderr-data"));
            testing_assert(rpt_os.str() == TESTING_STR_LIT("\nSTDERR TEXT:\nstderr-data\n"));
            testing_assert(n_write_duration_calls == 0);
            testing_assert(!wd_call.p_os);
            testing_assert(!wd_call.p_duration);
            testing_assert(wd_call.use_color);
            testing_assert(wd_call.braces);
            rpt_os.str(string_type{});
            oss_expected.str(string_type{});
        }

        {
            rep.finish(TESTING_STR_LIT("stdout-data"), TESTING_STR_LIT("stderr-data"));
            testing_assert(
                rpt_os.str() == TESTING_STR_LIT(
                    "\nSTDOUT TEXT:\nstdout-data\n\nSTDERR TEXT:\nstderr-data\n"
                )
            );
            testing_assert(n_write_duration_calls == 0);
            testing_assert(!wd_call.p_os);
            testing_assert(!wd_call.p_duration);
            testing_assert(wd_call.use_color);
            testing_assert(wd_call.braces);
            rpt_os.str(string_type{});
            oss_expected.str(string_type{});
        }
    }
}

struct test_reporter_json_ctx {
    size_t n_host_name_calls{};
    size_t n_now_calls{};
    vector<chrono::system_clock::time_point> get_time_stamp_calls;
    vector<write_duration_call> write_duration_calls;

    chrono::system_clock::time_point now_value{};
    ostringstream_type rpt_os;
    unique_ptr<tst::test_case_reporter_json> p_rep;

    test_reporter_json_ctx(
        size_t n_total_tests,
        size_t concurrency
    ) {
        p_rep.reset(
            new tst::test_case_reporter_json{
                rpt_os,
                n_total_tests,
                concurrency,
                tst::test_case_reporter_json::dependencies{
                    [this] {
                        ++n_host_name_calls;
                        return string{"host-name"};
                    },
                    [this] {
                        ++n_now_calls;
                        return now_value;
                    },
                    [this](chrono::system_clock::time_point const& t) {
                        get_time_stamp_calls.push_back(t);
                        return string{"gts"};
                    },
                    [this](
                        report_ostream_type& os,
                        chrono::steady_clock::duration const& elapsed,
                        bool use_color,
                        bool braces
                    ) -> report_ostream_type& {
                        os << "wd";
                        write_duration_calls.push_back(
                            write_duration_call{
                                &os,
                                &elapsed,
                                use_color,
                                braces
                            }
                        );
                        return os;
                    }
                }
            }
        );
    }

    void clear_outputs(bool clear_reporter = true) {
        n_host_name_calls = n_now_calls = 0;
        get_time_stamp_calls.clear();
        write_duration_calls.clear();
        rpt_os.str(string_type{});
        if (clear_reporter) {
            p_rep->is_first = true;
            p_rep->n_total_asserts = 0;
            p_rep->tc_failures.clear();
        }
    }
};

void test_reporter_json() {
    static struct {
        size_t n_tests;
        size_t concurrency;
    } const args[] = {
        {10, 1},
        {10, 8},
        {20, 1},
        {20, 8}
    };
    for (auto const& arg : args) {
        test_reporter_json_ctx t_ctx{arg.n_tests, arg.concurrency};

        auto& rep = *t_ctx.p_rep;
        testing_assert(&rep.os == &t_ctx.rpt_os);
        testing_assert(rep.n_total_tests == arg.n_tests);
        testing_assert(rep.concurrency == arg.concurrency);
        testing_assert(rep.n_total_asserts == 0);
        testing_assert(rep.is_first);
        testing_assert(rep.tc_failures.empty());

        {
            rep.start();
            ostringstream_type oss_expected;
            oss_expected << "{\"host-name\":\"host-name\",\"time-stamp\":\"gts\","
                "\"concurrency\":" << arg.concurrency << ",\"n-total\":" << arg.n_tests
                << ",\"test-cases\":[";
            testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
            testing_assert(t_ctx.n_host_name_calls == 1);
            testing_assert(t_ctx.n_now_calls == 1);
            testing_assert(t_ctx.get_time_stamp_calls.size() == 1);
            auto const& tp = t_ctx.get_time_stamp_calls[0];
            testing_assert(tp == t_ctx.now_value);
            testing_assert(t_ctx.write_duration_calls.empty());
            t_ctx.clear_outputs();
        }

        chrono::steady_clock::duration const elapsed{};

        test_case_instance const tc{"the\nfile", 34, TESTING_STR_LIT("test\"1"), 1, nullptr};
        test_case_instance const tc_arg{
            "the\nfile",
            53,
            TESTING_STR_LIT("test\"2"),
            2,
            nullptr,
            TESTING_STR_LIT("arg\tabc")
        };

        {
            rep.test_case_assert(tc, "", 1, "", "", string_type{}, "");
            testing_assert(rep.n_total_asserts == 1);
            rep.test_case_assert(tc, "", 1, "", "", string_type{}, "");
            testing_assert(rep.n_total_asserts == 2);
            rep.test_case_assert(tc, "", 1, "", "", string_type{}, "");
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(rep.n_total_asserts == 3);
            testing_assert(t_ctx.rpt_os.str().empty());
            t_ctx.clear_outputs();
        }

        {
            // Should be a no-op
            rep.test_case_starting(tc);
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(t_ctx.write_duration_calls.empty());
            testing_assert(t_ctx.rpt_os.str().empty());
            t_ctx.clear_outputs();
        }

        {
            // PASS - test case with no argument
            {
                rep.test_case_passed(tc, elapsed);
                ostringstream_type oss_expected;

                oss_expected << "{\"name\":\"test\\\"1\",\"file-name\":\"the\\nfile\","
                    "\"line\":" << tc.line << ",\"elapsed\":\"wd\",\"passed\":true}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.get_time_stamp_calls.empty());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);
                t_ctx.clear_outputs(false);
            }

            {
                // rep.is_first is false now. Next call should add a leading comma.
                rep.test_case_passed(tc, elapsed);
                ostringstream_type oss_expected;
                oss_expected << ",{\"name\":\"test\\\"1\",\"file-name\":\"the\\nfile\","
                    "\"line\":" << tc.line << ",\"elapsed\":\"wd\",\"passed\":true}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.get_time_stamp_calls.empty());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);
            }

            t_ctx.clear_outputs();
        }

        {
            // PASS - test case with argument
            {
                rep.test_case_passed(tc_arg, elapsed);
                ostringstream_type oss_expected;

                oss_expected << "{\"name\":\"test\\\"2\",\"arg\":\"arg\\tabc\","
                    "\"file-name\":\"the\\nfile\",\"line\":"
                    << tc_arg.line << ",\"elapsed\":\"wd\",\"passed\":true}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.get_time_stamp_calls.empty());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);
                t_ctx.clear_outputs(false);
            }

            {
                // rep.is_first is false now. Next call should add a leading comma.
                rep.test_case_passed(tc_arg, elapsed);
                ostringstream_type oss_expected;

                oss_expected << ",{\"name\":\"test\\\"2\",\"arg\":\"arg\\tabc\","
                    "\"file-name\":\"the\\nfile\",\"line\":"
                    << tc_arg.line << ",\"elapsed\":\"wd\",\"passed\":true}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.get_time_stamp_calls.empty());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);
            }

            t_ctx.clear_outputs();
        }

        test_case_failure_error const err{
            "the\nfile",
            56,
            TESTING_STR_LIT("the\terror\n.")
        };

        {
            // FAIL - test case with no argument
            {
                rep.test_case_failed(tc, err, elapsed);
                ostringstream_type oss_expected;

                oss_expected << "{\"name\":\"test\\\"1\",\"file-name\":\"the\\nfile\","
                    "\"line\":" << tc.line << ",\"elapsed\":\"wd\",\"error\":{\"file\":"
                    "\"the\\nfile\",\"line\":" << err.line << ",\"detail\":\"the\\terror\\n.\"}"
                    ",\"passed\":false}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.get_time_stamp_calls.empty());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);
                t_ctx.clear_outputs(false);
            }

            {
                // rep.is_first is false now. Next call should add a leading comma.
                rep.test_case_failed(tc, err, elapsed);
                ostringstream_type oss_expected;

                oss_expected << ",{\"name\":\"test\\\"1\",\"file-name\":\"the\\nfile\","
                    "\"line\":" << tc.line << ",\"elapsed\":\"wd\",\"error\":{\"file\":"
                    "\"the\\nfile\",\"line\":" << err.line << ",\"detail\":\"the\\terror\\n.\"}"
                    ",\"passed\":false}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.get_time_stamp_calls.empty());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);
            }

            t_ctx.clear_outputs();
        }

        {
            // FAIL - test case with argument
            {
                rep.test_case_failed(tc_arg, err, elapsed);
                ostringstream_type oss_expected;

                oss_expected << "{\"name\":\"test\\\"2\",\"arg\":\"arg\\tabc\","
                    "\"file-name\":\"the\\nfile\",\"line\":" << tc_arg.line
                    << ",\"elapsed\":\"wd\",\"error\":{\"file\":"
                    "\"the\\nfile\",\"line\":" << err.line << ",\"detail\":\"the\\terror\\n.\"}"
                    ",\"passed\":false}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.get_time_stamp_calls.empty());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);
                t_ctx.clear_outputs(false);
            }

            {
                // rep.is_first is false now. Next call should add a leading comma.
                rep.test_case_failed(tc_arg, err, elapsed);
                ostringstream_type oss_expected;

                oss_expected << ",{\"name\":\"test\\\"2\",\"arg\":\"arg\\tabc\","
                    "\"file-name\":\"the\\nfile\",\"line\":" << tc_arg.line
                    << ",\"elapsed\":\"wd\",\"error\":{\"file\":"
                    "\"the\\nfile\",\"line\":" << err.line << ",\"detail\":\"the\\terror\\n.\"}"
                    ",\"passed\":false}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.get_time_stamp_calls.empty());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);
            }

            t_ctx.clear_outputs();
        }

        {
            rep.end_test_cases(elapsed, vector<test_case_instance>{});
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(t_ctx.write_duration_calls.empty());
            testing_assert(t_ctx.rpt_os.str().empty());
            testing_assert(rep.total_elapsed == elapsed);
            testing_assert(rep.tc_failures.empty());
            t_ctx.clear_outputs();
        }

        {
            rep.end_test_cases(elapsed, vector<test_case_instance>{tc});
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(t_ctx.write_duration_calls.empty());
            testing_assert(t_ctx.rpt_os.str().empty());
            testing_assert(rep.total_elapsed == elapsed);
            testing_assert(rep.tc_failures.size() == 1);
            testing_assert(strcmp(rep.tc_failures[0].file, tc.file) == 0);
            testing_assert(rep.tc_failures[0].line == tc.line);
            testing_assert(rep.tc_failures[0].name == tc.name);
            t_ctx.clear_outputs();
        }

        {
            vector<vector<test_case_instance>> failure_lists{{}, {tc, tc_arg}};

            for (auto const& failures : failure_lists) {
                rep.end_test_cases(elapsed, failures);
                rep.n_total_asserts = 4;
                rep.finish(TESTING_STR_LIT("stdout\ttext"), TESTING_STR_LIT("stderr\ntext"));

                ostringstream_type oss_expected;
                oss_expected << "],\"n-total-asserts\":" << rep.n_total_asserts << ",\"n-passed\":"
                    << (arg.n_tests - rep.tc_failures.size()) << ",\"n-failed\":"
                    << rep.tc_failures.size() << ",\"elapsed\":\"wd\",\"passed\":"
                    << (rep.tc_failures.empty() ? "true" : "false")
                    << ",\"stdout\":\"stdout\\ttext\",\"stderr\":\"stderr\\ntext\"}";

                testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
                testing_assert(t_ctx.n_host_name_calls == 0);
                testing_assert(t_ctx.n_now_calls == 0);
                testing_assert(t_ctx.write_duration_calls.size() == 1);
                auto const& call = t_ctx.write_duration_calls[0];
                testing_assert(call.p_os == &t_ctx.rpt_os);
                testing_assert(call.p_duration == &rep.total_elapsed);
                testing_assert(!call.use_color);
                testing_assert(!call.braces);

                t_ctx.clear_outputs();
            }
        }
    }
}

struct test_reporter_junit_ctx {
    size_t n_host_name_calls{};
    size_t n_now_calls{};
    size_t n_seconds_calls{};
    vector<chrono::system_clock::time_point> get_time_stamp_calls;

    chrono::system_clock::time_point now_value{};
    ostringstream_type rpt_os;
    unique_ptr<tst::test_case_reporter_junit> p_rep;

    test_reporter_junit_ctx(
        string_type prog_name,
        size_t n_total_tests,
        string_type const& test_report_name = string_type{}
    ) {
        p_rep.reset(
            new tst::test_case_reporter_junit{
                rpt_os,
                std::move(prog_name),
                n_total_tests,
                test_report_name,
                tst::test_case_reporter_junit::dependencies{
                    [this] {
                        ++n_host_name_calls;
                        return string{"host-name"};
                    },
                    [this] {
                        ++n_now_calls;
                        return now_value;
                    },
                    [this](chrono::system_clock::time_point const& t) {
                        get_time_stamp_calls.push_back(t);
                        return string{"gts"};
                    },
                    [this](chrono::steady_clock::duration const& elapsed) {
                        ++n_seconds_calls;
                        return 7.2;
                    }
                }
            }
        );
    }

    void clear_outputs() {
        n_now_calls = n_host_name_calls = n_seconds_calls = 0;
        get_time_stamp_calls.clear();
        rpt_os.str(string_type{});
        p_rep->results.clear();
        p_rep->n_failures = 0;
        p_rep->n_asserts = 0;
        p_rep->n_total_asserts = 0;
    }
};

string_type xml_escape(string_type const& s) {
    string_type ret;
    for (auto c : s) {
        switch (c) {
            case '\'': ret += TESTING_STR_LIT("&apos;"); break;
            case '"': ret += TESTING_STR_LIT("&quot;"); break;
            case '<': ret += TESTING_STR_LIT("&lt;"); break;
            case '>': ret += TESTING_STR_LIT("&gt;"); break;
            case '&': ret += TESTING_STR_LIT("&amp;"); break;
            default: ret += c; break;
        }
    }
    return ret;
}

void test_reporter_junit() {
    vector<string_type> report_names{string_type{}, TESTING_STR_LIT("report&name<xyz")};

    static char_type const prog_name[] = TESTING_STR_LIT("/this/is/the/prog&name");

    for (auto const& report_name : report_names) {
        test_reporter_junit_ctx t_ctx{prog_name, 11, report_name};

        auto& rep = *t_ctx.p_rep;

        testing_assert(rep.n_total_tests == 11);
        testing_assert(rep.results.empty());

        testing_assert(rep.test_suite_name == TESTING_STR_LIT("this_is_the_prog&name"));
        testing_assert(rep.test_report_name == report_name);
        testing_assert(rep.n_failures == 0);
        testing_assert(rep.n_asserts == 0);
        testing_assert(rep.n_total_asserts == 0);

        {
            rep.start();
            testing_assert(t_ctx.rpt_os.str().empty());
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 1);
            testing_assert(rep.start_time == chrono::system_clock::time_point{});
            testing_assert(rep.results.empty());
            testing_assert(rep.n_failures == 0);
            testing_assert(rep.n_asserts == 0);
            testing_assert(rep.n_total_asserts == 0);
            t_ctx.clear_outputs();
        }

        chrono::steady_clock::duration const elapsed{};

        test_case_instance const tc{"the'file", 34, TESTING_STR_LIT("test>1"), 1, nullptr};
        test_case_instance const tc_arg{
            "the'file",
            53,
            TESTING_STR_LIT("test\"2"),
            2,
            nullptr,
            TESTING_STR_LIT("arg<abc")
        };

        {
            rep.test_case_assert(tc, "", 1, "", "", string_type{}, "");
            testing_assert(rep.n_total_asserts == 1);
            testing_assert(rep.n_asserts == 1);
            rep.test_case_assert(tc, "", 1, "", "", string_type{}, "");
            testing_assert(rep.n_total_asserts == 2);
            testing_assert(rep.n_asserts == 2);
            rep.test_case_assert(tc, "", 1, "", "", string_type{}, "");
            testing_assert(rep.n_total_asserts == 3);
            testing_assert(rep.n_asserts == 3);
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(rep.results.empty());
            testing_assert(rep.n_failures == 0);
            t_ctx.clear_outputs();
        }

        {
            // PASSED - no argument
            rep.n_asserts = 10;
            rep.n_total_asserts = 11;
            rep.test_case_passed(tc, elapsed);
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(rep.results.size() == 1);
            testing_assert(rep.n_asserts == 0);
            testing_assert(rep.n_total_asserts == 11);
            testing_assert(t_ctx.rpt_os.str().empty());

            auto const& res = rep.results[0];
            testing_assert(strcmp(res.file, tc.file) == 0);
            testing_assert(res.line == tc.line);
            testing_assert(res.full_name == TESTING_STR_LIT("test>1"));
            testing_assert(res.passed);
            testing_assert(res.n_asserts == 10);
            testing_assert(res.elapsed == elapsed);
            testing_assert(res.error.empty());
            t_ctx.clear_outputs();
        }

        {
            // PASSED - argument
            rep.n_asserts = 10;
            rep.n_total_asserts = 11;
            rep.test_case_passed(tc_arg, elapsed);
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(rep.results.size() == 1);
            testing_assert(rep.n_asserts == 0);
            testing_assert(rep.n_total_asserts == 11);
            testing_assert(t_ctx.rpt_os.str().empty());

            auto const& res = rep.results[0];
            testing_assert(strcmp(res.file, tc_arg.file) == 0);
            testing_assert(res.line == tc_arg.line);
            testing_assert(res.full_name == TESTING_STR_LIT("test\"2(arg<abc)"));
            testing_assert(res.passed);
            testing_assert(res.n_asserts == 10);
            testing_assert(res.elapsed == elapsed);
            testing_assert(res.error.empty());
            t_ctx.clear_outputs();
        }

        test_case_failure_error const err{
            "the'file",
            56,
            TESTING_STR_LIT("the<error&.")
        };

        {
            // FAILED - no argument
            rep.n_asserts = 10;
            rep.n_total_asserts = 11;
            rep.test_case_failed(tc, err, elapsed);
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(rep.results.size() == 1);
            testing_assert(rep.n_asserts == 0);
            testing_assert(rep.n_total_asserts == 11);
            testing_assert(t_ctx.rpt_os.str().empty());

            auto const& res = rep.results[0];
            testing_assert(strcmp(res.file, tc.file) == 0);
            testing_assert(res.line == tc.line);
            testing_assert(res.full_name == TESTING_STR_LIT("test>1"));
            testing_assert(!res.passed);
            testing_assert(res.n_asserts == 10);
            testing_assert(res.elapsed == elapsed);
            testing_assert(res.error == TESTING_STR_LIT("the'file:56 - the<error&."));
            t_ctx.clear_outputs();
        }

        {
            // FAILED - argument
            rep.n_asserts = 10;
            rep.n_total_asserts = 11;
            rep.test_case_failed(tc_arg, err, elapsed);
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(rep.results.size() == 1);
            testing_assert(rep.n_asserts == 0);
            testing_assert(rep.n_total_asserts == 11);
            testing_assert(t_ctx.rpt_os.str().empty());

            auto const& res = rep.results[0];
            testing_assert(strcmp(res.file, tc_arg.file) == 0);
            testing_assert(res.line == tc_arg.line);
            testing_assert(res.full_name == TESTING_STR_LIT("test\"2(arg<abc)"));
            testing_assert(!res.passed);
            testing_assert(res.n_asserts == 10);
            testing_assert(res.elapsed == elapsed);
            testing_assert(res.error == TESTING_STR_LIT("the'file:56 - the<error&."));
            t_ctx.clear_outputs();
        }

        {
            rep.end_test_cases(elapsed, vector<test_case_instance>{});
            testing_assert(t_ctx.n_host_name_calls == 0);
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(rep.results.empty());
            testing_assert(rep.n_asserts == 0);
            testing_assert(rep.n_total_asserts == 0);
            testing_assert(t_ctx.rpt_os.str().empty());
            t_ctx.clear_outputs();
        }

        {
            test_case_instance tc_fail{"the'file", 87, TESTING_STR_LIT("test&20"), 20, nullptr};
            rep.n_total_asserts = 12;
            rep.n_asserts = 3;
            rep.test_case_passed(tc, elapsed);
            rep.n_asserts = 4;
            rep.test_case_passed(tc_arg, elapsed);
            rep.n_asserts = 5;
            rep.test_case_failed(tc_fail, err, elapsed);
            auto const stdout_data = TESTING_STR_LIT("stdout&data");
            auto const stderr_data = TESTING_STR_LIT("stderr'data");
            rep.finish(stdout_data, stderr_data);
            testing_assert(t_ctx.n_host_name_calls == (report_name.empty() ? 0 : 1));
            testing_assert(t_ctx.n_now_calls == 0);
            testing_assert(rep.n_asserts == 0);
            testing_assert(rep.n_total_asserts == 12);

            ostringstream_type oss_expected;

            auto const test_suite_name = "this_is_the_prog&amp;name";
            auto const file_name = "the&apos;file";

            if (report_name.empty()){
                oss_expected << "<testsuite name='" << test_suite_name << "' tests='"
                    << rep.n_total_tests << "' failures='1' errors='0' skipped='0' timestamp='gts' "
                    "time='7.2' assertions='" << rep.n_total_asserts << "' >\n"
                    "\t<system-out>" << xml_escape(stdout_data) << "</system-out>\n"
                    "\t<system-err>" << xml_escape(stderr_data) << "</system-err>\n";
                for (auto const& res : rep.results) {
                    oss_expected << "\t<testcase name='" << xml_escape(res.full_name)
                        << "' classname='" << test_suite_name << "' assertions='" << res.n_asserts
                        << "' file='" << file_name << "' line='" << res.line << "' time='7.2'";
                    if (res.passed) {
                        oss_expected << " />\n";
                    } else {
                        oss_expected << " >\n\t\t<failure message='" << xml_escape(res.error)
                            << "' type='AssertionError' />\n\t</testcase>\n";
                    }
                }
                oss_expected << "</testsuite>\n";
            } else {
                oss_expected << "<?xml version='1.0' encoding='utf-8'>\n<testsuites "
                    "name='" << xml_escape(rep.test_report_name) << "' hostname='host-name' tests='"
                    << rep.n_total_tests << "' failures='1' errors='0' skipped='0' timestamp='gts' "
                    "time='7.2' assertions='" << rep.n_total_asserts << "' >\n"
                    << "\t<testsuite name='" << test_suite_name << "' tests='"
                    << rep.n_total_tests << "' failures='1' errors='0' skipped='0' timestamp='gts' "
                    "time='7.2' assertions='" << rep.n_total_asserts << "' >\n"
                    "\t\t<system-out>" << xml_escape(stdout_data) << "</system-out>\n"
                    "\t\t<system-err>" << xml_escape(stderr_data) << "</system-err>\n";
                for (auto const& res : rep.results) {
                    oss_expected << "\t\t<testcase name='" << xml_escape(res.full_name)
                        << "' classname='" << test_suite_name << "' assertions='" << res.n_asserts
                        << "' file='" << file_name << "' line='" << res.line << "' time='7.2'";
                    if (res.passed) {
                        oss_expected << " />\n";
                    } else {
                        oss_expected << " >\n\t\t\t<failure message='" << xml_escape(res.error)
                            << "' type='AssertionError' />\n\t\t</testcase>\n";
                    }
                }
                oss_expected << "\t</testsuite>\n</testsuites>\n";
            }

            testing_assert(t_ctx.rpt_os.str() == oss_expected.str());
            t_ctx.clear_outputs();
        }
    }
}

}

void testing_main(vector<string_type> const&) {
    test_iequals();
    test_calc_id_field_width();
    test_write_id();
    test_write_test_case_name();
    test_write_test_case_line_prefix();
    test_write_duration();
    test_get_time_stamp();
    test_usage();
    test_to_lower();
    test_split_delim();
    test_parse_delim();
    test_read_number_range();
    test_find_arg();
    test_remove_arg();
    test_is_arg_specified();
    test_get_arg_value();
    test_run_test_case();
    test_run_test_cases();
    test_list_tests();
    test_does_test_case_match_names();
    test_does_test_case_match_number_exprs();
    test_redirect_output();
    test_restore_output();
    test_with_redirection();
    test_check_args();
    test_main();
    test_wrap_main();
    test_reporter_plain_text();
    test_reporter_delim();
    test_reporter_json();
    test_reporter_junit();
}
