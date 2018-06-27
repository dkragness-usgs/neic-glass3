/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef BASECLASS_H
#define BASECLASS_H

#include <json.h>

/**
 * @namespace util
 * The namespace containing a collection of utility classes and functions.
 */
namespace util {
  // ========================
  // DK REVIEW 20180621
  // I suggested verbage change for the below comment.  I don't know if it makes since to define
  // a separate setup and clear interface that's implemented by the baseclass class.
  // I think the thing I most wanted chagned about the comment was something to say "basic"
  // as in, it doesn't do much, cause I was confused about what it did.
  // ========================

/**
 * \brief util baseclass class - encapsulates the most basic setup and configuration logic. 
 *
 * Class encapsulating the setup and configuration
 * logic.  The baseclass is a simple, almost abstract class that provides setup and clear interface-ish
 * properties and keeps a pointer to the current configuration
 *
 * This class is intended to be extended by derived classes.
 */

 // ========================
 // DK REVIEW 20180621
 // I don't understand...
 // I get that this is a somewhat abstract class, but could we be a little less abstract about the functionality.
 // Does it just manage configuration information? 
 // a clause for each of (  DK CLEANUP Stopped here...
 // ========================

 // ========================
 // DK REVIEW 20180621
 // Class does not appear to be threadsafe.
 // ========================



class BaseClass {
 public:
	/**
	 * \brief baseclass constructor
	 *
	 * The constructor for the baseclass class.
	 * Initializes members to default values.
	 */
	BaseClass();

	/**
	 * \brief baseclass destructor
	 *
	 * The destructor for the baseclass class.
	 */
	virtual ~BaseClass();

	/**
	 * \brief baseclass configuration function
	 *
	 * The this function configures the baseclass class
	 * \param config - A pointer to a json::Object containing to the
	 * configuration to use
	 * \return returns true if successful.
	 */
	virtual bool setup(json::Object *config);

	/**
	 * \brief baseclass clear function
	 *
	 * The clear function for the baseclass class.
	 * Clears all configuration
	 */
	virtual void clear();

	/**
	 * \brief A pointer to the json::Object that holds the configuration
	 */
   // ========================
   // DK REVIEW 20180621
   // should not everything below here be "protected" in scope, or "private" if you're super paranoid?
   // ========================
  json::Object *m_Config;

	/**
	 * \brief the boolean flag indicating whether the class has been
	 * setup.
	 */
   // ========================
   // DK REVIEW 20180626
   // What is the significance of m_bIsSetup.  Is it true anytime setup() has been called, or
   // only if setup completes successfully.
   // I noticed that m_bIsSetup is not 
   // ========================
  bool m_bIsSetup;
};
}  // namespace util
#endif  // BASECLASS_H
