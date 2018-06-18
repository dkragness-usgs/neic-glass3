/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef CONFIG_H
#define CONFIG_H

#include <json.h>
#include <string>
#include <fstream>

namespace util {
// ========================
// DK REVIEW 20180618
// Add something about JSON to the \brief description below
// ========================

/**
 * \brief glass configuration class
 *
 * The glass config class is a class used to read json formatted configuration
 * files from disk.  The config class filters out comment lines (signified by
 * '#') and white space, and provides the configuration as a json object.
 */

 // ========================
 // DK REVIEW 20180618
 // Seems like this Class is "Not thread safe".  Seems like it should
 // be labeled as such.
 // ========================


 // ========================
 // DK REVIEW 20180618
 // Seems like this class has 2 operations:
 // 1) Open a file, read it, and generate a string from it's contents (Optional)
 // 2) Parse a string into a json::Object
 // It has potentially 2 sets of inputs
 //  a) Dir + filename (to pull the string from)
 //  b) String (to parse into JSON)
 //
 // I would think the most likely needed public functions are:
 //    1) ParseJSONFromFile()   -- could return JSON object or some sort of return code, or could throw an exception in lieu of a return code.
 //    2) ParseJSONFromString() -- could either return JSON object or some sort of return code, or could throw an exception in lieu of a return code.
 //    3) GetJSON()             -- return JSON object.  Needed in case the parsing is done within
 //                                the context of the constructor.
 //    4) GetParseStatus()      -- this would tell the caller the parse state:  SUCcESS, or if it failed, then WHY.  Could be a string or 
 //                                an errno value, or whatever, just something to indicate success or not.
 // Then you would probably want matching constructors to make it more convenient
 // And you need a destructor (although possibly you could live with the default)
 // Then if you wanted to start getting anal about it, you could add things like:
 //     GetConfigFileInfo()
 //     GetInputJSONString()
 // I fail to see a reason for any other public functions.  This doesn't actually do any 
 // application specific parsing, it's just string_to_json() or file_to_json()
 //
 // I think all the extra functions add unneeded flexibility and undesired extra codepaths.
 //
 //  Some sort of condensed narrative should go into the documentation for the class, above.  
 //  "BLAH BLAH BLAH is what this class is about and this is what it does and see method descriptions for more details."

class Config {
 public:
	/**
	 * \brief config constructor
	 *
	 * The constructor for the config class.
	 * Initilizes members to default values.
	 */
	Config();

	/**
	 * \brief config advanced constructor
   // ========================
   // DK REVIEW 20180618
   // "Advanced constructor" is not a helpful description
   // "Constructor based on filepath and filename" would be more useful.
   // Not a helpful description
   // maybe:
   // "An advanced constructor that loads configuration from a JSON formatted file accessed via filepath/filename".
   // ========================
   *
	 * The advanced constructor for the config class.
	 * Loads the provided configuration file containing the configuration.
   *
	 * \param filepath - A std::string containing the path to the configuration
	 * file
	 * \param filename - A std::string containing the configuration file name.
	 */
	Config(std::string filepath, std::string filename);

	/**
  // ========================
  // DK REVIEW 20180618
  // "Advanced constructor" is not a helpful description
  // "Constructor based on string" would be more useful.
  // Not a helpful description
  // maybe:
  // "An advanced constructor that loads configuration from a JSON formatted string".
  // ========================
  * \brief config advanced constructor
	 *
	 * The advanced constructor for the config class.
	 * Loads the provided std::string containing the configuration.
	 *
	 * \param newconfig - A std::string containing the json formatted
	 * configuration data to load.
	 */
	explicit Config(std::string newconfig);

	/**
	 * \brief config destructor
	 *
	 * The destructor for the config class.
	 */
	~Config();

  // ========================
  // DK REVIEW 20180618
  // Only function without a doxy comment.  Need one?
  // ========================
  void clear();

