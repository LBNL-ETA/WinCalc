#include <gtest/gtest.h>

#include "paths.h" 

std::string test_dir;

int main( int argc, char* argv[] ) {
#ifdef ENABLE_GTEST_DEBUG_MODE
  ::testing::GTEST_FLAG(break_on_failure) = true;
  ::testing::GTEST_FLAG(catch_exceptions) = false;
#endif
	::testing::InitGoogleTest( &argc, argv );
	test_dir = argv[1];
	return RUN_ALL_TESTS();
}

