#ifndef TESTS_HPP
#define TESTS_HPP

#include <string>
#include <iostream>
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

struct Tests {
    // ---- Cpu Test Opcode Functions ----
    // Functions are defined in optests.cpp
    static void cpuLdaAddressingTests();
    static void cpuMathTests();
    static void cpuBitwiseTests();
    static void cpuStatusTests();
    static void cpuJumpBranchTests();
    static void cpuCompareTests();
    static void cpuStackTests();
    static void cpuMessage();

    // ---- NesTest Functions ----
    static void nesCpuTest();

    static void testenv();

};


#endif // TESTS_HPP
