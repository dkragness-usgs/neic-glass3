/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef PICKLIST_H
#define PICKLIST_H

#include <json.h>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <mutex>
#include <thread>
#include <queue>
#include <random>
#include "Glass.h"

namespace glasscore {

// forward declarations
class CGlass;
class CSite;
class CSiteList;
class CPick;
class CHypo;

/**
 * \brief glasscore pick list class
 *
 * The CPickList class is the class that maintains a std::map of all the
 * waveform arrival picks being considered by glasscore.
 *
 * CPickList also maintains a std::vector mapping the double pick arrival time
 * (in julian seconds) to the std::string pick id
 *
 * CPickList contains functions to support pick scavenging, resoultion, rogue
 * tracking, and new data input.
 *
 * CPickList uses smart pointers (std::shared_ptr).
 */
class CPickList {
 public:
	/**
	 * \brief CPickList constructor
	 *
	 * The constructor for the CPickList class.
	 * \param numThreads - An integer containing the number of
	 * threads in the pool.  Default 1
	 * \param sleepTime - An integer containing the amount of
	 * time to sleep in milliseconds between jobs.  Default 50
	 * \param checkInterval - An integer containing the amount of time in
	 * seconds between status checks. -1 to disable status checks.  Default 300.
	 */
	explicit CPickList(int numThreads = 1, int sleepTime = 50,
						int checkInterval = 300);

	/**
	 * \brief CPickList destructor
	 *
	 * The destructor for the CPickList class.
	 */
	~CPickList();

	/**
	 * \brief CPickList clear function
	 */
	void clear();

	/**
	 * \brief Remove all picks from pick list
	 *
	 * Clears all picks from the vector and map
	 */
	void clearPicks();

	/**
	 * \brief CPickList communication receiving function
	 *
	 * The function used by CPickList to receive communication
	 * (such as configuration or input data), from outside the
	 * glasscore library, or it's parent CGlass.
	 *
	 * Supports Pick (add pick data to list) and ClearGlass
	 * (clear all pick data) inputs.
	 *
	 * \param com - A pointer to a json::object containing the
	 * communication.
	 * \return Returns true if the communication was handled by CPickList,
	 * false otherwise
	 */
	bool dispatch(std::shared_ptr<json::Object> com);

	/**
	 * \brief CPickList add pick function
	 *
	 * The function used by CPickList to add a pick to the vector
	 * and map, if the new pick causes the number of picks in the vector/map
	 * to exceed the configured maximum, remove the oldest pick
	 * from the list/map, as well as the list of picks in CSite.
	 *
	 * This function will generate a json formatted request for site
	 * (station) information if the pick is from an unknown site.
	 *
	 * This function will first attempt to associate the pick with
	 * an existing hypocenter via calling the CHypoList::associate()
	 * function.  If association is unsuccessful, the pick is nucleated
	 * using the CPick::Nucleate() function.
	 *
	 * \param pick - A pointer to a json::object containing the
	 * pick.
	 * \return Returns true if the pick was usable and added by CPickList,
	 * false otherwise
	 */
	bool addPick(std::shared_ptr<json::Object> pick);

	/**
	 * \brief CPickList get pick function
	 *
	 * Given the integer id of a pick get a shared_ptr to that pick.
	 *
	 * \param idPick - An integer value containing the id of the pick to get
	 * \return Returns a shared_ptr to the found CPick, or null if no pick
	 * found.
	 */
	std::shared_ptr<CPick> getPick(int idPick);  // DK REVIEW 20180822 - this function isn't used anywhere other than in unit test.  Toss it, or move it into unittest derived class.

	/**
	 * \brief Get insertion index for pick
	 *
	 * This function looks up the proper insertion index for the vector given an
	 * arrival time using a binary search to identify the index element
	 * is less than the time provided, and the next element is greater.
	 *
	 * \param tPick - A double value containing the arrival time to use, in
	 * julian seconds of the pick to add.
	 * \return Returns the insertion index, if the insertion is before
	 * the beginning, -1 is returned, if insertion is after the last element,
	 * the id of the last element is returned, if the vector is empty,
	 * -2 is returned.
	 */
	int indexPick(double tPick);   // DK REVIEW 20180828 - this function should go away.  use lower_bound() or upper_bound() instead.
                                 // DK REVIEW 20180828 -  DK REVIEW
                                 // This funciton and the whole list model need work.
                                 // It seems inefficient to me to have a vector vPick
                                 // that contains a bunch of PickIDs, that you then have
                                 // a map mPick that you look up a CPick pointer for, based
                                 // on the pick int.
                                 // This function should use std::lower_bound() and std::upper_bound(),
                                 // unless there's some excellent reason not to
                                 // might be more efficient to user std:: multiset
                                 // instead of std::vector if the picks can come in significantly out of order.

