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

test_suite* init_unit_test_suite(int argc, char* argv[]) {
    UNUSED(argc); UNUSED(argv);
    test_suite* opTests = createOpcodeTestSuite();


    framework::master_test_suite().add(opTests);

    return nullptr;
}

