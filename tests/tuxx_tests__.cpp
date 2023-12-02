#include <vector>
#include <tuxx.hpp>

// We're not #define'ing TUXX_DEFINE_TEST_MAIN here so we need to add an
// implementation for this
std::vector<::nyej::tuxx::test_case_instance>& ::nyej::tuxx::detail::tests__() {
    static std::vector<::nyej::tuxx::test_case_instance> tests;
    return tests;
}
