/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <ctime>
#include <string>

namespace util {
/**
 * \brief Convert time from epoch time to ISO8601
 *
 * Convert the given epoch time from decimal seconds to an
 * ISO8601 time string
 // ========================
 // DK REVIEW 20180627
 // give an example of an ISO8601 time string
 // provide atleast one link (somewhere in this file to the specification for ISO8601)
 // if ISO 8601 doesn't specify precision, then
 // indicate how rounding/truncation will be done
 // and at what level.
 // ========================

 * \param epochtime - A double containing the epoch time
 * \return returns an ISO8601 formatted time std::string
 */
std::string convertEpochTimeToISO8601(double epochtime);

/**
 * \brief Convert time from epoch time to ISO8601
 *
 * Convert the given epoch time from decimal seconds to an
 * ISO8601 time string
 // ========================
 // DK REVIEW 20180627
 // From the doc it doesn't seem like "now" has anything to do with now, 
 // but is instead tEpochTime or epochtime
 // ========================
 * \param now - A time_t containing the epoch time
 * \param decimalseconds - A an optional double containing the decimal seconds
 * \return returns an ISO8601 formatted time std::string
 */
std::string convertEpochTimeToISO8601(time_t now, double decimalseconds = 0);

/**
 * \brief Convert time from date time to ISO8601
 *
 * Convert the given date time string to an
 * ISO8601 time string
 // ========================
 // DK REVIEW 20180627
 // Input is a string, but indicated to be a time.
 // No format is given for the input string.  Please specify.
 // ========================
 * \param TimeString - A std::string containing the date time
 * \return returns an ISO8601 formatted time std::string
 */
std::string convertDateTimeToISO8601(const std::string &TimeString);

/**
 * \brief Convert time from date time to epoch time
 *
 * Convert the given datetime string to an epoch time
 * \param TimeString - A std::string containing the date time
 * \return returns a double variable containing the epochtime
 */
 // ========================
 // DK REVIEW 20180627
 // Input is a string, but indicated to be a time.
 // No format is given for the input string.  Please specify.
 // ========================
double convertDateTimeToEpochTime(const std::string &TimeString);

/**
 * \brief Convert time from ISO8601 time to epoch time
 *
 * Convert the given ISO8601 string to an epoch time
 * \param TimeString - A std::string containing the ISO8601 time
 * \return returns a double variable containing the epochtime
 */
double convertISO8601ToEpochTime(const std::string &TimeString);
}  // namespace util
#endif  // TIMEUTIL_H
