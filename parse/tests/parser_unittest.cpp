#include <parser.h>
#include <gtest/gtest.h>

#include <string>
#include <memory>

#define TESTAGENCYID "US"
#define TESTAUTHOR "glasstest"

// DK Review 2018/07/30
// Splain why you are creating this class.
// Something like:
// glass3::parse::Parser is an abstract class and
// must be derived into a concrete class before use.
// create parserstub class to have a concrete class
// that can be used to teach the Parser abstract class functions.
class parserstub : public glass3::parse::Parser {
 public:
	parserstub()
			: glass3::parse::Parser() {
	}

  // DK Review 2018/07/30
  // these input parameters should be named default*  instead of new*
  // since they represent the default values for messages that don't contain
  // the respective data.
  // This fix was already made in parser.h and should also be made here.
  // This comment applies to ALL DERIVED PARSERS
  parserstub(const std::string &newagencyid, const std::string &newauthor)
			: glass3::parse::Parser(newagencyid, newauthor) {
	}

	~parserstub() {
	}

	std::shared_ptr<json::Object> parse(const std::string &input) override {
		return (NULL);
	}

	bool validate(std::shared_ptr<json::Object> &input) override {
		return (true);
	}
};

// tests to see if parser constructs correctly
TEST(ParserTest, Construction) {
	// default constructor
	parserstub * Parser = new parserstub();
	std::string emptyString = "";

	// assert that agencyid is ok
	ASSERT_STREQ(Parser->getAgencyId().c_str(), emptyString.c_str())<<
	"AgencyID check";

	// assert that author is ok
	ASSERT_STREQ(Parser->getAuthor().c_str(), emptyString.c_str())<< "Author check";

	ASSERT_TRUE(Parser->parse(emptyString) == NULL)<< "parse returns null";

	std::shared_ptr<json::Object> nullData;
	ASSERT_TRUE(Parser->validate(nullData))<< "validate returns true";

	std::string agencyid = std::string(TESTAGENCYID);
	std::string author = std::string(TESTAUTHOR);

	// advanced constructor
	parserstub * Parser2 = new parserstub(agencyid, author);

  // DK Review 2018/07/30
  // Add some comment about how setAgencyId() and setAuthor() don't need to be explicitly
  // tested here, since they are both called/tested by the "parser" parameterized constructor
  // that's called by the parserstub() paramaterized constructor above.

	// assert that agencyid is ok
	ASSERT_STREQ(Parser2->getAgencyId().c_str(), agencyid.c_str())<<
	"AgencyID check";

	// assert that author is ok
	ASSERT_STREQ(Parser2->getAuthor().c_str(), author.c_str())<< "Author check";

	// cleanup
	delete (Parser);
	delete (Parser2);
}
