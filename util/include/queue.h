/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef QUEUE_H
#define QUEUE_H

#include <json.h>

#include <memory>
#include <mutex>
#include <string>
#include <queue>

namespace util {
  // ========================
  // DK REVIEW 20180627
  // a "cache queue"?
  // ========================
  /**
 * \brief util queue class
 *
 * The util cache queue is a class implementing a queue of
 * pointers to json::Objects.  The queue IS THREAD SAFE.

 // ========================
 // DK REVIEW 20180627
 // Looks like this is a standard queue(FIFO) with no ability to peek or iterate through.
 // Only supports push to the back and pop from the front, no?
 // would be nice to see "FIFO" and something about no peek or no iterate,
 // even though those concepts go along with the definition of a classic queue.
 //
 // I don't see the point to using this class unless you're gonna use it in a threadsafe mode.
 // I think you should pull the "lock" options and always lock, unless you can provide an 
 // example use-case of using it without the lock.
 // Seems like if you didn't need locking, you'd just use:
 //    std::queue<std::shared_ptr<json::Object>>
 // and not add the extra layer of beauracracy on top.
 // ========================



 */
class Queue {
 public:
	/**
	 * \brief queue constructor
	 *
	 * The constructor for the queue class.
	 */
	Queue();

	/**
	 * \brief queue destructor
	 *
	 * The destructor for the queue class.
	 */
	~Queue();

	/**
	 *\brief add data to queue
	 *
	 * Add the provided data the queue
	 * \param data - A pointer to a json::Object to add to the queue
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns true if successful, false otherwise.
   // ========================
   // DK REVIEW 20180627
   // Why would you not lock the mutex?  Performance?  What's the expected behavior if you do not 
   // lock the mutex and two threads access the queue at once?
   // ========================
   */
	bool addDataToQueue(std::shared_ptr<json::Object> data, bool lock = true);

	/**
	 *\brief get data from queue
	 *
	 * Get the next data from the queue
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns a pointer to the json::Object containing the data, NULL
	 * there was no
	 * data in the queue
   // ========================
   // DK REVIEW 20180627
   // Why would you not lock the mutex?  Performance?  What's the expected behavior if you do not
   // lock the mutex and two threads access the queue at once?
   // ========================
   */
	std::shared_ptr<json::Object> getDataFromQueue(bool lock = true);

	/**
	 *\brief clear data from queue
	 *
	 * Clear all data from the queue
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
   // ========================
   // DK REVIEW 20180627
   // Why would you not lock the mutex?  Performance?  What's the expected behavior if you do not
   // lock the mutex and two threads access the queue at once?
   // ========================
   */
	void clearQueue(bool lock = true);

	/**
	 *\brief get the size of the queue
	 *
	 * Get the current size of the queue
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns an integer value containing the current size of the
	 * queue
	 */
	int size(bool lock = true);

 private:
	/**
	 * \brief the std::queue used to store the queue
	 */
	std::queue<std::shared_ptr<json::Object>> m_DataQueue;

	/**
	 * \brief the mutex for the queue
	 */
	std::mutex m_QueueMutex;
};
}  // namespace util
#endif  // QUEUE_H

