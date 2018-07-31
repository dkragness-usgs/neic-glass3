#include <gpickparser.h>
#include <gtest/gtest.h>

#include <string>
#include <memory>

#define TESTGPICKSTRING1 "228041013 22637648 1 BOZ BHZ US 00 20150303000044.175 P -1.0000 U  ? m 1.050 2.650 0.0 0.000000 3.49 0.000000 0.000000" // NOLINT
#define TESTGPICKSTRING2 "228041013 22637649 1 BOZ BHZ US 00 20150303000045.175 P -1.0000 D  i r 1.050 2.650 0.0 0.000000 3.49 0.000000 0.000000" // NOLINT
#define TESTGPICKSTRING3 "228041013 22637650 1 BOZ BHZ US 00 20150303000046.175 P -1.0000 U  e l 1.050 2.650 0.0 0.000000 3.49 0.000000 0.000000" // NOLINT
#define TESTGPICKSTRING4 "228041013 22637651 1 BOZ BHZ US 00 20150303000047.175 P -1.0000 U  q e 1.050 2.650 0.0 0.000000 3.49 0.000000 0.000000" // NOLINT
#define TESTGPICKSTRING5 "228041013 22637652 1 BOZ BHZ US 00 20150303000048.175 P -1.0000 U  q U 1.050 2.650 0.0 0.000000 3.49 0.000000 0.000000" // NOLINT

#define TESTFAILSTRING1 "228041013 22637648 1 BOZ BHZ US 00 P -1.0000 U  ? r 1.050 2.650 0.0 0.000000 3.49 0.000000 0.000000" // NOLINT
#define TESTFAILSTRING2 "228041013 22637648 1 BOZ BHZ US 00 20150303000044.175 P -1.0000 U  ? m BBC 2.650 0.0 0.000000 3.49 0.000000 0.000000" // NOLINT
#define TESTFAILSTRING3 "228041013 22637649 1 BOZ BHZ US 00 20150303000045.175 P -1.0000 D  i r 1.050 2.650 0.0 0.000000 AFW 0.000000 0.000000" // NOLINT
#define TESTFAILSTRING4 "228041013 22637652 1 BOZ BHZ US 00 20150303000048.175 P -1.0000 U  q U 1.050 2.650 0.0 0.000000 3.49 ABC EASY_AS_123" // NOLINT

#define TESTAGENCYID "US"
#define TESTAUTHOR "glasstest"

class GPickParser : public ::testing::Test {
 protected:
	virtual void SetUp() {
		agencyid = std::string(TESTAGENCYID);
		author = std::string(TESTAUTHOR);

		Parser = new glass3::parse::GPickParser(agencyid, author);
	}

	virtual void TearDown() {
		// cleanup
		delete (Parser);
	}

	std::string agencyid;
	std::string author;
	glass3::parse::GPickParser * Parser;
};

// tests to see gpick parser constructs correctly
TEST_F(GPickParser, Construction) {
	// assert that agencyid is ok
	ASSERT_STREQ(Parser->getAgencyId().c_str(), agencyid.c_str())<<
	"AgencyID check";

	// assert that author is ok
	ASSERT_STREQ(Parser->getAuthor().c_str(), author.c_str())
	<< "Author check";
}

