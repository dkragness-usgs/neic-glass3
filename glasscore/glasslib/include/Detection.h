/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef DETECTION_H
#define DETECTION_H

#include <json.h>
#include <memory>
#include <string>
#include <vector>

namespace glasscore {

// forward declarations
class CSite;
class CHypo;

/**
 * \brief glasscore detection class
 *
 * The CDetection class is a class that encapsulates the processing a detection
 * message from an external source, including parseing the message, creating
 * a hypo from the message, and adding the hypo to the hypo list.
 */
class CDetection {
 public:
	/**
	 * \brief CDetection constructor
	 */
	CDetection();

	/**
	 * \brief CDetection destructor
	 */
	~CDetection();

	/**
	 * \brief CDetection clear function
	 */
	void clear();

	/**
	 * \brief CDetection communication receiving function
	 *
	 * The function used by CDetection to receive communication (such as
	 * configuration or input data), from outside the glasscore library, or it's
	 * parent CGlass.
	 *
	 * Supports processing Detection messages
	 *
	 * \param com - A pointer to a json::object containing the
	 * communication.
	 * \return Returns true if the communication was handled by CDetection,
	 * false otherwise
	 */
	bool dispatch(std::shared_ptr<json::Object> com);

	/**
	 * \brief Process detection message
	 *
	 * Receives an incoming 'Detection' type message and does on of two things.
	 * First it checks to see if there is another hypocenter that is near enough
	 * in space and time to be considered to be the same event. If so, it
	 * schedules the existing hypocenter for processing. If no existing
	 * hypocenter fits the information in the detection message, a new
	 * hypocenter is created and scheduled for processing.
	 *
	 * \param detectionMessage -  A pointer to a json::object containing the
	 * incoming Detection message
	 */
	bool processDetectionMessage(
			std::shared_ptr<json::Object> detectionMessage);

	/**
	 * \brief Get the CGlass pointer used by this class for configuration lookups
	 * \return Return a pointer to the CGlass class used by this class
	 */
	const CGlass* getGlass() const;

	/**
	 * \brief Set the CGlass pointer used by this class for configuration lookups
	 * \param glass - a pointer to the CGlass class used by this class
	 */
	void setGlass(CGlass* glass);

 private:
	/**
	 * \brief A pointer to the parent CGlass class, used to get configuration
	 * values and access other parts of glass3
	 */
	CGlass * m_pGlass;

	/**
	 * \brief A recursive_mutex to control threading access to CDetection.
	 * NOTE: recursive mutexes are frowned upon, so maybe redesign around it
	 * see: http://www.codingstandard.com/rule/18-3-3-do-not-use-stdrecursive_mutex/
	 * However a recursive_mutex allows us to maintain the original class
	 * design as delivered by the contractor.
	 */
	mutable std::recursive_mutex m_DetectionMutex;
};
}  // namespace glasscore
#endif  // DETECTION_H