	/**
	 * \brief Print basic values to screen for pick list
	 *
	 * Causes CPickList to print basic values to the console for
	 * each pick in the list
	 */
	void listPicks();   // DK REVIEW 20180828 - get rid of this function(currently unused).  If we still want this function, replace
                      // it with one that returns a string.  Only CGlass should write to the console, although I guess calling
                      // CLogit from here isn't horrible....

	/**
	 * \brief Checks if picks is duplicate
	 *
	 * Takes a new pick and compares with list of picks.
	 * True if pick is a duplicate
	 */
	bool checkDuplicate(CPick *newPick, double window);   // DK REVIEW 20180828 - Don't actually check for duplicates here.  Find the CSite
                                                        // for the pick and then search that CSite's picklist for duplicates.  Much faster.

	/**
	 * \brief Search for any associable picks that match hypo
	 *
	 * Search through all picks within a provided number seconds from the origin
	 * time of the given hypocenter, adding any picks that meet association
	 * criteria to the given hypocenter.
	 *
	 * \param hyp - A shared_ptr to a CHypo object containing the hypocenter
	 * to attempt to associate to.
	 * \param tDuration - A double value containing the duration to search picks
	 * from origin time in seconds, defaults to 2400.0
	 * \return Returns true if any picks were associated to the hypocenter,
	 * false otherwise.
	 */
	bool scavenge(std::shared_ptr<CHypo> hyp, double tDuration = 2400.0);

	/**
	 * \brief Generate rogue list for tuning process
	 *
	 * Search through all picks within a provided number of seconds from the
	 * given origin time, creating a vector of all picks that could be
	 * associated with given hypocenter ID, but are actually either unassociated
	 * or associated with another hypocenter
	 *
	 * \param pidHyp - A std::string containing the id of the hypocenter to use
	 * \param tOrg - A double value containing the origin time in julian seconds
	 * of the hypocenter to use.
	 * \param tDuration - A double value containing the duration to search picks
	 * from origin time in seconds, defaults to 2400.0
	 */
	std::vector<std::shared_ptr<CPick>> rogues(std::string pidHyp, double tOrg,
												double tDuration = 2400.0);    // DK REVIEW 20180828 - Seems like this function is no longer used.
                                                       // It is only used in Hypo.list(), which is an uncalled auditing function in Hypo.
                                                       // Seems like it could happily just disappear.

	/**
	 * \brief check to see if each thread is still functional
	 *
	 * Checks each thread to see if it is still responsive.
	 */
	bool statusCheck();  // DK REVIEW 20180828 - Patton healthCheck() ?

	/**
	 * \brief CGlass getter
	 * \return the CGlass pointer
	 */
	const CGlass* getGlass() const;  // DK REVIEW 20180828  - These go away if we make CGlass a static class.

	/**
	 * \brief CGlass setter
	 * \param glass - the CGlass pointer
	 */
	void setGlass(CGlass* glass);

	/**
	 * \brief CSiteList getter
	 * \return the CSiteList pointer
	 */
	const CSiteList* getSiteList() const;

	/**
	 * \brief CSiteList setter
	 * \param siteList - the CSiteList pointer
	 */
	void setSiteList(CSiteList* siteList);

	/**
	 * \brief nPick getter
	 * \return the nPick
	 */
	int getNPick() const;  // DK REVIEW 20180828  - rename to something clear like getCurrentPickCount()

	/**
	 * \brief nPickMax getter
	 * \return the nPickMax
	 */
	int getNPickMax() const;  // DK REVIEW 20180828  - getMaxAllowedPickCount

	/**
	 * \brief nPickMax Setter
	 * \param picknMax - the nPickMax
	 */
	void setNPickMax(int picknMax);  // DK REVIEW 20180828  - setMaxAllowedPickCount

	/**
	 * \brief nPickTotal getter
	 * \return the nPickTotal
	 */
	int getNPickTotal() const;  // DK REVIEW 20180828  - getNumOfPicksProcessed

	/**
	 * \brief Get the current size of the pick list
	 */
	int getVPickSize() const;   // DK REVIEW 20180828  - is this different than getNPick()?

 private:
	/**
	 * \brief Process the next pick on the queue
	 *
	 * Attempts to associate and nuclate the next pick on the queue.
	 */
	void processPick();   // DK REVIEW 20180828  - Rename.  This is the main work function for PickList(), it pulls picks from a 
                        // a queue and processes them, and runs until something tells it to stop.  It is not a
                        // simple function that processes one pick.

	/**
	 * \brief the job sleep
	 *
	 * The function that performs the sleep between jobs
	 */
	void jobSleep();   // DK REVIEW 20180828  -  This function should go away, or have it's logic replaced by a simple Sleep(xmillisec);

	/**
	 * \brief thread status update function
	 *
	 * Updates the status for the current thread
	 * \param status - A boolean flag containing the status to set
	 */
	void setStatus(bool status);

	/**
	 * \brief A pointer to the parent CGlass class, used to look up site
	 * information, configuration values, call association functions, and debug
	 * flags
	 */
	CGlass *pGlass;