	// setup the config class
	// set up for configuration from a file
	/**
  // ========================
  // DK REVIEW 20180618
  // Eh?  I don't understand the \brief...
  // I think this whole comment needs an english check.  Oh so fun!
  // ========================
  * \brief config configuration function
	 *
	 * The this function configures the config class.
	 * \param filepath - A std::string containing the path to the configuration
	 * file
	 * \param filename - A std::string containing the configuration file name.
	 * \return returns true if successful, false otherwise
	 */
	bool setup(std::string filepath, std::string filename);

	/**
  // ========================
  // DK REVIEW 20180618
  // What's the point of this function?  Is it to reload configuration information
  // from an already described source?  Should it be a protected/private function that's
  // called by setup or matching constructor?
  // ========================
  * \brief Load a configuration file from disk
	 */
	void loadConfigfile();

	/**
	 * \brief Passes a json formatted string into the config class
	 *
	 * \param newconfig - A std::string containing the json formatted
	 * configuration data to load.
	 */
	void loadConfigstring(std::string newconfig);

	/**
	 * \brief Get current configuration as json object
	 *
	 * \returns Return a json::Object containing the configuration
   // ========================
   // DK REVIEW 20180618
   // Does it return NULL(or an empty object) if the configuration hasn't been configured (i.e. nothing loaded via string or file)?  Too Anal?
   // ========================
   */
	json::Object getConfigJSON();

	/**
	 * \brief Get current configuration as a string
	 *
	 * \returns Return a std::string containing the json formatted
	 * configuration string.
	 */
	std::string getConfig_String();

	/**
	 *\brief getter for m_sFileName
	 */
	const std::string& getFileName() const {
		return m_sFileName;
	}

	/**
	 *\brief getter for m_sFilePath
	 */
	const std::string& getFilePath() const {
		return m_sFilePath;
	}

 protected:
	/**
  // ========================
  // DK REVIEW 20180618
  // What's the difference between this function and loadConfigstring() ?
  // Is there any data member that indicates whether the current config is based on 
  // a string or a file-load?
  // ========================

	 * \brief Configuration parsing function
	 *
	 * Parses the provided string into a json::Object.
	 * Sets m_ConfigJSON to the parsed json::Object
	 * Also sets m_sConfigString to the provided string.
   // ========================
   // DK REVIEW 20180618
   // Seems like this function and loadConfigstring() should NULL out the
   // path and file member attributes, so as to avoid confusion about
   // the current config being read from a file instead of coming in as
   // a string, no?
   // ========================
   * \param newconfig - A std::string containing the json formatted
	 * configuration data to parse
	 */
	bool setConfigString(std::string newconfig);

	/**
	 * \brief Opens the configuration file
	 */
	bool openConfigFile();

	/**
	 * \brief Checks that the configuration file
	 * is still open and is not at the end.
   // ========================
   // DK REVIEW 20180618
   // Doesn't it read the config file?
   // ========================
   */
	std::string getNextLineFromConfigFile();

	/**
	 * \brief Opens the configuration file
   // ========================
   // DK REVIEW 20180618
   // Mwah?  Really?  Seems like that's what 
   // openConfigFile() does...
   // ========================
   */
	bool hasDataConfigFile();

	/**
	 * \brief Closes the open configuration file.
	 */
	bool closeConfigFile();

 private:
	/**
	 * \brief The path to the configuration file
	 */
	std::string m_sFilePath;

	/**
	 * \brief The name of the configuration file
	 */
	std::string m_sFileName;

	/**
	 * \brief The configuration file stream
	 */
	std::ifstream m_InFile;

	/**
	 * \brief The configuration loaded from the config file as
	 * a std::string.
	 */
	std::string m_sConfigString;

	/**
	 * \brief The configuration loaded from the config file as
	 * a json::Object
	 */
	json::Object m_ConfigJSON;
};
}  // namespace util
#endif  // CONFIG_H

