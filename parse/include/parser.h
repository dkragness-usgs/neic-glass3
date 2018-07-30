/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef PARSER_H
#define PARSER_H

#include <json.h>
#include <baseclass.h>
#include <string>
#include <memory>

namespace glass3 {

/**parse
 * \namespace util
 * \brief neic-glass3 namespace containing parsing classes and functions
 *
 * The neic-glass3 parse namespace contains various classes, and functions used
 * by other components of neic-glass3 to parse data.
 */
namespace parse {

/**
 * \brief glass parser class
 *
 * The glass parser class is a class encapsulating basic parsing
 * logic and validation.
 *
 * This class is intended to be extended by derived classes.
 */
class Parser : public glass3::util::BaseClass {
 public:
	Parser();

	/**
	 * \brief parser constructor
	 *
	 * The constructor for the parser class.
	 * Initializes members to provided values.
	 *
	 * \param defaultAgencyID - A std::string containing the agency id to
	 * use if one is not provided by the parsed message.
	 * \param defaultAuthor - A std::string containing the author to
	 * use if one is not provided by the parsed message.
	 */
	Parser(const std::string &defaultAgencyID,
			const std::string &defaultAuthor);

	/**
	 * \brief parser destructor
	 *
	 * The destructor for the parser class.
	 */
	virtual ~Parser();

	/**
	 * \brief parsing function
	 *
	 * Virtual parsing function to be overridden by deriving classes, used to
	 * parse input.  Function parser a string into a json::Object, returning
   * a shared_ptr to the resulting object.
	 *
   // DK 2018/07/30 review
   // if defaultAgencyID and/or defaultAuthor are expected to be applied in
   // any way, as part of this function, then please indicate that.
	 * \param input - The std::string to parse
	 * \return Returns a shared pointer to the json::Object containing
	 * the parsed data.
	 */
	virtual std::shared_ptr<json::Object> parse(const std::string &input) = 0;

	/**
	 * \brief validation function
	 *
	 * Virtual validation function to be overridden by deriving classes, used
	 * to validate parsed input
   // DK 2018/07/30 review
   // I don't understand what this function is supposed to do, or where it is to be called in the scheme of parsing.
   // Could you PLEASE give some more info and add an example (hypothetical or otherwise).

   // DK 2018/07/30 review
   // if defaultAgencyID and/or defaultAuthor are expected to be applied in
   // any way, as part of this function, then please indicate that.
   *
	 * \param input - A shared pointer to a json::Object containing the data to
	 * validate.
	 * \return Returns true if valid, false otherwise.
	 */
	virtual bool validate(std::shared_ptr<json::Object> &input) = 0;  // NOLINT

	/**
	 * \brief Function to retrieve the name of the default agency id
	 *
	 * This function retrieves the name of default agency id, this name is used
	 * in parsing
	 *
	 * \return A std::string containing the  agency id
	 */
	const std::string& getAgencyId();

	/**
	 * \brief Function to set the name of the default agency id
	 *
	 * This function sets the name of the default agency id, this name is used
	 * in parsing
	 *
	 * \param id = A std::string containing the default agency id to set
	 */
   // DK 2018/07/30 review
   // id should be const, or you should explain how modifying it is part of your plan for world domination!
  // Also, I would think codacy might complain if you don't pass it as a reference.  Pretty sure it's gonna be short
  // so doubt it will make much performance difference.
  void setAgencyId(std::string id);

	/**
	 * \brief Function to retrieve the name of the default author
	 *
	 * This function retrieves the name of the default author, this name is used
	 * in parsing
	 *
	 * \return A std::string containing the author
	 */
	const std::string& getAuthor();

	/**
	 * \brief Function to set the name of the default author
	 *
	 * This function sets the name of the default author, this name is used in
	 * parsing
	 *
	 * \param author = A std::string containing the default author to set
	 */
   // DK 2018/07/30 review
   // author should be const, or you should explain how modifying it is part of your plan for world domination!
  void setAuthor(std::string author);

 private:
	/**
	 * \brief A std::string containing the default agency id to
	 * use in parsing if one is not provided.
	 */
	std::string m_AgencyID;

	/**
	 * \brief A std::string containing the default author to
	 * use in parsing if one is not provided.
	 */
	std::string m_Author;
};
}  // namespace parse
}  // namespace glass3
#endif  // PARSER_H
