/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef BASECLASS_H
#define BASECLASS_H

#include <json.h>
#include <mutex>
#include <atomic>

namespace glass3 {
namespace util {
/**
 * \brief util baseclass class - encapsulates the most basic setup and
 * configuration logic.
 *
 * Class encapsulating the setup and configuration logic, which is common to
 * most neic-glass3 classes outside of glasscore.  The baseclass is a
 * simple, almost abstract class that provides setup and clear interfaces and
 * keeps a pointer to the current configuration. The class also provides a
 * mutex for thread safety
 *
 * This class is intended to be extended by derived classes.
 */
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
	 * \warning WARNING! Uses the base class mutex available via getMutex(), locking
	 * getMutex(), or any other method where the class mutex is obtained and
	 * locked, before calling setup will cause a deadlock.
	 * \return returns true if successful.
	 */
	virtual bool setup(json::Object *config);


	/**
	 * \brief baseclass clear function
	 *
	 * The clear function for the baseclass class.
	 * Clears all configuration.
	 * \warning This function does not clean up memory (m_Config), it assumes
	 * that the original owner of the json::Object * will.
   * WARNING! Uses the base class mutex available via getMutex(), locking
   * getMutex(), or any other method where the class mutex is obtained and
   * locked, before calling setup will cause a deadlock.	 */
	virtual void clear();

	/* \brief Retrieves a pointer to the class member json::Object that holds
	 * the configuration.
	 *
	 * \warning Caller is responsible for ensuring that the configuration object
	 * pointer is not referenced after the original config object is destroyed
	 *
	 * \return returns a json::Object pointer to the configuration
	 */
	const json::Object * getConfig();

	/**
	 * \brief Retrieves the class member boolean flag indicating whether the
	 * class has been setup, set to true if setup was successful.
	 */
	bool getSetup();

 protected:
	/**
	 * \brief Retrieves a reference to the class member containing the mutex
	 * used to control access to class members
	 */
	std::mutex & getMutex();

	/**
	 * \brief A pointer to the json::Object that holds the configuration
	 */
	json::Object *m_Config;
  /* DK REVIEW 20180716
     you could define this type as:
     atomic<json::Object *> m_Config
     and completely do away with your mutex use in getConfig()
  */

	/**
	 * \brief the boolean flag indicating whether the class has been
	 * setup, set to true if setup was successful.
	 */
	std::atomic<bool> m_bIsSetup;

	/**
	 * \brief A mutex to control access to baseclass members
	 */
	std::mutex m_Mutex;
};
}  // namespace util
}  // namespace glass3
#endif  // BASECLASS_H
