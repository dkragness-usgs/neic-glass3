/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef CACHE_H
#define CACHE_H

#include <baseclass.h>
#include <json.h>
#include <logger.h>

#include <mutex>
#include <string>
#include <map>
#include <memory>

namespace util {
/**
 * \brief util cache class
 *
 * The util cache class is a class implementing an in-memory cache of
 * pointers to json::Objects.  The cache is thread safe and supports writing
 * the cache to disk.
 *
 * cache inherits from the baseclass class.
 */
class Cache : public util::BaseClass {
 public:
	/**
	 * \brief cache constructor
	 *
	 * The cache for the lookup class.
	 * Initializes members to default values.
	 */
	Cache();

  // ========================
  // DK REVIEW 20180621
  // Needs a better \brief description
  // ========================

	/**
	 * \brief cache advanced constructor
	 *
	 * The advanced constructor for the cache class.
	 * Initializes members to default values.
	 * Calls setup to configure the  class
	 * \param config - A json::Object pointer to the configuration to use
	 */
	explicit Cache(json::Object *config);

	/**
	 * \brief cache destructor
	 *
	 * The destructor for the cache class.
	 */
	virtual ~Cache();

	/**
	 * \brief cache configuration function
	 *
	 * This function configures the cache class
	 * \param config - A pointer to a json::Object containing to the
	 * configuration to use

   // ========================
   // DK REVIEW 20180621
   // Looks like "Cmd" and "DiskFile" 
   // are both required keys for the config parameter in the setup JSON object.
   // Should be doc'd somewhere, probably here.
   // ========================
	 * \return returns true if successful, false otherwise.
	 */
	bool setup(json::Object *config) override;

	/**
	 * \brief cache clear function
	 *
	 * The clear function for the cache class.
	 * Clears all configuration
	 */
	void clear() override;

	/**
	 *\brief add data to cache
	 *
	 * Add the provided data identified by the provided id to the cache
	 * \param data - A pointer to a json::Object to add to the cache
	 * \param id - A std::string that contains the key that identifies the data
	 * to add
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns true if successful, false otherwise.
	 */
	virtual bool addToCache(std::shared_ptr<json::Object> data, std::string id,
							bool lock = true);

	/**
	 *\brief remove data from cache
	 *
	 * Remove the data identified by the provided id from the cache
	 * \param id - A std::string that contains the key that identifies the data
	 * to remove
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns true if successful, false otherwise.
	 */
	virtual bool removeFromCache(std::string id, bool lock = true);

	/**
	 *\brief check for data in cache
	 *
	 * Check to see if the data identified by the provided id is in the cache
	 * \param id - A std::string that contains the key that identifies the data
	 * to remove
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns true if successful, false otherwise.
	 */
	virtual bool isInCache(std::string id, bool lock = true);

	/**
	 *\brief get data from cache
	 *
	 * Get the data identified by the provided id from the cache
	 * \param id - A std::string that contains the key that identifies the data
	 * to get
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns a pointer to the json::Object containing the data, NULL
	 * if the data
	 * was not found
	 */
	virtual std::shared_ptr<json::Object> getFromCache(std::string id,
														bool lock = true);

	/**
	 * \brief get next data from cache
	 *
	 * Gets the next data available from the cache
	 * \param restart - A boolean flag indicating whether to start at the
	 * beginning of the cache
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns a pointer to the json::Object containing the next data,
	 * NULL if there is no
	 * more data available.
	 */
	virtual std::shared_ptr<json::Object> getNextFromCache(bool restart = false,
															bool lock = true);

	/**
	 *\brief clear data from cache
	 *
	 * Clear all data from the cache
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 */
	virtual void clearCache(bool lock = true);

	/**
	 *\brief check if cache empty
	 *
	 * Check to see if the cache is empty
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns true if empty, false otherwise.
	 */
	virtual bool isEmpty(bool lock = true);

	/**
	 *\brief load cache from disk
	 *
	 * Load the cache from disk
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns true if successful, false otherwise.
	 */
	bool loadCacheFromDisk(bool lock = true);

	/**
	 *\brief write cache to disk
	 *
	 * Write the cache to disk
	 * \param lock - A boolean value indicating whether to lock the mutex.
	 * Defaults to true
	 * \return returns true if successful, false otherwise.
	 */
	bool writeCacheToDisk(bool lock = true);

  // ========================
  // DK REVIEW 20180621
  // Function name capitalization
  // looks really odd.
  // This doesn't seem to match the google style guide
  // for variable names.
  // seems like it should be m_DiskCacheFilename
  // and then getDiskCachFileName(), if I'm not misinterpreting the guide.  (I can't remember what the capitalization should be)
  // ========================
  /**
	 *\brief getter for m_sDiskCacheFile
	 */
	const std::string getSDiskCacheFile() {
		m_ConfigMutex.lock();
		std::string diskcachefile = m_sDiskCacheFile;
		m_ConfigMutex.unlock();
		return (diskcachefile);
	}

  // ========================
  // DK REVIEW 20180621
  // How do you set the DiskCachFile filename?  Looks like it's loaded from the config JSON object in setup()
  // Should there be a separate realtime set option, or a protected set option that gets called from the setup()
  // function?
  // ========================

	/**
	 *\brief getter for m_bCacheModified
	 */
	bool getBCacheModified() {
		return (m_bCacheModified);
	}

	/**
	 *\brief getter for m_Cache size
	 */
	int size() {
		int cachesize = 0;

		m_CacheMutex.lock();
		if (m_Cache.empty() != true) {
			cachesize = static_cast<int>(m_Cache.size());
		}
		m_CacheMutex.unlock();

		return (cachesize);
	}

 private:
	/**
	 * \brief the std::map used to store the cache
	 */
	std::map<std::string, std::shared_ptr<json::Object>> m_Cache;

	/**
	 * \brief a boolean flag indicating whether the cache has
	 * been modified
	 */
	bool m_bCacheModified;

	/**
	 * \brief a std::map iterator used to output the cache
	 */
	std::map<std::string, std::shared_ptr<json::Object>>::iterator m_CacheDumpItr;


  // ========================
  // DK REVIEW 20180621
  // Do you really need 3 different mutex's.  Isn't one enough?  What are the 
  // properties of the 3 different mutex's that make them neccessary.
  // I understand 1 (I'm using the cache, don't try to use it till I"m done)
  // I understand 2  read + write  (where read says I"m reading it, so don't change it, but somebody else can read it)
  // And write says (I'm messing with the cache, so don't anybody else try to do anything with it).
  // I don't understand needing one for the config and one for the cache and one for disk (unless it's somehow
  // attached to the file and preventing some other process from accessing it at the same time).
  // ========================

	/**
	 * \brief the mutex for the cache
	 */
	std::mutex m_CacheMutex;

	/**
	 * \brief the std::string configuration value containing the location
	 * of the disk file used by the cache
	 */
	std::string m_sDiskCacheFile;

	/**
	 * \brief the mutex for configuration
	 */
	std::mutex m_ConfigMutex;

	/**
	 * \brief the mutex for disk operations
	 */
	std::mutex m_DiskFileMutex;
};
}  // namespace util
#endif  // CACHE_H

