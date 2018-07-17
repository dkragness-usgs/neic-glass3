#include <baseclass.h>
#include <json.h>
#include <logger.h>
#include <mutex>

namespace glass3 {
namespace util {

// ---------------------------------------------------------BaseClass
BaseClass::BaseClass() {
	m_bIsSetup = false;
	m_Config = NULL;
}

// ---------------------------------------------------------~BaseClass
BaseClass::~BaseClass() {
	clear();
}

// ---------------------------------------------------------setup
bool BaseClass::setup(json::Object *config) {
	std::lock_guard<std::mutex> guard(getMutex());

	// null check
	if (config == NULL) {
		return (false);
	}

	// to be overridden by child classes
	m_Config = config;
	m_bIsSetup = true;

	return (true);
}

// ---------------------------------------------------------clear
void BaseClass::clear() {
	std::lock_guard<std::mutex> guard(getMutex());

	// to be overridden by child classes
	m_Config = NULL;
	m_bIsSetup = false;
}

// ---------------------------------------------------------getConfig
const json::Object * BaseClass::getConfig() {
	std::lock_guard<std::mutex> guard(getMutex());
  // DK REVIEW 20180716
  // not sure how many times you call getConfig(), but you could define
  // m_Config as atomic, instead of using the mutex here, since it's a pointer.
	return (m_Config);
}

// ---------------------------------------------------------getSetup
bool BaseClass::getSetup() {
	return (m_bIsSetup);
}

// ---------------------------------------------------------getMutex
std::mutex & BaseClass::getMutex() {
	return (m_Mutex);
}

}  // namespace util
}  // namespace glass3
