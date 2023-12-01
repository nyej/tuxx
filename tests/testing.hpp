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

#include <iostream>
#include <string>

#define TUXX_UNDER_TEST__

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

inline
std::string simple_narrow(std::wstring const& s) {
    std::string ret;
    ret.reserve(s.length());
    for (auto c : s) {
        ret += static_cast<char>(c);
    }
    return ret;
}

// Forward-declared entry point
template <typename Ch> inline
void testing_main(
    int argc,
    Ch* argv[]
);

template <typename Ch> inline
int testing_main_wrapper(
    int argc,
    Ch* argv[]
) {
    using namespace std;
    try {
        testing_main(argc, argv);
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
            vector<TSTR> argv;
            for (auto& arg : args) {
                argv.push_back(arg.c_str());
            }
            return testing_main_wrapper(static_cast<int>(argv.size()), &argv[0]);
        }
        #define TESTING_MAIN_IS_DEFINED
    #elif defined(TUXX_IS_WINDOWS_UNICODE)
        int wmain(
            int argc,
            wchar_t* argv[]
        ) {
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
        return testing_main_wrapper(argc, argv);
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
                    wstring arg;
                    // Do a simple char -> wchar_t conversion here...
                    for (auto j = 0; argv[i][j]; ++j) {
                        arg += static_cast<wchar_t>(argv[i][j]);
                    }
                    if (i) {
                        cmdLine += arg;
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
