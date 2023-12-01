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

#include "testing.hpp"

#include <tuxx.hpp>

namespace {

template <typename Ch>
void test_iequals() {
    testing_assert(true);
}

template <typename Ch>
void test_write_idx() {
  
}

template <typename Ch>
void test_write_test_case_name() {
  
}

template <typename Ch>
void test_write_test_case_line_prefix() {
  
}

template <typename Ch>
void test_write_duration() {
  
}

template <typename Ch>
void test_get_time_stamp() {
  
}

template <typename Ch>
void test_usage() {
  
}

template <typename Ch>
void test_to_lower() {
  
}

template <typename Ch>
void test_split_delim() {
  
}

template <typename Ch>
void test_parse_delim() {
  
}

template <typename Ch>
void test_read_number_range() {
  
}

template <typename Ch>
void test_main() {
  
}

template <typename Ch>
void test_wrap_main() {
  
}

template <typename Ch>
void test_reporter_plain_text() {
  
}

template <typename Ch>
void test_reporter_delim() {
  
}

template <typename Ch>
void test_reporter_json() {
  
}

template <typename Ch>
void test_reporter_junit() {
  
}

}

template <typename Ch>
void testing_main(
    int argc,
    Ch* argv[]
) {
    test_iequals<Ch>();
    test_write_idx<Ch>();
    test_write_test_case_name<Ch>();
    test_write_test_case_line_prefix<Ch>();
    test_write_duration<Ch>();
    test_get_time_stamp<Ch>();
    test_usage<Ch>();
    test_to_lower<Ch>();
    test_split_delim<Ch>();
    test_parse_delim<Ch>();
    test_read_number_range<Ch>();
    test_main<Ch>();
    test_wrap_main<Ch>();
    test_reporter_plain_text<Ch>();
    test_reporter_delim<Ch>();
    test_reporter_json<Ch>();
    test_reporter_junit<Ch>();
}
