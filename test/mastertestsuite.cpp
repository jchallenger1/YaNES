#include "tests.hpp"
#include <boost/test/included/unit_test.hpp>
#include <boost/program_options.hpp>
using namespace boost::unit_test;
namespace po = boost::program_options;

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
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Produce help message")
            ("env", "Use the predefault programming environment for YaNES's classes")
            ("opcode", "Performs opcode tests")
            ("nestest", "Peform nesTest cpu tests")
            ("ppureg", "Performs register tests for the ppu")
            ("all,a", "Performs all tests")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("env")) {
        Tests::testenv();
        return nullptr;
    }

    if (vm.empty() || vm.count("help")) {
        std::cout << "\t USAGE: testing -- [OPTION]...\n";
        std::cout << desc << "\n";
        return nullptr;
    }

    if (vm.count("all") || vm.count("a")) {
        framework::master_test_suite().add(createOpcodeTestSuite());
        framework::master_test_suite().add(createCpuDiagTestSuite());
        framework::master_test_suite().add(createPpuTestSuite());
        return nullptr;
    }

    if (vm.count("opcode")) {
        test_suite* opTests = createOpcodeTestSuite();
        framework::master_test_suite().add(opTests);
    }
    if (vm.count("nestest")) {
        test_suite* cpuDiag = createCpuDiagTestSuite();
        framework::master_test_suite().add(cpuDiag);
    }
    if (vm.count("ppureg")) {
        framework::master_test_suite().add(createPpuTestSuite());
    }

    return nullptr;
}

