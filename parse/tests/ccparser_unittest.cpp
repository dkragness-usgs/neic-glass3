#include <ccparser.h>
#include <gtest/gtest.h>

#include <string>
#include <memory>

#define TESTCCSTRING   "2015/03/23 23:53:47.630 36.769 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 P 2015/03/23 23:53:50.850 0.7663822 0.65" // NOLINT
// DK REVIEW 20180730  - possibly some extra test strings that have problems (such as those you might see from a non-NEIC contributor, that aren't so pronounced.
#define TESTCCSTRING2  "2015/03/23 23: 3:47.630 36.769 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 P 2015/03/23 23:53:50.850 0.7663822 0.65" // NOLINT
#define TESTCCSTRING3  "2015/03/23 23:53:47 36.769 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 P 2015/03/23 23:53:50.850 0.7663822 0.65" // NOLINT
#define TESTCCSTRING4  "2015/03/23 23:53:47.630 36.769 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 P 2015/03/23 23: 3:50.850 0.7663822 0.65" // NOLINT
#define TESTCCSTRING5  "2015/03/23 23:53:47.630 36.769 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 P 2015/03/23 23:53:50 0.7663822 0.65" // NOLINT
#define TESTCCSTRING6  "2015/03/23 23:53:47.630 36.769 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 X 2015/03/23 23:53:50 0.7663822 0.65" // NOLINT
#define TESTCCSTRING7  "2015/03/23 23:53:47.630 36.769 -98.019 5.0 1.2677417 1234 GS OK032 HHZ 00 S 2015/03/23 23:53:50 0.7663822 0.65" // NOLINT
#define TESTCCSTRING8  "2015/03/23 23:AB:47.630 36.769 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 P 2015/03/23 23:53:50.850 0.7663822 0.65" // NOLINT
#define TESTCCSTRING9  "2015/03/23 23:53:47.630 36.769 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 P ABC 123 0.7663822 0.65" // NOLINT
#define TESTFAILSTRING "2015/03/23 -98.019 5.0 1.2677417 mblg GS OK032 HHZ 00 P 2015/03/23 23:53:50.850 0.7663822 0.65" // NOLINT
#define TESTAGENCYID "US"
#define TESTAUTHOR "glasstest"

class CCParser : public ::testing::Test {
 protected:
	virtual void SetUp() {
		agencyid = std::string(TESTAGENCYID);
		author = std::string(TESTAUTHOR);

		Parser = new glass3::parse::CCParser(agencyid, author);
	}

	virtual void TearDown() {
		// cleanup
		delete (Parser);
	}

  // DK Review 2018/07/30
  // why not m_agencyid?  Seems to me that if you're gonna go for a code-format standard, it should be implemented across the project.
  // and if you're not gonna make it standard across the project, then it should not be listed.
  // perhaps you can implement it for all major code components, but not for test cases.  random thoughts....
  std::string agencyid;
	std::string author;
	glass3::parse::CCParser * Parser;
};

// tests to see CC(Correlation) parser constructs correctly
TEST_F(CCParser, Construction) {
	// assert that agencyid is ok
	ASSERT_STREQ(Parser->getAgencyId().c_str(), agencyid.c_str())<<
	"AgencyID check";

	// assert that author is ok
	ASSERT_STREQ(Parser->getAuthor().c_str(), author.c_str())
	<< "Author check";

}

// test correlations
TEST_F(CCParser, CorrelationParsing) {
	std::string ccstring = std::string(TESTCCSTRING);

	std::shared_ptr<json::Object> CCObject = Parser->parse(ccstring);

	// parse the cross correlation
	ASSERT_FALSE(CCObject == NULL)<< "Parsed cross correlation not null.";

	// validate the cross correlation
	ASSERT_TRUE(Parser->validate(CCObject))<<
	"Parsed cross correlation is valid";
}

// test failure
TEST_F(CCParser, FailTest) {
	std::string failstring = std::string(TESTFAILSTRING);

	std::shared_ptr<json::Object> FailObject = Parser->parse(failstring);

	// parse the bad data
	ASSERT_TRUE(FailObject == NULL)<< "Parsed fail string is null.";

	// validate the bad data
	ASSERT_FALSE(Parser->validate(FailObject))<<
	"Parsed failstring is not valid";

	// parse empty string
	FailObject = Parser->parse("");

	// parse the empty string
	ASSERT_TRUE(FailObject == NULL)<< "Parsed empty string is null.";

	// verify that validate fails for a NULL pointer.
	ASSERT_FALSE(Parser->validate(FailObject))<<
	"Parsed empty string is not valid";

  // DK Review 2018/07/30
  // Is there any way we could do a partial-data test, where 
  // Parser->validate(Object) should fail, even though Object is non-null(i.e. parse() returned an object instead of a NULL pointer)?
}
