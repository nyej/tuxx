# TUXX (Test-the-units++)

A simple, modern, extensible single-header C++ unit testing library.

- [TUXX (Test-the-units++)](#tuxx-test-the-units)
  - [Why TUXX?](#why-tuxx)
  - [Supported C++ Versions](#supported-c-versions)
  - [Getting Started](#getting-started)
    - [Basics](#basics)
      - [Including the Library](#including-the-library)
      - [Defining Test Cases](#defining-test-cases)
      - [Assertions](#assertions)
  - [Output and User-Defined Types](#output-and-user-defined-types)
  - [Command Line Options](#command-line-options)
  - [Test Case Macros](#test-case-macros)
  - [List of Assertions](#list-of-assertions)
  - [Building the Unit Test Executable](#building-the-unit-test-executable)
  - [Some More Interesting Examples](#some-more-interesting-examples)
    - [Test Cases with Fixtures](#test-cases-with-fixtures)
    - [Test Cases with Arguments](#test-cases-with-arguments)
      - [Arguments With a Pre-Defined Array](#arguments-with-a-pre-defined-array)
      - [Arguments With an Automatically Defined Array](#arguments-with-an-automatically-defined-array)
      - [Arguments With a more interesting Argument Type](#arguments-with-a-more-interesting-argument-type)
    - [Argument Test Cases with Fixtures](#argument-test-cases-with-fixtures)
    - [Running the Tests](#running-the-tests)
      - [Capturing `stdout`/`stderr`](#capturing-stdoutstderr)
  - [Character Types](#character-types)
  - [Mocking](#mocking)
  - [Reporters](#reporters)
    - [Custom Reporters](#custom-reporters)
    - [Defining a Custome Reporter](#defining-a-custome-reporter)
      - [Deriving a custom Class from `nyej::tuxx::test_case_reporter`](#deriving-a-custom-class-from-nyejtuxxtest_case_reporter)
      - [Creating a custom Reporter without Derivation](#creating-a-custom-reporter-without-derivation)
  - [Exit Codes](#exit-codes)
  - [Colorized Output](#colorized-output)
  - [Emojis](#emojis)
  - [TUXX Library Tests and Examples](#tuxx-library-tests-and-examples)
  - [Testes Platforms/Compilers](#testes-platformscompilers)
  - [Future Improvements](#future-improvements)

A small but powerful and very flexible unit testing library for C++.

## Why TUXX?

- Single-header `#include`
- Filtering test cases by name prefix or by their auto-generated IDs
- Fixtures support
- Several built-in reporting formats
- Can add custom reporters
- Randomized ordering of test cases
- Parallel execution of test cases
- Individual test cases and overall execution is timed
- No external dependencies other than the C++ standar library
- Permissive license
- Very easy to use and easy to get started with
- Concise and simple
- Portable
- Several useful assertion macros
- Test case generation from argument lists
- Works seamlessly with `char` or `wchar_t` based on configuration macro
- Highly customizable

## Supported C++ Versions

This library currently supports C++11 and later.

## Getting Started

This library is similar to other C++ test libraries in that test cases can be defined in several modules with simple macros and one source file in the executable needs to `#define TUXX_DEFINE_TEST_MAIN` prior to `#include`ing `tuxx.hpp`.

### Basics

#### Including the Library

This is a single-header library, so all one needs to do to use the library is:

```C++
// In one of your source files, #define TUXX_DEFINE_TEST_MAIN before #including the below header
// to get the main() entry point.

#include <tuxx.hpp>
```

#### Defining Test Cases
Here is an example of the simplest test case form:

```C++
test_case(some_test_case_name) {
    // Arrange
    // Act
    // Assert
}

test_case(another_test_case_name) {
    // Arrange
    // Act
    // Assert
}
```
Assuming everything builds and passes, running the resulting executable will produce the following output with the default reporter and emojis enabled (using the default emojis):

<pre style="background-color:#222">
[<span style="color:cyan">1</span>] 🧪 <span style="color:yellow">some_test_case_name</span>    ✅ [<span style="color:white">00</span><span style="color:#00f">:</span><span style="color:white">00</span><span style="color:#00f">:</span><span style="color:white">00</span><span style="color:#00f">.</span><span style="color:white">000</span>]
[<span style="color:cyan">2</span>] 🧪 <span style="color:yellow">another_test_case_name</span> ✅ [<span style="color:white">00</span><span style="color:#00f">:</span><span style="color:white">00</span><span style="color:#00f">:</span><span style="color:white">00</span><span style="color:#00f">.</span><span style="color:white">000</span>]

<span style="color:white">CONCURRENCY:                     1</span>
<span style="color:white">TOTAL TEST CASES RUN:            2</span>
<span style="color:white">ASSERTS:                        12</span>
<span style="color:white">ELAPSED:</span>              <span style="color:white">00</span><span style="color:#00f">:</span><span style="color:white">00</span><span style="color:#00f">:</span><span style="color:white">00</span><span style="color:#00f">.</span><span style="color:white">000</span>
<span style="color:#0f0">PASSED</span>:                          2

😎
</pre>

Notice the cyan number in square brackets to the left of the listed test cases.
This is the test case's id/number and is used to help focus on certain tests while in development.
For example, if you were only interested in test cases 3 and 11, you can isolate these cases by specifying `-n3,11` or `--number 3,11` on the executable's command line.
Test case ranges for the `-n|--number` option are also supported by using the `-` character in between test case numbers.

Tests can also be selected by using a case-insensitive test case name prefix with the `-m|--match` argument as in: `-mTestGroup1,TestGroup7` or `--match TestGroup1,TestGroup7`.
Keep in mind there is no significance in the names here - they are simply prefixes of the available test cases.

#### Assertions

This library fails a test case immediately on any assertion failure by throwing an internal exception.

>NOTE: if you ever need to `catch (...)` in your test cases, be sure to have it `throw;` at the end of this `catch`'s body.

Assertions are of the form:

```C++
assert_[op](...args...);
```

There is a corresponding `assert_[op]_msg` macro for every plain `assert_[op]` macro which accepts one additional argument which is used to provide extra information on assertion failures.
It can be a simple value or a `std::basic_ostream<char_type>` streaming style expression:

An example of using a `_msg` assert:

```C++
test_case(some_test_case) {
    for (auto i = 0; i < 10; i++) {
        assert_true_msg(expr, "iteration: " << i);
    }
}
```

## Output and User-Defined Types

In order for the library to provide as much detail as possible to the user, every user-defined type that is used with the library except fixtures is expected to have a corresponding `std::basic_ostream<char_type>& operator<<(std::basic_ostream<char_type>& os, user_defined_type const&)`, where `char_type` is the character type being used - see the [Character Types section](#character-types) section, below.
This applies to anything passed to an `assert_[op]` macro and arguments provided to the `test_case_args_*` test case definition macros.

Compile errors will result if a type is missing the operator.

## Command Line Options

The following command line options are supported by the resulting test executable:

|Option|Description|
|-|-|
|`-h`, `--help`|Prints usage and exits|
|`-l`, `--list`|Lists all test cases, including their ids/numbers without running any.|
|`-e`, `--emojis`|Enable emojis for results with text reporter. Only applies to basic text reporter.| 
|`-n<NumberExprs>`, `--number <NumberExprs>`|Selects specific tests by id/number. The `<NumberExpr>` is a comma-separated list of expressions that are either a single id/number or include a `-` to represent a range. Valid ranges are `4-8` (runs tests `4` through `8`), `-9` (runs tests `1` through `9`) and `5-` (runs all tests from `5` to the end). This can be specified along with `-m` or `--match`, below|
|`-m<NamePrefixes>`, `--match <NamePrefixes>`|Selects test cases matching the provided comma-delimited name prefixes. Matching is case-insensitive|
|`--no-color`|Suppresses color output to avoid having control characters in the output. If `stdout` is not a terminal, this will automatically be added|
|`-p[n]`, `--parallel [n]`|Runs up to `n` tests in parallel to speed up the overall execution. If `n` is omitted, the hardware concurrency is used for `n`. The output of the test will show the concurrency that was used. This should only be used if the test cases are truely stateless.|
|`-r`, `--randomized`|Runs tests in randomized order. This is used to detect any accidental interdependencies between test cases. It is recommended that this flag always be used in development.|
|`-j`, `--json`|Print json output. This is ignored by `-l`, `--list`|
|`-junit [<report-name>]`|Prints a JUnit-style report or report fragment. If `<report-name>` is specified, a `<testsuites>`-level report is created; otherwise, a `<testsuite>`-level report is created with the assumption the full report will be produced by a script that collects the outputs of several unit test executables from a directory tree|
|`-d[c]`, `--delim [c]`|Prints a tabular output with `c` as the column delimiter. If not specified, tab is used for `c`|
|`-t`, `--text`|Chooses the default text reporter even if a custom reporter was provided|
|`-c`, `--capture`|Captures `stdout`/`stderr` even if the default reporter is being used|

>NOTE: test case ids/numbers will change as test cases are added or removed.
They are not permanent ids; they are intended to help provide a quick turnaround time when fixing failing test cases or otherwise isolating them.

## Test Case Macros

Test cases can be added by using one of the following macros.
The body of the test case is a regular function body and is expected to be added immediately following the use of each of the following macros.

|Macro|Description|
|-|-|
|`test_case(`test_case_name`)`|Declares a test case that accepts no arguments with the name `test_case_name`.|
|`test_case_f(`fixture_class_or_struct, test_case_name`)`|Declares a test case that uses a user-defined `struct`/`class` `fixture_class_or_struct` whose constructor and destructor works as the `setup`/`teardown`, respectively, if needed. The fixture instance is available within the body of the test case as `fixture`.|
|`test_case_args_arr(`test_case_name, name_of_array/container_with_args`)`|Generates a set of test cases, one for each element in the provided array. Each test case will have its own separate id/number so it can be isolated with `-n`, `--number`. The argment is available within the test case as `arg`.|
|`test_case_args(`test_case_name, arg_type, ...comma-delimited args...`)`|This is the same as `test_case_args_arr`, except the array does not need to be defined prior. This will define the array as `arg_type hidden_name_of_arr[] = {...comma-delimited args};` then call `test_case_args_arr`.|
|`test_case_args_arr_f(`fixture_class_or_struct, name_of_array/container_with_args`)`|This is a combination of `test_case_f` and `test_case_args_arr`. The fixture is available within the body of the test case as `fixture` and each argument is available as `arg`.|
|`test_case_args_f(`fixture_class_or_struct, arg_type ...comma-delimited args...`)`|This is a combination of `test_case_f` and `test_case_args`. The fixture is available within the body of the test case as `fixture` and each argument is available as `arg`.|

## List of Assertions

The following assertion macros are available:

|Assertion Macro|Description|
|-:|-|
|`assert_true(`expr`)`|Asserts that `expr` evaluates to a _truthy_ value|
|`assert_false(`expr`)`|Asserts that `expr` evaluates to a _falsey_ value|
|`assert_null(`expr`)`|Asserts that `expr` evaluates to the equivalent of `nullptr`|
|`assert_not_null(`expr`)`|Asserts that `expr` does not evaluate to the equivalent of `nullptr`|
|`assert_eq(`a, b`)`|Asserts that expressions `a` and `b` are equivalent based on the associated `==` operator|
|`assert_ne(`a, b`)`|Asserts that expressions `a` and `b` are not equivalent based on the associated `!=` operator|
|`assert_lt(`a, b`)`|Asserts that the expressions `a` is less than `b` based on the associated `<` operator|
|`assert_le(`a, b`)`|Asserts that the expressions `a` is less than or equal to `b` based on the associated `<=` operator|
|`assert_gt(`a, b`)`|Asserts that the expressions `a` is greater than `b` based on the associated `>` operator|
|`assert_ge(`a, b`)`|Asserts that the expressions `a` is greater than or equal to `b` based on the associated `>=` operator|
|`assert_close(`a, b, tol`)`|Asserts that the expressions `a` and `b` differ by no more than `tol`|
|`assert_throws(`expr, ex`)`|Asserts that the expressions `expr` throws the exception type `ex`|
|`assert_ieq(`a, b`)`|Asserts that the strings `a` and `b` are equal using a basic case-insensitve comparison|
|`test_fail(`msg`)`|Forcibly fails a test with the provided message. The message here is treated the same as the message in the `*_msg` versions of the assertion macros|
|`*_msg`|Except for `test_fail`, all of the above assertions have a corresponding version named with a `_msg` suffix that accepts an extra argument (can be a streaming expression). Example: `assert_ge_msg(1, (x - 1), "expected x (" << x << ") to be at least 2")`|

## Building the Unit Test Executable

The unit test executable is a simple command line executable built from source files that have `test_case*`es defined.
As mentioned earlier, one of the source files must `#define TUXX_DEFINE_TEST_MAIN` prior to `#include`ing the library header to make sure a `main()` entry point is created and test cases are run.
There is no limit on the number of `test_case*`s per file or across source files in a given test executable, except as limited by the system.

>NOTE: it is not possible to guarantee any order of test case execution for test cases across translation unit boundaries.
This is similar to the no-guaranteed-ordering of global object instantiation characteristic of C++.
This should be okay as there should typically be no reliance on any ordering of test cases.

>IMPORTANT: it is strongly recommended that the `#include <tuxx.hpp>` is the _last_ include in any given source file to ensure that it has no effects on any code under test.

Other than the standard libraries, there are no dependencies required.

You can decide how to incorporate your code under test.
Some choose to link to their built library and some choose to directly compile in the source files of the code under test into the unit test executable.
`tuxx` is indifferent on this choice. The actual name of the resulting executable is also up to you.

The resulting executable target can be added to a `cmake` build to make it runnable via `ctest` by using the `add_test` command.

## Some More Interesting Examples

### Test Cases with Fixtures

```C++
// Fixture types do not need an associated streaming operator.
struct my_fixture {
    int some_val{};
    my_fixture() {
        /* Some setup functionality */
        some_val = ...;
    }
    ~my_fixture() { /* Some teardown functionality */ }
};

test_case_f(my_fixture, my_test_case_name) {
    // A new instance of my_fixture is created prior to entry here.

    assert_ge(fixture.some_val, 0);

    // The my_fixture instance is destructed immediately after exit.
}
```

### Test Cases with Arguments

#### Arguments With a Pre-Defined Array

```C++
int const some_values[] = {1, 2, 3, 4};

// This will generate a unique test case for each argument. Each will have its
// own ID.
test_case_args_arr(my_test_case_with_a_arg, some_values) {
    // This will be run for each element of some_values

    // The argument is available as 'arg'.
    assert_lt(arg, 100);
}
```

#### Arguments With an Automatically Defined Array

A more convenient form of the above:

```C++
// This is equivalent behaviorally to the above.
test_case_args(my_test_case_with_a_arg, some_values, int, 1, 2, 3, 4) {
    ...
}
```

#### Arguments With a more interesting Argument Type

Suppose this is some code under test:

```C++
namespace my_lib {

struct point2d {
    point2d(int x, int y);
    int x() const;
    int y() const;
};

// Need to define a streaming operator for point2d...
inline
std::ostream& operator<<(
    std::ostream& os,
    point2d const& pt
) {
    // Just an example implementation
    return os << "point2d{x: " << pt.x() << ", y: " << pt.y() << '}';
}

}
```

Sample test case to validate `point2d`s are constructed as expected:

```C++
struct point2d__ctor_test_arg {
    my_lib::point2d pt;
    int expected_x;
    int expected_y;

    point2d__ctor_test_arg(int x, int y)
        : pt{x, y}, expected_x{x}, expected_y{y} {}
};

// NOTE: assuming the character type is 'char' here. See the section on
//       character types, below for more information.
inline
std::ostream& operator<<(
    std::ostream& os,
    point2d__ctor_test_arg const& arg
) {
    return os << "{pt: " << arg.pt
        << ", expected: {" << arg.expected_x << ',' arg.expected_y
        << "}}";
}

test_case_args(
    point2d__ctor_test,
    point2d__ctor_test_arg, // The type for the array of arguments
    // NOTE: in C++14 and later versions later, the type name can be omitted in
    // the entries in the following list.
    point2d__ctor_test_arg{0, 4},
    point2d__ctor_test_arg{6, -1},
    point2d__ctor_test_arg{-10003, 504301}
) {
    // Everything is the same as above (but with arg being a point2d__ctor_test_arg).
    assert_eq(arg.pt.x(), arg.expected_x);
    assert_eq(arg.pt.y(), arg.expected_y);
}
```

### Argument Test Cases with Fixtures

```C++
struct my_fixture {
    int some_val{};
    my_fixture() { /* Some setup functionality */ }
    ~my_fixture() { /* Some teardown functionality */ }
};

test_case_args_f(my_fixture, my_test_case_name, int, 1, 2, 3) {
    // A new instance of my_fixture is created prior to entry here.

    // 'fixture.some_val' is available here.

    // The argument (an int) is availble as 'arg'...

    // The my_fixture instance is destructed after exit.
}
```

### Running the Tests

Once the test executable is built, the test cases can be run by calling the executable directly and
optionally passing it arguments listed above.
By default all test cases will be run and the output will be printed to `stdout` by the selected
reporter.

#### Capturing `stdout`/`stderr`

Any writing to `stdout`/`stderr` while the test cases are running is captured and provided to the
reporter at the end.
Some reporters may output this in separate sections/fields in the report and some may ignore it.

## Character Types

This library works natively with `char` and `wchar_t`-based strings as long as the types being used
in the tests can be streamed to `char`/`wchar_t`-based streams, respectively.
The character type is usually auto-deduced by the library but can be overridden using the following `#define`s.
The `#define`s do not need an actual value.

|`#define`|Description|
|-|-|
|`TUXX_CHAR_TYPE_IS_CHAR`|The library will set its `char_type` alias to `char`. This is the default for all platforms unless the plaform is Windows and `UNICODE` is `#defined`.|
|`TUXX_CHAR_TYPE_IS_WCHAR_T`|The library will set its `char_type` to `wchar_t`. This is the default on Windows when `UNICODE` is defined.|

>NOTE: I attempted to provide seamless support for `char*_t` but it seems no standard library
implementations have full support for these character types.

## Mocking

This library currently does not support mocking.
If mocking (similar to something like `gmock`) were added, it would probably be done using a
separate project.

## Reporters

The library provides the following basic reporters that can be selected using command line options:

- The default reporter that prints an output intended to be used at development time.
Color is enabled by default but can be disabled.
Emojis can also be enabled.
- A `JSON` reporter that produces a machine-readable output that can be used in CI builds.
- A `JUNIT` reporter that produces a machine-readable output that can be used in CI builds.
- A tabular reporter that is also relatively machine-readable.
The intent of this output format is for it to be pastable into a spreadsheet.

### Custom Reporters

The reporter used by the library can be overridden by creating a subclass of
`tuxx::test_case_reporter` then `#define`ing
 `TUXX_USER_TEST_CASE_REPORTER_INIT(ostrm, tests_list, write_colorized, concurrency)`.
The exact way to do this will be described below.

The `tuxx::test_case_reporter` base "interface" is as follows:

```C++
namespace nyej {
namespace tuxx {

struct test_case_reporter {
    ...

    // Called at the beginning of the execution of all test cases.
    virtual void start() {}

    // Called before a given test case has started running. This is called
    // only if test cases are running sequentially (not in parallel). The purpose
    // of this is to show the test case name while the test case is running.
    // If this behavior is not important to your reporter or the tests are running
    // in parallel, use test_case_passed/test_case_failed to output all information
    // about the test case.
    virtual void test_case_starting(test_case_instance const& tc) {}

    virtual void test_case_passed(
        test_case_instance const& tc,
        std::chrono::steady_clock::duration const& elapsed
    ) = 0;

    // Called before every assert_* statement.
    virtual void test_case_assert(
        test_case_instance const& tc,
        char const* file,
        int line,
        char const* assert_name,
        char const* expr,
        string_type const& msg,
        char const* ex_info = nullptr
    ) {}

    virtual void test_case_failed(
        test_case_instance const& tc,
        test_case_failure_error const& err,
        std::chrono::steady_clock::duration const& elapsed
    ) = 0;

    // Called after all test cases have finished.
    virtual void end_test_cases(
        std::chrono::steady_clock::duration const& elapsed,
        std::vector<test_case_instance> const& failures
    ) = 0;

    // Called after end_test_cases to finalize the results and to provide any
    // captured stdout/stderr.
    virtual void finish(
        string_type const& stdout_data,
        string_type const& stderr_data
    ) = 0;
};

}
}
```

The `test_case_instance` and `test_case_failure_error` types are defined as:

```C++
namespace nyej {
namespace tuxx {

struct test_case_instance {
    char const* file{};             // File in which test case is defined
    int line{};                     // Line in the file on which test is defined
    string_type name;               // The textual name of the test case
    std::size_t idx{};              // The id/number of the test case
    string_type arg;                // The textual representation of the argument or empty if none
};

}
}
```

and

```C++
namespace nyej {
namespace nyej {

struct test_case_failure_error {
    char const* file;               // File where the assertion failuer occurred
    int line;                       // Line in the file on which the assertion failure occurred
    string_type detail;             // Detail about the assertion failure
};

}
}
```

All calls into the reporter methods are serialized to avoid any concurrency issues.

### Defining a Custome Reporter

Since this is a single header library, a specific sequence of steps needs to be done to properly
override the reporter.

Your reporter factory function will be passed a `nyej::tuxx::test_case_reporter_args` instance,
which is declared as:

```C++
namespace nyej {
namespace tuxx {

struct test_case_reporter_args {
    report_ostream_type* p_report_ostream{};
    test_case_instance const* p_test_cases; // Array of test cases.
    std::size_t n_tests;
    std::size_t concurrency;
    bool capture_output;        // Informative, no requirement to use. Indicates whether stdout/stderr are being captured.
    bool write_colorized;       // Informative, no requirement to use
    bool use_emojis;            // Informative, no requirement to use
};

}
}
```

Following are an examples of how to override the reporter.

#### Deriving a custom Class from `nyej::tuxx::test_case_reporter`

`my-test-main.cpp`:
```C++

... include headers for code under test ...

//------------------------------------------------------------------------------
// 1. Forward-declare a couple of tuxx types...
namespace nyej {
namespace tuxx {

struct test_case_reporter;
struct test_case_reporter_args;

}
}

//------------------------------------------------------------------------------
// 2. Forward-declare your reporter's factory function. (Do not 'use' the tuxx namespaces yet)
static nyej::tuxx::test_case_reporter* make_my_test_case_reporter(
    nyej::tuxx::test_case_reporter_args const&
);

//------------------------------------------------------------------------------
// 3. #define TUXX_USER_TEST_CASE_REPORTER_INIT to call your factory function
#define TUXX_USER_TEST_CASE_REPORTER_INIT(args) make_my_test_case_reporter(args)

//------------------------------------------------------------------------------
// 4. This must be done with a defined main()...
#define TUXX_DEFINE_TEST_MAIN

//------------------------------------------------------------------------------
// 5. #include tuxx
#include <tuxx.hpp>

... can also put test_cases here or later ...

//------------------------------------------------------------------------------
// 6. Define your test case reporter (or #include it here).
namespace {

struct my_test_case_reporter : nyej::tuxx::test_case_reporter {
    // Override the member functions you need
}; 

}

//------------------------------------------------------------------------------
// 7. Define your reporter's factory function
static nyej::tuxx::test_case_reporter* make_my_test_case_reporter(
    nyej::tuxx::test_case_reporter_args const& args
) {
    // NOTE: args will remain in scope for the lifetime of the created test reporter.
    return new my_test_case_reporter{
        /* Pass along the fields from args that make sense for your reporter */
    };
}
```

#### Creating a custom Reporter without Derivation

`my-test-main.cpp`:
```C++

... include headers for code under test ...

//------------------------------------------------------------------------------
// 1. Forward-declare a couple of tuxx types...
namespace nyej {
namespace tuxx {

struct test_case_reporter;
struct test_case_reporter_args;

}
}

//------------------------------------------------------------------------------
// 2. Forward-declare your reporter's factory function.
static nyej::tuxx::test_case_reporter* make_my_test_case_reporter(
    nyej::tuxx::test_case_reporter_args const&
);

//------------------------------------------------------------------------------
// 3. #define TUXX_USER_TEST_CASE_REPORTER_INIT for your factory function
#define TUXX_USER_TEST_CASE_REPORTER_INIT(args) make_my_test_case_reporter(args)

//------------------------------------------------------------------------------
// 4. This must be done with a defined main()...
#define TUXX_DEFINE_TEST_MAIN

//------------------------------------------------------------------------------
// 5. #include tuxx
#include <tuxx.hpp>

... can also put test_cases here or later ...

//------------------------------------------------------------------------------
// 6. Define your reporter's factory function
static nyej::tuxx::test_case_reporter* make_my_test_case_reporter(
    nyej::tuxx::test_case_reporter_args const& args
) {
    // NOTE: args will remain in scope for the lifetime of the created test reporter.

    // nyej::tuxx::test_case_reporter_fns is defined to allow you to avoid
    // having to create a new derived class. The lambdas provided here need
    // to match the order and signatures of the corresponding functions in
    // the nyej::tuxx::test_case_reporter interface. If empty or null
    // functions are provided, the library will simply skip these functions.
    return new nyej::tuxx::test_case_reporter_fns(
        // Pass lambdas here (only pass/fail are used in this example)
        nullptr,
        nullptr,
        nullptr,
        [...state you want captured...](
            test_case_instance const& tc,
            std::chrono::steady_clock::duration const& elapsed
        ) {
            ... your logic here
        },
        [...state you want captured...](
            test_case_instance const& tc,
            test_case_failure_error const& err,
            std::chrono::steady_clock::duration const& elapsed
        ) {
            ... your logic here
        },
        nullptr,
        nullptr
    );
}
```

>NOTE: Your custom test case reporter cannot simply write its results to `stdout`/`stderr` before
`finish` is called because these are captured before that point.
It will need to either use the output stream provided to it or accumulate all results then write
the results to `stdout`/`stderr` in `finish`.

## Exit Codes

If all test cases pass, the test executable will exit with a status code of `0`.
Otherwise, it will exit with a non-zero code.

## Colorized Output

The default test case reporter and the test listing support colorized output.
The library uses a default color scheme that uses the following `enum` for color values:

```C++
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
```

The colors used for output in the test listings and default reporter can be overridden by
overriding the following `#define`s:

|`#define`|Affects|Default|
|-:|-|-|
|`TUXX_SQ_BR_COLOR`|Color for `[]` characters used around test ids|`tuxx::color::fg_gray`|
|`TUXX_TEST_ID_COLOR`|Color for the test id|`tuxx::color::fg_cyan`|
|`TUXX_TEST_NAME_COLOR`|Color for the test case name|`tuxx::color::fg_yellow`|
|`TUXX_PAREN_COLOR`|Color for `()` characters, if the test case accepts a parameter|`tuxx::color::fg_blue`|
|`TUXX_PARAM_COLOR`|Color for parameter text, if the test case accepts a parameter|`tuxx::color::fg_magenta`|
|`TUXX_PUNCT_COLOR`|Color for `'.'` and `':'` characters in durations|`tuxx::color::fg_blue`|
|`TUXX_DURATION_COLOR`|Color for non-punctuation characters in durations|`tuxx::color::fg_white`|
|`TUXX_SUCCESS_COLOR`|Color success text|`tuxx::color::fg_green`|
|`TUXX_FAIL_COLOR`|Color for fail text|`tuxx::color::fg_red`|
|`TUXX_FILE_REF_COLOR`|Color for any file/line references in error detail output|`tuxx::color::fg_gray`|

## Emojis

The default test case reporter and the test listing support emojis if the `-e|--emojis` command line
argument is specified.

>NOTE: The support for emojis depends on your system and your system's terminal.
Emojis also often do not work at all if you are using `wchar_t`-based strings and may interfere with
the output.

The default emojis can be overridden using the following `#define`s:

|`#define`|Usage|Default|
|-:|-|:-:|
|`TUXX_EMOJI_PREFIX`|Printed at the beginning of each test case name|🧪|
|`TUXX_EMOJI_PASSED`|Printed to show a test case passed|✅|
|`TUXX_EMOJI_FAILED`|Printed to show a test case failed|❌|
|`TUXX_EMOJI_SUCCESS`|Printed as summary of the overall results when all cases pass|😎|
|`TUXX_EMOJI_FAILURE`|Printed as summary of the overall results when one or more cases fail|🤒|

>NOTE: these are simply literal strings and more than one character can be in them.
For example, one could `#define TUXX_EMOJI_PREFIX "this is the emoji prefix even though I am not using any emojis here"`.

## TUXX Library Tests and Examples

The library has several test cases and examples which use `cmake` to generate their builds.
This `CMakeLists.txt` is _not_ to be used for consuming the library - simply `#include` its header
after `#define`ing what you need.

To build everything, create a `build` directory somewhere and run:

```bash
$ cd build
$ cmake <path-to-tuxx-source>
$ cmake --build .
```

To run the unit tests for the library, run:

```bash
$ ctest .   # From the build directory
```

To run examples, cd into the `build/examples` directory and run the executables there.

If you run into any issues with building or running the tests or examples, please submit a ticket.

## Testes Platforms/Compilers

|Platform|Compiler|
|-|-|
|Ubuntu/Linux|G++ 11.4|
|Mac|Clang 11|

## Future Improvements

- More library tests
- More examples
- More asserts, improvements to existing asserts
- Windows tests to make sure all combinations of `main`/`wmain`/`WinMainA`/`WinMainW` work.