	/**
	 * \brief A pointer to a CSiteList object containing all the sites for
	 * lookups
	 */
	CSiteList *pSiteList;

	/**
	 * \brief An integer containing the total number of picks ever added to
	 * CPickList
	 */
	int nPickTotal;   // DK REVIEW 20180828  - nPicksProcessed

	/**
	 * \brief An integer containing the maximum number of picks allowed in
	 * CPickList. This value is overridden by pGlass->nPickMax if provided.
	 * Defaults to 10000.  
	 */
	int nPickMax;   

	/**
	 * \brief An integer containing the current number of picks in CPickList.
	 * Used to generate the pick id.
	 */
	int nPick;

	/**
	 * \brief A std::vector mapping the arrival time of each pick in CPickList
	 * to it's integer pick id. The elements in this vector object are inserted
	 * in a manner to keep it in sequential time order from oldest to youngest.
	 */
	std::vector<std::pair<double, int>> vPick;  // DK REVIEW 20180828 - meh.. would this have better performance as a list?
                                              

	/**
	 * \brief A std::map containing a std::shared_ptr to each pick in CPickList
	 * indexed by the integer pick id.
	 */
	std::map<int, std::shared_ptr<CPick>> mPick;   // DK REVIEW 20180828  -  Outside of PickList::getPick() which is only used
                                                 // by unit-testing code, mPick is only ever accessed after a lookup in
                                                 // vPick.  Ditch mPick and store a shared_ptr in vPick.

	/**
	 * \brief A std::queue containing a std::shared_ptr to each pick in that
	 * needs to be processed
	 */
	std::queue<std::shared_ptr<CPick>> qProcessList;   // DK REVIEW 20180828  -  This is the work(picks to be processed) queue.  Presumably revamped by Patton as part of threading rework.

	/**
	 * \brief the std::mutex for qProcessList
	 */
	std::mutex m_qProcessMutex;   // DK REVIEW 20180828  -  mutex for the  work(picks to be processed) queue.

	/**
	 * \brief the std::vector of std::threads
	 */
	std::vector<std::thread> vProcessThreads;   // DK REVIEW 20180828  - Presumably revamped by Patton as part of threading rework.

	/**
	 * \brief An integer containing the number of
	 * threads in the pool.
	 */
	int m_iNumThreads;   // DK REVIEW 20180828  - Presumably revamped by Patton as part of threading rework.

	/**
	 * \brief A std::map containing the status of each thread
	 */
	std::map<std::thread::id, bool> m_ThreadStatusMap;   // DK REVIEW 20180828  - Presumably revamped by Patton as part of threading rework.

	/**
	 * \brief An integer containing the amount of
	 * time to sleep in milliseconds between picks.
	 */
	int m_iSleepTimeMS;

	/**
	 * \brief the std::mutex for m_ThreadStatusMap
	 */
	std::mutex m_StatusMutex;   // DK REVIEW 20180828  - Presumably replaced by atomic-ification of status

	/**
	 * \brief the integer interval in seconds after which the work thread
	 * will be considered dead. A negative check interval disables thread
	 * status checks
	 */
	int m_iStatusCheckInterval;   // DK REVIEW 20180828  - Presumably revamped by Patton as part of threading rework.

	/**
	 * \brief the time_t holding the last time the thread status was checked
	 */
	time_t tLastStatusCheck   // DK REVIEW 20180828  - Presumably revamped by Patton as part of threading rework.

	/**
	 * \brief the boolean flags indicating that the process threads
	 * should keep running.
	 */
	bool m_bRunProcessLoop;   // DK REVIEW 20180828  - Presumably revamped by Patton as part of threading rework.

	/**
	 * \brief A recursive_mutex to control threading access to vPick.
	 * NOTE: recursive mutexes are frowned upon, so maybe redesign around it
	 * see: http://www.codingstandard.com/rule/18-3-3-do-not-use-stdrecursive_mutex/
	 * However a recursive_mutex allows us to maintain the original class
	 * design as delivered by the contractor.
	 */
	mutable std::recursive_mutex m_vPickMutex;   // DK REVIEW 20180828  - seems like this is gonna be a likely bottleneck point

	/**
	 * \brief A recursive_mutex to control threading access to CPickList.
	 * NOTE: recursive mutexes are frowned upon, so maybe redesign around it
	 * see: http://www.codingstandard.com/rule/18-3-3-do-not-use-stdrecursive_mutex/
	 * However a recursive_mutex allows us to maintain the original class
	 * design as delivered by the contractor.
	 */
	mutable std::recursive_mutex m_PickListMutex;    // DK REVIEW 20180828  - This is the class config mutex

	/**
	 * \brief A random engine used to generate random numbers
	 */
	std::default_random_engine m_RandomGenerator;   // DK REVIEW 20180828  -  Go away.
};
}  // namespace glasscore
#endif  // PICKLIST_H