// test picks
TEST_F(GPickParser, PickParsing) {
	std::string pickstring1 = std::string(TESTGPICKSTRING1);
	std::string pickstring2 = std::string(TESTGPICKSTRING2);
	std::string pickstring3 = std::string(TESTGPICKSTRING3);
	std::string pickstring4 = std::string(TESTGPICKSTRING4);
	std::string pickstring5 = std::string(TESTGPICKSTRING5);

	// pick 1
	std::shared_ptr<json::Object> PickObject = Parser->parse(pickstring1);

	// parse the pick from string
	ASSERT_FALSE(PickObject == NULL)<< "Parsed pick 1 not null.";

	// validate the pick
	ASSERT_TRUE(Parser->validate(PickObject))<< "Parsed pick 1 is valid";

	// pick 2
	std::shared_ptr<json::Object> PickObject2 = Parser->parse(pickstring2);

  // parse the pick from string
	ASSERT_FALSE(PickObject2 == NULL)<< "Parsed pick 2 not null.";

	// validate the pick
	ASSERT_TRUE(Parser->validate(PickObject2))<< "Parsed pick 2 is valid";

	// pick 3
	std::shared_ptr<json::Object> PickObject3 = Parser->parse(pickstring3);

	// parse the pick from string
	ASSERT_FALSE(PickObject3 == NULL)<< "Parsed pick 3 not null.";

	// validate the pick
	ASSERT_TRUE(Parser->validate(PickObject3))<< "Parsed pick 3 is valid";

	// pick 4
	std::shared_ptr<json::Object> PickObject4 = Parser->parse(pickstring4);

	// parse the pick from string
	ASSERT_FALSE(PickObject4 == NULL)<< "Parsed pick 4 not null.";

	// validate the pick
	ASSERT_TRUE(Parser->validate(PickObject4))<< "Parsed pick 4 is valid";

	// pick 5
	std::shared_ptr<json::Object> PickObject5 = Parser->parse(pickstring5);

  // parse the pick from string
	ASSERT_FALSE(PickObject5 == NULL)<< "Parsed pick 2 not null.";

	// validate the pick
	ASSERT_TRUE(Parser->validate(PickObject5))<< "Parsed pick 2 is valid";


  // DK REVIEW 20180730  
  // test field values for the fields recognized by the function?  Or overkill?
  // In most projects that I've worked on, I just run the code once in the debugger
  // and eyeball the results from one run.  No regression protection, but much quicker.
  // Which approach would you like to take here?
  // index 0 is the author/logo, use
  // index 1 is the pick id, need
  // index 2 is the gpick version, ignore
  // indexes 3-6 is the SCNL, need
  // index 7 is the arrival time, need
  // index 8 is the phase, need
  // index 9 is the error window half width, need
  // index 10 is the polarity, need
  // index 11 is the onset, need
  // index 12 is the picker type, need
  // index 13 is the high pass frequency, need
  // index 14 is the low pass frequency, need
  // index 15 is the back azimuth, ignore
  // index 16 is the slowness, ignore
  // index 17 is the SNR, need
  // index 18 is the amplitude, need
  // index 19 is the period, need
}

// test failure
TEST_F(GPickParser, FailTest) {
	std::string failstring1 = std::string(TESTFAILSTRING1);
	std::string failstring2 = std::string(TESTFAILSTRING2);
	std::string failstring3 = std::string(TESTFAILSTRING3);
  std::string failstring4 = std::string(TESTFAILSTRING4);

	// test missing time
	std::shared_ptr<json::Object> FailObject = Parser->parse(failstring1);

	// parse the bad data
	ASSERT_TRUE(FailObject == NULL)<< "Parsed fail string is null.";

	// validate the bad data
	ASSERT_FALSE(Parser->validate(FailObject))<<
	"Parsed failstring is not valid";

	// test bad filter
	FailObject = Parser->parse(failstring2);

	// parse the bad data
	ASSERT_FALSE(FailObject == NULL)<< "Parsed fail string is not null.";

	// validate the bad data
	ASSERT_TRUE(Parser->validate(FailObject))<<
	"Parsed failstring is valid";

	// test bad snr
	FailObject = Parser->parse(failstring3);

	// parse the bad data
	ASSERT_FALSE(FailObject == NULL)<< "Parsed fail string is not null.";

	// validate the bad data
	ASSERT_TRUE(Parser->validate(FailObject))<<
	"Parsed failstring is valid";

  // test bad amplitude - hopefully triggering the exception handler in parse
  FailObject = Parser->parse(failstring4);

  // parse the bad data
  ASSERT_FALSE(FailObject == NULL) << "Parsed fail string is not null.";

  // validate the bad data
  ASSERT_TRUE(Parser->validate(FailObject)) <<
    "Parsed failstring is valid";

  // test empty string
	FailObject = Parser->parse("");

	// parse the empty string
	ASSERT_TRUE(FailObject == NULL)<< "Parsed empty string is null.";

	// validate the bad data
	ASSERT_FALSE(Parser->validate(FailObject))<<
	"Parsed empty string is not valid";
}
