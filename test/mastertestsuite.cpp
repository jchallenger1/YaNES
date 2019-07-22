#include "tests.hpp"
#include <boost/test/included/unit_test.hpp>
using namespace boost::unit_test;

#define UNUSED(x) (void)(x)

test_suite* createOpcodeTestSuite() {
    test_suite* opTests = BOOST_TEST_SUITE("opcode_test_suite");
    opTests->add(BOOST_TEST_CASE( &cpuLdaAddressingTests ));
    opTests->add(BOOST_TEST_CASE( &cpuMathTests ));
    opTests->add(BOOST_TEST_CASE( &cpuBitwiseTests ));
    opTests->add(BOOST_TEST_CASE( &cpuStatusTests ));
    opTests->add(BOOST_TEST_CASE( &cpuJumpBranchTests ));
    opTests->add(BOOST_TEST_CASE( &cpuCompareTests ));
    opTests->add(BOOST_TEST_CASE( &cpuStackTests ));
    return opTests;
}

test_suite* createCpuDiagTestSuite() {
    test_suite* cpuDiagTest = BOOST_TEST_SUITE("cpu diagnostic test");
    cpuDiagTest->add(BOOST_TEST_CASE( &nesCpuTest ));
    return cpuDiagTest;
}

test_suite* init_unit_test_suite(int argc, char* argv[]) {
    UNUSED(argc); UNUSED(argv);
    constexpr bool allowOp = true, allowDiag = true;
    if (allowOp) {
        std::cout << " --- Running Opcode Test Cases ---\n";
        test_suite* opTests = createOpcodeTestSuite();
        framework::master_test_suite().add(opTests);
        std::cout << "\n";
    }
    if (allowDiag) {
        std::cout << " --- Running CPU Diagnostics, Nestest ---\n";
        test_suite* cpuDiag = createCpuDiagTestSuite();
        framework::master_test_suite().add(cpuDiag);
        std::cout << "\n";
    }
    return nullptr;
}

