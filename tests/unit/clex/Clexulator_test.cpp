#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

/// What is being tested:
#include "casm/clex/Clexulator.hh"

/// Dependencies

/// What is being used to test it:
#include <boost/filesystem.hpp>

using namespace CASM;

BOOST_AUTO_TEST_SUITE(ClexulatorTest)

BOOST_AUTO_TEST_CASE(MakeClexulatorTest) {
  namespace fs = boost::filesystem;

  Clexulator clexulator("test_Clexulator",
                        "tests/unit/clex",
                        RuntimeLibrary::default_compile_options() + " --std=c++11 -Iinclude",
                        RuntimeLibrary::default_so_options() + " -lboost_filesystem -lboost_system");

  BOOST_CHECK_EQUAL(clexulator.corr_size(), 75);

}

BOOST_AUTO_TEST_SUITE_END()
