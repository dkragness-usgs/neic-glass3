#ifndef OUTPUTINTERFACE_H
#define OUTPUTINTERFACE_H

#include <json.h>
#include <memory>

// ========================
// DK REVIEW 20180627
// "util" is a generic name.  I think most of your code, if not all, should 
// live under a glass3 or similar namespace, and then you can
// use a namespace alias in your code in case you ever want to move it.
//  something like:
//     namespace glass3 { namespace util }}
//
// and 
//     namespace util = glass3::util;	
// or
//     namespace my_util = glass3::util;	
// and then reference stuff using my_util::
// 
// Just a thought.
// ========================

namespace util {

/**
 * \interface ioutput
 // ========================
 // DK REVIEW 20180627
 // Needs more descriptive comment.  
 // ========================
 * \brief output data interface
 *
 // ========================
 // DK REVIEW 20180627
 // Maybe include an example.  This comment is much better than the one in inputinterface,
 // but still seems kinda nebulous.  I think it would help if you could give at least one
 // example of how/when/why it would be used.
 // ========================

 * The ioutput interface is implemented by concrete classes that
 * output data from the associator, providing a standardized
 * interface for other classes in glass to request the sending
 * of output data via the sendtooutput() function.
 */
struct iOutput {
	/**
	 * \brief Send output data
	 *
	 * This pure virtual function is implemented by a concrete class to
	 * support sending output data.
	 *
	 * \param data - A pointer to a json::object containing the output
	 * data.
	 */
	virtual void sendToOutput(std::shared_ptr<json::Object> data) = 0; // NOLINT
};
}  // namespace util
#endif  // OUTPUTINTERFACE_H
