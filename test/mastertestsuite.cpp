#include "tests.hpp"
#include <boost/test/included/unit_test.hpp>
using namespace boost::unit_test;

#define UNUSED(x) (void)(x)

test_suite* createOpcodeTestSuite() {
    test_suite* opTests = BOOST_TEST_SUITE("opcode_test_suite");
    opTests->add(BOOST_TEST_CASE( &Tests::cpuMessage ));
    opTests->add(BOOST_TEST_CASE( &Tests::cpuLdaAddressingTests ));
    opTests->add(BOOST_TEST_CASE( &Tests::cpuMathTests ));
    opTests->add(BOOST_TEST_CASE( &Tests::cpuBitwiseTests ));
    opTests->add(BOOST_TEST_CASE( &Tests::cpuStatusTests ));
    opTests->add(BOOST_TEST_CASE( &Tests::cpuJumpBranchTests ));
    opTests->add(BOOST_TEST_CASE( &Tests::cpuCompareTests ));
    opTests->add(BOOST_TEST_CASE( &Tests::cpuStackTests ));
    return opTests;
}

test_suite* createCpuDiagTestSuite() {
    test_suite* cpuDiagTest = BOOST_TEST_SUITE("cpu diagnostic test");
    cpuDiagTest->add(BOOST_TEST_CASE( &Tests::nesCpuTest ));
    return cpuDiagTest;
}

test_suite* createPpuTestSuite() {
    test_suite* ppuTest = BOOST_TEST_SUITE(" ppu tests");
    ppuTest->add(BOOST_TEST_CASE(&Tests::ppuRegisterTests));
    return ppuTest;
}

test_suite* init_unit_test_suite(int argc, char* argv[]) {
    UNUSED(argv);
    bool allowOp = true, allowDiag = true, allowPpu = true;
    if (argc == 1) {
        Tests::testenv();
        return nullptr;
    }
    if (allowPpu) {
        framework::master_test_suite().add(createPpuTestSuite());
    }
    if (allowOp) {
        test_suite* opTests = createOpcodeTestSuite();
        framework::master_test_suite().add(opTests);
        std::cout << "\n";
    }
    if (allowDiag) {
        test_suite* cpuDiag = createCpuDiagTestSuite();
        framework::master_test_suite().add(cpuDiag);
        std::cout << "\n";
    }
    return nullptr;
}

