#include <gtest/gtest.h>

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

// DK REVIEW 20180731
// Missing test cases for "input" class.
// seems like there's some functionality here to test.
// If it seems like too much overhead to create a test-class, and
// testing is instead done for input in a derived-class's test area
// then please add a comment here that identifies where the testing is done.
// Seems like there should still be some simple testing done here, since
// it lives in it's own mini sub-project.
