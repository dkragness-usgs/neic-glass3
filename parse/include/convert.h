/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef CONVERT_H
#define CONVERT_H

#include <json.h>
#include <string>
#include <memory>

namespace glass3 {
namespace parse {

/**
 * \brief json hypo conversion function
 *
 * The function is used to convert a hypo message to the json detection format
 // DK 2018/07/30 review
 // Links to definition of "hypo" and "json detection format" messages?
 *
 * \param data - A pointer to a json::Object containing the data to be
 * converted.
 // DK 2018/07/30 review
 // data is missing a name in the function definition (1st param is unnamed)
 // The param formerly known as "data" should be a shared_ptr to a const object,
 // unless you have great plans to change it.
 * \param outputAgencyID - A std::string containing the agency id to use for
 * output
 // DK 2018/07/30 review - how/where/why is outputAgencyID used?
 * \param outputAuthor - A std::string containing the author to use for output
 // DK 2018/07/30 review - how/where/why is outputAuthor used?
 * \return Returns a string containing the converted json detection, empty
 * string otherwise

 // DK 2018/07/30 review 
 // not sure how frequently this function is called.  Worth making return a shared pointer
 // to avoid generating the string a second time via copy constructor on return value?

 */
std::string hypoToJSONDetection(std::shared_ptr<json::Object>,
								const std::string &outputAgencyID,
								const std::string &outputAuthor);

/**
 * \brief json cancel conversion function
 *
 * The function is used to convert a cancel message to the json retraction
 * format
 // DK 2018/07/30 review
 // Links to definition of "cancel message" and "json retraction format" messages?
 *
 * \param data - A pointer to a json::Object containing the data to be
 * converted.
 // DK 2018/07/30 review
 // see hypoToJSONDetection() comments regarding AgencyID and Author

 * \param outputAgencyID - A std::string containing the agency id to use for
 * output
 * \param outputAuthor - A std::string containing the author to use for output
 * \return Returns a string containing the converted json detection, empty
 * string otherwise
 */
std::string cancelToJSONRetract(std::shared_ptr<json::Object>,
								const std::string &outputAgencyID,
								const std::string &outputAuthor);

/**
 * \brief json station list conversion function
 *
 * The function is used to convert a site list message to the json station list
 * format
 // DK 2018/07/30 review
 // Links to definition of "site list message" and "json station list format" messages?
 *
 * \param data - A pointer to a json::Object containing the data to be
 * converted.
 // DK 2018/07/30 review
 // data is missing a name in the function definition (1st param is unnamed)
 // The param formerly known as "data" should be a shared_ptr to a const object,
 // unless you have great plans to change it.
 * \return Returns a string containing the converted json detection, empty
 * string otherwise
 // DK 2018/07/30 review
 // returns a whojahwhat?
 */
std::string siteListToStationList(std::shared_ptr<json::Object>);

/**
 * \brief json station list conversion function
 *
 * The function is used to convert a site lookup message to the json station
 * info request format
 // DK 2018/07/30 review
 // Links to definition of "site lookup message" and "json station info request format" messages?
 *
 * \param data - A pointer to a json::Object containing the data to be
 * converted.
 // DK 2018/07/30 review
 // data is missing a name in the function definition (1st param is unnamed)
 // The param formerly known as "data" should be a shared_ptr to a const object,
 // unless you have great plans to change it.
 * \param outputAgencyID - A std::string containing the agency id to use for
 * output
 * \param outputAuthor - A std::string containing the author to use for output
 // DK 2018/07/30 review
 // see hypoToJSONDetection() comments regarding AgencyID and Author
 * \return Returns a string containing the converted json detection, empty
 * string otherwise
 // DK 2018/07/30 review
 // returns a whojahwhat?
 */
std::string siteLookupToStationInfoRequest(std::shared_ptr<json::Object>,
											const std::string &outputAgencyID,
											const std::string &outputAuthor);

}  // namespace parse
}  // namespace glass3
#endif  // CONVERT_H
