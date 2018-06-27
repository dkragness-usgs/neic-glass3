/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef THREADBASECLASS_H
#define THREADBASECLASS_H

#include <baseclass.h>
#include <thread>
#include <mutex>
#include <string>

namespace util {
/**
 * \brief util threadbaseclass class
 *
 * The util threadbaseclass class is a class encapsulating the threading
 * logic.  The baseclass class supports creating, starting, stopping,
 * and monitoring a work thread.
 *
 * This class inherits from util::baseclass
 * This class is intended to be extended by derived classes.
 */
 // ========================
 // DK REVIEW 20180627
 // My attitude is that documentation for library Public methods/attributes should be very explicit and descriptiive, and either provide
 // an example or a reference to an example code block at the class level.
 // Protected and Private doc can be a bit more sparse and assume a little more skill and familiarity
 // on the part of the developer.
 // There should not be documentation that says "getter for  <protected member>"  or "sets <protected member>"
 // Should be something like, "retrieves the value of internal flag that tracks (or controls) ..."
 // ========================

 // ========================
 // DK REVIEW 20180627
 // Needs example code that shows how to use the class
 // ========================

class ThreadBaseClass : public util::BaseClass {
 public:
	/**
	 * \brief threadbaseclass constructor
	 *
	 * The constructor for the threadbaseclass class.
	 * Initilizes members to default values.
	 */
	ThreadBaseClass();

	/**
	 * \brief threadbaseclass advanced constructor
 // ========================
 // DK REVIEW 20180627
 // \brief needs better description.
 // ========================
 *
	 * The advanced constructor for the threadbaseclass class.
	 * Initializes members to default values.
	 * \param threadname - A std::string containing the desired
	 * name of the work thread.
	 * \param sleeptimems - An integer value containing the amount of
	 * time to sleep between work() calls in the work thread
	 */
	ThreadBaseClass(std::string threadname, int sleeptimems);

	/**
	 * \brief threadbaseclass destructor
	 *
	 * The destructor for the threadbaseclass class.
	 */
	~ThreadBaseClass();

	/**
	 * \brief thread start function
	 *
	 * Allocates and starts the work thread.
	 * \return returns true if successful.
   // ========================
   // DK REVIEW 20180627
   // Need to be explicit about when the thread comes about at an OS object level.
   // when does the thread object get created?  Here?
   // ========================
   */
	virtual bool start();

	/**
	 * \brief thread stop function
	 *
	 * Stops, waits for, and deallocates the work thread.
	 * \return returns true if successful.
	 */
	virtual bool stop();

	/**
	 * \brief thread check function
	 *
	 * Checks to see if the work thread is still running.
	 * \return returns true if thread is still running.
   // ========================
   // DK REVIEW 20180627
   // What does "running" mean in this instance?  The OS Thread is still alive?
   // The worker thread is still busy executing some piece of work and not idle?
   // ========================
   */
	virtual bool check();

	/**
	 * \brief set thread sleep time
	 *
	 * Sets the amount of time to sleep between
	 * work() calls in the work thread
	 */
	void setSleepTime(int sleeptimems);

	/**
	 * \brief get thread sleep time
	 *
	 * Gets the amount of time to sleep between
	 * work() calls in the work thread.
	 * \return Returns the amount of time to sleep.
	 */
	int getSleepTime();

	/**
	 * \brief threadbaseclass work function
	 *
	 * Virtual work function to be overridden by deriving classes
	 * \return Returns true if work was successful, false otherwise
	 */
   // ========================
   // DK REVIEW 20180627
   // Public method requires better documentation, with either an example or a reference
   // to an example code block at the class level.
   // ========================
  virtual bool work() = 0;

	/**
	 * \brief threadbaseclass is running
	 *
	 * Checks to see if the work thread is running
	 * \return Returns true if m_bRunWorkThread is true, false otherwise
	 */
   // ========================
   // DK REVIEW 20180627
   // Public method requires better documentation, with either an example or a reference
   // to an example code block at the class level.
   // ========================
  virtual bool isRunning();

	/**
	 * \brief threadbaseclass is set work check
	 *
	 * Signifies that the work thread is still alive by setting
	 * m_bCheckWorkThread to true
	 */
   // ========================
   // DK REVIEW 20180627
   // Public method requires better documentation, with either an example or a reference
   // to an example code block at the class level.
   // ========================
  void setWorkCheck();

	/**
	 *\brief getter for m_iCheckInterval
	 */
   // ========================
   // DK REVIEW 20180627
   // Public method requires better documentation, with either an example or a reference
   // to an example code block at the class level.
   // ========================
  int getCheckInterval() const {
		return m_iCheckInterval;
	}

	/**
	 *\brief getter for m_bCheckWorkThread
	 */
   // ========================
   // DK REVIEW 20180627
   // Public method requires better documentation, with either an example or a reference
   // to an example code block at the class level.
   // ========================
  bool getCheckWorkThread() const {
		return m_bCheckWorkThread;
	}

	/**
	 *\brief getter for m_bStarted
	 */
   // ========================
   // DK REVIEW 20180627
   // seems like a function with the name getstarted
   // should be much more active!
   // ========================
   // ========================
   // DK REVIEW 20180627
   // Public method requires better documentation, with either an example or a reference
   // to an example code block at the class level.
   // ========================
  bool getStarted() const {
		return m_bStarted;
	}

	/**
	 *\brief getter for m_sThreadName
	 */
   // ========================
   // DK REVIEW 20180627
   // Only way to set the threadname is in one of the constructors.  Intentional?
   // ========================
  const std::string& getThreadName() const {
		return m_sThreadName;
	}

	/**
	 *\brief getter for tLastCheck
	 */
   // ========================
   // DK REVIEW 20180627
   // Public method requires better documentation, with either an example or a reference
   // to an example code block at the class level.
   // ========================
  time_t getLastCheck() const {
		return tLastCheck;
	}

 protected:
	/**
	 * \brief the std::string containing the name of the work thread
	 */
	std::string m_sThreadName;

	/**
	 * \brief the integer interval in seconds after which the work thread
	 * will be considered dead. A negative check interval disables thread
	 * status checks
	 */
	int m_iCheckInterval;

	/**
	 * \brief threadbaseclass work loop
	 *
	 * threadbaseclass work loop, runs while m_bRunWorkThread
	 * is true, calls work(), reports run status with setworkcheck()
	 */
	void workLoop();

 private:
	/**
	 * \brief the std::thread pointer to the work thread
	 */
	std::thread *m_WorkThread;

	/**
	 * \brief boolean flag indicating whether the work thread should run
	 */
	bool m_bRunWorkThread;

	/**
	 * \brief boolean flag indicating whether the work thread has been started
	 */
	bool m_bStarted;

	/**
	 * \brief boolean flag used to check thread status
	 */
	bool m_bCheckWorkThread;

	/**
	 * \brief the std::mutex for m_bCheckWorkThread
	 */
	std::mutex m_CheckMutex;

	/**
	 * \brief the time_t holding the last time the thread status was checked
	 */
   // ========================
   // DK REVIEW 20180627
   // why is this not M_tLastCheck ? 
   // ========================
  time_t tLastCheck;

	/**
	 * \brief integer variable indicating how long to sleep in milliseconds in
	 * the work thread
	 */
	int m_iSleepTimeMS;
};
}  // namespace util
#endif  // THREADBASECLASS_H
