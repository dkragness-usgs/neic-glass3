/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef OUTPUT_H
#define OUTPUT_H

 #include <json.h>
#include <threadbaseclass.h>
#include <outputinterface.h>
#include <associatorinterface.h>
#include <cache.h>
#include <queue.h>
#include <threadpool.h>

#include <thread>
#include <mutex>
#include <future>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

namespace glass3 {
namespace output {

  // DK REVIEW 20180802
  // The biggest problem is the lack of (meaningful) documentation, including a description of the outputs framework,
  // the messages exchanged with the associator, the internal messages, and what all the threads and data-stores are for.
  // I feel like output could be streamlined and enhanced, but maybe that's just me being curmudgeouny...  I'd like to hold off until after review of 
  // glass core, before making any big suggestions.
  // Some possibilities might include adding more info to event messages(something akin to Hydra notifier messages), so that full Hypo
  // contents are only needed at publication time, not during pub evaluation.
  // Having output directly access the current version of an event from glass core, or access it via an associator callback function.
  // Separating the event output stream(where seconds are critically important) from station-info related output streams (where multi-minute delays seem reasonable)
  // Storing more publication info than just version.
  // I also would pull any of the WriteOutput() stuff from the dequeue thread, moving it to a work thread.  I understand there are extenuating circumstances for
  // Expire, but I think those can be overcome.
  // 
  // Could also make pub algorithm more advanced, taking into account time since detection, as well as change in number of supporting data, location change, or location quality.
  // Again, I'd like to avoid proposing or making any big changes until after reviewing Glass core
  // DK REVIEW 20180802

/**
 * \brief glass output class
 *
 * The glass output class is a thread class encapsulating the detection output
 * logic.  The output class handles output messages from from glasscore,

 // DK REVIEW 20180801 - I feel like this class is woefully under-commented.  Seems like there should be at least 4 paragraphs in this section alone:
 * Purpose or purposes:  Theres a little bit of this above, but it could be broken out in much more detail.
 * Inputs: (who(what other threads/objects) does this class/thread get data from, how, what types, and in what formats
 * Outputs: does this class/thread send data to (objects/threads/processes/files), how, what types, and in what formats.
 * Threads: what various thread or threadgroups does this object employ.  List them and give a not-too-brief description of what each does (and how it
 *          interacts with the other threads)
 *
 * output inherits from the threadbaseclass class.
 * output implements the ioutput interface.
 */

class output : public glass3::util::iOutput,
		public glass3::util::ThreadBaseClass {
 public:
	/**
	 * \brief output constructor
	 *
	 * The constructor for the output class.
	 * Initializes members to default values.
	 */
	output();

	/**
	 * \brief output destructor
	 *
	 * The destructor for the output class.
	 * Stops the work thread
	 */
	~output();

	/**
	 * \brief output configuration function
	 *
	 * The function configures the output class, and the tracking cache it
	 * contains.
	 *
	 * \param config - A pointer to a json::Object containing the
	 * configuration to use
   // DK REVIEW 20180801 - what happens if setup() a second time
   // or if it's called after clear()?
	 * \return returns true if successful.
	 */
	bool setup(std::shared_ptr<const json::Object> config) override;

	/**
	 * \brief output clear function
	 *
	 * The clear function for the output class.
	 * Clears all configuration, clears and reallocates the message queue and
	 * cache
	 */
	void clear() override;

	/**
	 * \brief output message sending function
	 *
	 * The function (from ioutput) used to send communication to output.
	 *
	 * \param message - A json::Object containing the message to send to output.
	 */
	void sendToOutput(std::shared_ptr<json::Object> message) override;

	/**
	 * \brief work thread start function
	 *
	 * Overrides ThreadBaseClass::start(). Creates a thread object to run the
	 * checkEventsLoop() function, and starts it, setting m_bEventThreadStarted to be
	 * true, then calls ThreadBaseClass::start() to start the ThreadBaseClass
	 * work thread
   // DK REVIEW 20180801 - is this comment up to date, or did any of this change
   // at the same time  util was redone with different thread state flags?
	 *
	 * \return returns true if successful, false if the thread creation failed
	 * or if a thread had already been started
	 */
	bool start() override;

	/**
	 * \brief work thread stop function
	 *
	 * Overrides ThreadBaseClass::stop(). Stops, waits for, and deletes the
	 * thread that runs the checkEventsLoop() function,
	 * setting m_bEventThreadStarted, m_bEventThreadRunning, and m_bEventThreadHealth to
	 * false, then calls ThreadBaseClass::stop() to stop the ThreadBaseClass
	 * work thread
	 *
	 * \return returns true if successful, false if the thread is not created and
	 * running
	 */
	bool stop() override;

	/**
	 * \brief output heath check function
	 *
	 * Checks to see if the thread pool is still running, calls
	 * threadbaseclass::healthCheck for worker thread monitoring.
	 * \return returns true if thread pool is still running.
	 */
	bool healthCheck() override;

  // DK REVIEW 20180801 - seems like you have a lot of this
  // AgencyID and Author stuff.
  // seems like you should have a baseclass that implements
  // storage, manipulation, and retrieval of these values
  // so that you don't have to have the same code in 8 places
  // (this isn't mag modules, you know....)

  // 
	/**
	 * \brief Function to set the name of the output agency id
	 *
	 * This function sets the name of the output agency id, this name is used in
	 * generating output
	 *
	 * \param id = A std::string containing the agency id to set
	 */
	void setOutputAgency(std::string agency);

	/**
	 * \brief Function to retrieve the name of the output agency id
	 *
	 * This function retrieves the name of output agency id, this name is used
	 * in generating output
	 *
	 * \return A std::string containing the agency id
	 */
	const std::string getOutputAgencyId();

	/**
	 * \brief Function to set the name of the output author
	 *
	 * This function sets the name of the output author, this name is used in
	 * generating output
	 *
	 * \param author = A std::string containing the author to set
	 */
	void setOutputAuthor(std::string author);

	/**
	 * \brief Function to retrieve the name of the output author
	 *
	 * This function retrieves the name of the output author, this name is used
	 * in generating output
	 *
	 * \return A std::string containing the author
	 */
	const std::string getOutputAuthor();

  // DK REVIEW 20180801 - I don't understand the next two functions
  // What are they for, and why are they in outputs?

	/**
	 * \brief Function to set the delay in requesting the site list
	 *
	 * This function sets the delay in seconds before requesting glass core's
	 * current sitelist. A negative delay indicates that the site list should
	 * not be requested
	 *
	 * \param delay = An integer value containing the delay in seconds
	 */
	void setSiteListDelay(int delay);

	/**
	 * \brief Function to retrieve the delay in requesting the site list
	 *
	 * This function retrieves the delay in seconds before requesting glass
	 * core's current sitelist
	 *
	 * \return Returns an integer value containing the delay in seconds
	 */
	int getSiteListDelay();

  // DK REVIEW 20180801 - I don't understand the next two functions
  // What are they for, and why are they in outputs?

	void setStationFile(std::string filename);

	const std::string getStationFile();

	/**
	 * \brief Function to set the interval to generate informational reports
	 *
	 * This function sets the interval in seconds between logging informational
	 * reports on output throughput and performance
	 *
	 * \param interval = An integer value containing the interval in seconds
	 */
	void setReportInterval(int interval);

	/**
	 * \brief Function to retrieve the interval to generate informational reports
	 *
	 * This function retrieves the interval in seconds between logging
	 * informationalreports on output throughput and performance
	 *
	 * \return Returns an integer value containing the interval in seconds
	 */
	int getReportInterval();

	/**
	 * \brief Function to set the associator interface pointer
	 *
	 * This function sets the associator interface pointer used by output to
	 * communicate with the associator via the sendToAssociator() function
   // DK REVIEW 20180801 - I don't understand the associator/outputs relationship.
   // Do they each have a pointer to the other?  Or does output track associator
   // or vice versa?
   *
	 * \param associator = A pointer to an object that implements the
	 * glass3::util::iAssociator interface.
	 */
	void setAssociator(glass3::util::iAssociator* associator);

	/**
	 * \brief Function to get the associator interface pointer
	 *
	 * This function gets the associator interface pointer used by output to
	 * communicate with the associator via the sendToAssociator() function
	 *
	 * \return Returns a pointer to an object that implements the
	 * glass3::util::iAssociator interface.
	 */
	glass3::util::iAssociator* getAssociator();

	/**
	 * \brief Function to set the publish on expiration flag
	 *
	 * This function sets the boolean flag that indicates whether output should
	 * generate a detection message when it receives an expiration notification
	 * from the associator.
	 *
	 * \param pub = A boolean flag indicating whether to generate a detection
	 * message on expiration
	 */
	void setPubOnExpiration(bool pub);

	/**
	 * \brief Function to retrieve the publish on expiration flag
	 *
	 * This function retrieves the boolean flag that indicates whether output
	 * should generate a detection message when it receives an expiration
	 * notification from the associator.
	 *
	 * \return Returns a boolean flag indicating whether to generate a detection
	 * message on expiration
	 */
	int getPubOnExpiration();

	/**
	 * \brief Function to retrieve the publication times
	 *
	 * This function retrieves the publication times in seconds used to determine
	 * when to generate detection messages for events. An event will not generate
	 * a message if it has not changed, and will not generate a message if all
	 * publication times have passed (unless m_bPubOnExpiration is set to true)
	 *
	 * \return Returns a std::vector of integers containing the times in seconds
	 * since initial report that events should generate detection messages
	 */
	std::vector<int> getPubTimes();
  // DK REVIEW 20180801  - this is liable to be a pretty short list, so probably not worth
  // doing, but ideally, this would be a const pointer or reference return to avoid
  // copy-constructing the vector.

	/**
	 * \brief Function to set the publication times
	 *
	 * This function sets the publication times in seconds used to determine when
	 * to generate detection messages for events. An event will not generate
	 * a message if it has not changed, and will not generate a message if all
	 * publication times have passed (unless m_bPubOnExpiration is set to true)
	 *
	 * \param pubTimes = A std::vector of integers containing the times in seconds
	 * since initial report that events should generate detection messages
	 */
	void setPubTimes(std::vector<int> pubTimes);  // DK REVIEW 20180802 - function is never called.  addPubTime() is always called piecemeal instead.  Should be removed.
  // DK REVIEW 20180801  - this is liable to be a pretty short list, so probably not worth
  // doing, but ideally, this would be a const reference param to avoid
  // double copy-constructing the vector.

	/**
	 * \brief Function to add a single publication time to the list
	 *
	 * This function adds a single publication time in seconds to the list used
	 * to determine when to generate detection messages for events. An event
	 * will not generate a message if it has not changed, and will not generate
	 * a message if all publication times have passed (unless m_bPubOnExpiration
	 * is set to true)
	 *
	 * \param pubTime = An integer containing a times in seconds since initial
	 * report that events should generate detection messages
	 *
	 */
	void addPubTime(int pubTime);

	/**
	 * \brief Function to clear the publication times
	 *
	 * This function clears the publication times in seconds used to determine
	 * when to generate detection messages for events.
	 */
	void clearPubTimes();

	/**
	 * \brief Checks to see if the event thread should still be running
	 *
   // DK REVIEW 20180801 - grammar check
	 * This function checks to see if the event thread should still running by
	 * returning the value of m_bRunWorkThread.
	 * \return Returns true if the thread should still running, false if it
	 * has been stopped
	 */
  // DK REVIEW 20180802
  // Seems like this should be more of a protected function, as the "EventThread" is
  // an internal thing and it's running should be part of an "is this object healthy" check.
	bool isEventThreadRunning();

  // DK REVIEW 20180801 - Define "TrackingData" preferably
  // near the top of the class, where there's a request to 
  // define all the output messages.
  // Would be good to define any communication that this thread/class has with
  // other parts of the world.

	/**
	 * \brief add data to the output tracking cache
	 *
	 * Add the detection data to the cache of data pending for output
	 *
	 * \param data - A pointer to a json::Object containing the detection data.
	 * \return Returns true if successful, false otherwise
	 */
	bool addTrackingData(std::shared_ptr<json::Object> data);

	/**
	 * \brief get data from the output tracking cache by id
	 *
	 * Get the detection data from the cache of data pending for output based
	 * on a provided id
	 *
	 * \param id - A std::string contaning the id of the detection data to
	 * retrieve from the cache
	 * \return Returns the data if found, null otherwise
	 */
	std::shared_ptr<const json::Object> getTrackingData(std::string id);

	/**
	 * \brief get data from the output tracking cache
	 *
   // DK REVIEW 20180802  -  I'm not sure I understand what this function is supposed
   // to do.  Could you take another attempt at documenting it with a little more
   // detail and clarity.  PLEASE.
	 * Get the first available detection data from the cache of data pending for
	 * output that is ready
	 *
	 * \return Returns a pointer to the json::Object containing the detection
	 * data ready for output, NULL if no data found that is ready.
	 */
	std::shared_ptr<const json::Object> getNextTrackingData();

	/**
	 * \brief check if data is in output tracking cache
	 *
	 * Check to see if given detection data is already in the output tracking
	 * cache.
	 *
	 * \param data - A pointer to a json::Object containing the detection data.
	 * \return Returns true if the data is in the cache, false otherwise
	 */
	bool haveTrackingData(std::shared_ptr<json::Object> data);  // DK REVIEW 20180802 - this function is not used outside of unit testing - consider moving it into unit-test code.

	/**
	 * \brief check if data is in output tracking cache by id
	 *
	 * Check to see if given detection data is already in the output tracking
	 * cache by id.
	 *
	 * \param ID - A std::string containing the id of the detection data to check.
	 * \return Returns true if the data is in the cache, false otherwise
	 */
	bool haveTrackingData(std::string ID);   // DK REVIEW 20180802 - this function is not used outside of unit testing - consider moving it into unit-test code.

	/**
	 * \brief remove data from the output tracking cache
	 *
	 * Remove the provided detection data from the output tracking cache.
	 *
	 * \param data - A pointer to a json::Object containing the detection data.
	 * \return Returns true if successful, false otherwise
	 */
	bool removeTrackingData(std::shared_ptr<const json::Object> data);

	/**
	 * \brief remove data from the output tracking cache by id
	 *
	 * Remove the provided detection data from the output tracking cache. by id
	 *
	 * \param ID - A std::string containing the id of the detection data to remove.
	 * \return Returns true if successful, false otherwise
	 */
	bool removeTrackingData(std::string ID);

	/**
	 * \brief clear output tracking cache
	 *
	 * Clear all detection data from the output tracking cache.
	 */
	void clearTrackingData();

	/**
	 * \brief check to see if detection data is ready for output
	 *
	 * Check the given detection data to see if it is ready for output
	 *
	 * \param data - A pointer to the json::Object containing the detection
	 * data to check
	 * \return Returns true if the data is ready, false if not.
	 */
	bool isDataReady(std::shared_ptr<const json::Object> data);

	/**
	 * \brief check to see if detection data has changed
	 *
	 * Check the given detection data to see if it has been changed
	 *
	 * \param data - A pointer to the json::Object containing the detection
	 * data to check
	 * \return Returns true if the data is has been changed, false if not.
   // DK REVIEW 20180802 - this looks more like hasThisVersionBeenPublished()
	 */
	bool isDataChanged(std::shared_ptr<const json::Object> data);

	/**
	 * \brief check to see if detection data has been published before
	 *
	 * Check the given detection data to see if it has been previously published
	 *
	 * \param data - A pointer to the json::Object containing the detection
	 * data to check
	 * \return Returns true if the data is has been published, false if not.
	 */
	bool isDataPublished(std::shared_ptr<const json::Object> data,
							bool ignoreVersion = true);
	/**
	 * \brief check to see if detection data is finished
	 *
	 * Check the given detection data to see if is finished (no more
	 * publications)
	 *
	 * \param data - A pointer to the json::Object containing the detection
	 * data to check
	 * \return Returns true if the data is finished, false if not.
	 */
	bool isDataFinished(std::shared_ptr<const json::Object> data);


  // DK REVIEW 20180802
  // I'm confused as to why you have declared the EventThread as a std:thread
  // and then added all this EventThread health stuff that you already have
  // in ThreadBaseClass.  Perhaps it's an order thing, and you did it all
  // before you invented ThreadBaseClass, or perhaps there's something more
  // significant that I haven't caught on to.
  // I would recommend you ditch the std::thread implementation
  // and all the added health stuff here, and move to 
  // a threadbaseclass implementation with the thread health
  // stuff embedded there.
  // new std::thread(&output::checkEventsLoop, this);

	/**
	 * \brief Function to retrieve the last time the event thread health status
	 * was checked
	 *
	 * This function retrieves the last time the health status of the event
	 * thread was checked by the check() function
	 *
	 * \return A std::time_t containing the last check time
	 */
	std::time_t getLastEventHealthCheck();   // DK REVIEW 20180802  This function is never called and should be removed or called.

	/**
	 * \brief Function to check thread health
	 *
	 * This function checks the thread health by getting the value of
	 * m_bCheckWorkThread.
	 *
	 * \return Returns true if the thread is alive, false if the thread has
	 * not responded yet
	 */
	bool getEventThreadHealth();    // DK REVIEW 20180802  This function is never called and should be removed or called.


	/**
	 *\brief Retrieves whether the event thread has been started
	 *
	 * This function retrieves the value of m_bEventThreadStarted, which indicates
	 * whether the event thread has been created and started
	 * \returns true if the event thread has been started, false otherwise
	 */
	bool isEventThreadStarted();   // DK REVIEW 20180802  This function is never called and should be removed or called.

	/**
	 * \brief output file writing function
	 *
   // DK REVIEW - from the doc, I have no idea what this function does.  Please improve
   // with a plethora of more detail.  Should this be a pure virtual function
   // since it's writing something to an output system and this is already an abstract class?
	 * The function used output detection data
	 *
	 * \param data - A pointer to a json::Object containing the data to be
	 * output.
	 */
	void writeOutput(std::shared_ptr<json::Object> data);

	/**
	 * \brief output background work function
	 *
	 * The function (from threadclassbase) used to do background work. It is
	 * used to  process messages from the associator, and to queue messages to
	 * be written out
	 *
	 * \return returns true if work was successful, false otherwise.
	 */
	bool work() override;

	/**
	 * \brief Function to retrieve the last time the event thread health status
	 * was checked
	 *
	 * This function retrieves the last time the health status of the event
	 *  thread was set by the setEventLastHealthy() function
	 *
	 * \return A std::time_t containing the last check time
	 */
	std::time_t getEventLastHealthy();

	/**
	 * \brief Function to get event thread state
	 *
	 * This function gets the event thread state by getting the value of
	 * m_bEventThreadState.
	 *
	 * \return Returns a glass3::util::ThreadState enumeration value representing
	 * the event thread state
	 */
	virtual glass3::util::ThreadState getEventThreadState();

	/**
	 * \brief Function to set event thread health
	 *
	 * This function signifies the event thread health by using
	 * setEventLastHealthy to set m_tEventLastHealthy to now if health is true
	 *
	 * \param health = A boolean value indicating thread health, true indicates
	 * that setLastHealthy to set m_tEventLastHealthy to now, false indicates
	 * it should not
	 */
	void setEventThreadHealth(bool health = true);

 protected:
	/**
	 * \brief output tracking data background work function
	 *
	 * This function is used to manage the tracking cache, and to send request
	 * data (such as hypocenters) from the associator
   // DK REVIEW 20180801 -need more documentation with more details.  Maybe give an example?
   */
	void checkEventsLoop();

	/**
	 * \brief Send output data
   // DK REVIEW 20180801
   // Rejecting your comment.  I don't understand if this is something from iOutput that the Associator or 
   // some other module/library calls to pass data to Output, or if this is something that Output
   // calls to write data to specific output format.
	 *
	 * This pure virtual function is implemented by a class to support writing
	 * a message, be it to disk, memory, kafka, etc.
	 *
	 * \param type - A std::string containing the type of the message
	 * \param id - A std::string containing the id of the message
	 * \param message - A std::string containing the message
	 */
	virtual void sendOutput(const std::string &type, const std::string &id,
							const std::string &message) = 0;

	/**
	 * \brief Function to set thread state
	 *
	 * This function signifies the wcwnr thread state by setting
	 * m_bEventThreadState to the provided value.
   // DK REVIEW 20180801 what does wcwnr stand for?
   *
	 * \param state = A glass3::util::ThreadState enumeration value indicating
	 * the new event thread state
	 */
	void setEventThreadState(glass3::util::ThreadState state);

	/**
	 * \brief Function to set the last time the event thread was healthy
	 *
	 * This function sets the last time the event thread was healthy
	 *
	 * \param now - A std::time_t containing the last time the event thread was
	 * healthy
	 */
	void setEventLastHealthy(std::time_t now);

 private:
	/**
	 * \brief A std::vector of integers containing the times in seconds
	 * since initial report that events should generate detection messages. An
	 * event will not generate a message if it has not changed, and will not
	 * generate a message if all publication times have passed (unless
	 * m_bPubOnExpiration is set to true)
	 */
	std::vector<int> m_PublicationTimes;

	/**
	 * \brief The boolean flag controlling whether to generate a detection
	 * message on expiration notification from the associator
	 */
	std::atomic<bool> m_bPubOnExpiration;

	/**
	 * \brief Information Report interval
	 *
	 * An integer containing the interval (in seconds) between
	 * logging informational reports.
	 */
	std::atomic<int> m_iReportInterval;

	/**
	 * \brief Pointer to Association class
	 *
	 * A glass3::util::iassociator pointer to the class that handles association for
	 * glass
	 */
	glass3::util::iAssociator* m_Associator;

	/**
	 * \brief the std::string configuration value defining the
	 * agency identifier used when generating output files
	 */
	std::string m_sOutputAgencyID;

	/**
	 * \brief the std::string configuration value defining the
	 * author used when generating output files
	 */
	std::string m_sOutputAuthor;

	/**
	 * \brief the integer configuration value indicating the delay in seconds
	 * before requesting glass core's current sitelist
	 */
	std::atomic<int> m_iSiteListDelay;

	/**
	 * \brief the std::string containing the station file name.
	 */
	std::string m_sStationFileName;

	/**
	 * \brief pointer to the glass3::util::cache class used to
	 * store output tracking information
	 */
	glass3::util::Cache * m_TrackingCache;
	std::mutex m_TrackingCacheMutex;

	/**
	 * \brief  pointer to the glass3::util::queue class used to manage
	 * incoming output messages
	 */
	glass3::util::Queue* m_OutputQueue;

	/**
	 * \brief  pointer to the glass3::util::queue class used to manage
	 * incoming lookup messages
   // DK REVIEW 20180801 Site lookup, or hypo lookup, or directions lookup?

	 */
	glass3::util::Queue* m_LookupQueue;

	/**
  // DK REVIEW 20180801 These are lazy comments that make the purpose of the variable at least somewhat unclear.
  // Please be more descriptive for the next 10+ attributes.
  * \brief performance counter
  A better description would be something like(assuming I correctly guessed what this class attribute is used for):
  * \brief A performance counter indicating the total messages processed in the last interval
  */
	int m_iMessageCounter;

	/**
	 * \brief the new event messages performance counter
	 */
	int m_iEventCounter;

	/**
	 * \brief the event cancel messages performance counter
	 */
	int m_iCancelCounter;

	/**
	 * \brief the event expire messages performance counter
	 */
	int m_iExpireCounter;

	/**
	 * \brief the hypo messages performance counter
	 */
	int m_iHypoCounter;

	/**
	 * \brief the lookup messages performance counter
	 */
	int m_iLookupCounter;

	/**
	 * \brief the sitelist messages performance counter
	 */
	int m_iSiteListCounter;
  // DK REVIEW 20180801
  // I am probably the most guilty of this offense, but I think you're supposed to use
  // 'n' as the Hungarian Notation prefix for an int, whereas 'i' is supposed to be used
  // for an iterator.  Just an observation.

	/**
	 * \brief the last time a performance report was generated
	 */
	std::time_t tLastWorkReport;

	/**
	 * \brief the last time a site list was requested
	 */
	std::time_t m_tLastSiteRequest;

	/**
	 * \brief pointer to the glass3::util::threadpool used to queue and
	 * perform output.
   // DK REVIEW 20180801 - Really?  there are so many outputs that we need a pool of threads?
   // or easier to create a pool and then tune the number of threads used?
   // seems like having a separate thread to asynchronously produce outputs, makes a lot of sense.
   // I struggle to imagine that we need a pool of them though...
	 */
	glass3::util::ThreadPool *m_ThreadPool;

	/**
	 * \brief the std::thread pointer to the event thread
	 */
	std::thread *m_EventThread;
  // DK REVIEW 20180801 - I don't understand.  We have a base thread, AND an EventThread (whatever that's for),
  // AND a Threadpool for ouputs.
  // Where's the Watch Watcher and Watch Watcher Watcher thread?

	/**
	 * \brief glass3::util::ThreadState enumeration used to track event thread
	 * status, set by setEventThreadState()
	 */
	std::atomic<glass3::util::ThreadState> m_bEventThreadState;

	/**
	 * \brief the time_t holding the last time the event thread status was
	 * checked, set by setEventLastHealthy() in check
	 */
	std::atomic<double> m_tEventLastHealthy;
};
}  // namespace output
}  // namespace glass3
#endif  // OUTPUT_H
