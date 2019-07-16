#ifndef TESTS_HPP
#define TESTS_HPP

#include <string>
#include <boost/test/unit_test.hpp>

inline constexpr void ckPassFail(const bool&& b, const std::string& str) {
    if (!b) {
        BOOST_FAIL(str);
    }
}

inline constexpr void ckPassErr(const bool&& b, const std::string& str) noexcept {
    if (!b) {
        BOOST_ERROR(str);
    }
}
// ---- Cpu Test Opcode Functions ----
// Functions are defined in optests.cpp
extern void cpuLdaAddressingTests();
extern void cpuMathTests();
extern void cpuBitwiseTests();
extern void cpuStatusTests();
extern void cpuJumpBranchTests();
extern void cpuCompareTests();
extern void cpuStackTests();


#endif // TESTS_HPP
