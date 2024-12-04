#include <gtest/gtest.h>

#include "paths.h" 

std::string test_dir;
bool update_results;

int main( int argc, char* argv[] ) {
#ifdef ENABLE_GTEST_DEBUG_MODE
  ::testing::GTEST_FLAG(break_on_failure) = true;
  ::testing::GTEST_FLAG(catch_exceptions) = false;
#endif
	::testing::InitGoogleTest( &argc, argv );
	test_dir = TEST_DATA_DIR;
	update_results = true;
	return RUN_ALL_TESTS();
}

