/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef CCPARSER_H
#define CCPARSER_H

#include <json.h>
#include <parser.h>
#include <string>
#include <memory>

namespace glass3 {
namespace parse {
/**
 * \brief glass cross correlation parser class
 *
 * The glass cross correlation parser class is a class encapsulating the logic
 * for parsing a cross correlation message and validating the results.
 *
 * This class inherits from the parser class
 */
class CCParser : public glass3::parse::Parser {
 public:
	/**
	 * \brief ccparser constructor
	 *
	 * The constructor for the ccparser class.
	 * Initializes members to provided values.
	 *
	 * \param newAgencyID - A std::string containing the agency id to
	 * use if one is not provided.
	 * \param newAuthor - A std::string containing the author to
	 * use if one is not provided.
	 */

 // DK Review 2018/07/30
 // these input parameters should be named default*  instead of new*
 // since they represent the default values for messages that don't contain
 // the respective data.
 // This fix was already made in parser.h and should also be made here.
 // This comment applies to ALL DERIVED PARSERS
	CCParser(const std::string &newAgencyID, const std::string &newAuthor);

	/**
	 * \brief ccparser destructor
	 *
	 * The destructor for the ccparser class.
	 */
	~CCParser();

	/**
	 * \brief cross correlation parsing function
	 *
	 * Parsing function that parses cross correlation formatted strings
	 * into json::Objects

   // DK 2018/07/30 review
   // Include a detailed description(or link to one) of the format being parsed.
   // This comment applies to ALL DERIVED PARSERS

   // if defaultAgencyID(m_AgencyID) and/or defaultAuthor(m_Author) are expected to be applied in
   // any way, as part of this function, then please indicate that.
   // This comment applies to ALL DERIVED PARSERS

	 *
	 * \param input - The cross correlation formatted std::string to parse
	 * \return Returns a pointer to the json::Object containing
	 * the data parsed from the input string.
	 */
	std::shared_ptr<json::Object> parse(const std::string &input) override;

	/**
	 * \brief cross correlation validation function
	 *
	 * Validates a cross correlation object
   // DK 2018/07/30 review
   // I don't understand what this function is supposed to do, or where it is to be called in the scheme of parsing.
   // Could you PLEASE give some more info and add an example (especially since this is a concrete example of the function).
   // This comment applies to ALL DERIVED PARSERS

   // DK 2018/07/30 review
   // if defaultAgencyID(m_AgencyID) and/or defaultAuthor(m_Author) are expected to be applied in
   // any way, as part of this function, then please indicate that.
   // This comment applies to ALL DERIVED PARSERS
   *
	 * \param input - A pointer to a json::Object containing the data to
	 * validate.
	 * \return Returns true if valid, false otherwise.
	 */
	bool validate(std::shared_ptr<json::Object> &input) override;
};
}  // namespace parse
}  // namespace glass3
#endif  // CCPARSER_H
