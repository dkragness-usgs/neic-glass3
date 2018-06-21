// gen-travel-times-app.cpp : Defines the entry point for the console
// application.
#include <json.h>
#include <logger.h>
#include <config.h>
#include <Logit.h>
#include <GenTrv.h>
#include <Terra.h>
#include <Ray.h>
#include <TTT.h>
#include <TravelTime.h>
#include "gen-travel-times-appCMakeConfig.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>

// ========================
// DK REVIEW 20180621
// Missing description of the file.
// Something about application that generates traveltimes
// Seems like it should also list external library dependencies.
// And provide a USAGE statement.  
// ========================


// ========================
// DK REVIEW 20180621
// Missing doxy comment.
// this looks like a logging function, but there's a bunch of code
// in main() that calls logger::log() and references spdlog
// OMG! What's up with that!?!
// ========================
void logTravelTimes(glassutil::logMessageStruct message) {
	if (message.level == glassutil::log_level::info) {
		logger::log("info", "traveltime: " + message.message);
	} else if (message.level == glassutil::log_level::debug) {
		logger::log("debug", "traveltime: " + message.message);
	} else if (message.level == glassutil::log_level::warn) {
		logger::log("warning", "traveltime: " + message.message);
	} else if (message.level == glassutil::log_level::error) {
		logger::log("error", "traveltime: " + message.message);
	}
}

// ========================
// DK REVIEW 20180621
// Missing doxy comment.
// ========================
int main(int argc, char* argv[]) {
	// check our arguments
	if ((argc < 2) || (argc > 3)) {
		std::cout << "gen-travel-times-app version "
				<< std::to_string(GEN_TRAVELTIMES_VERSION_MAJOR) << "."
				<< std::to_string(GEN_TRAVELTIMES_VERSION_MINOR) << "; Usage: "
				<< "gen-travel-times-app <configfile> [logname]" << std::endl;
		return 1;
	}

	// Look up our log directory
	std::string logpath;
  // ========================
  // DK REVIEW 20180621
  // Somewhere in some doc for this app, it should indicate use of this env variable
  // ========================
  char* pLogDir = getenv("GLASS_LOG");
	if (pLogDir != NULL) {
		logpath = pLogDir;
	} else {
		std::cout << "gen-travel-times-app using default log directory of ./"
					<< std::endl;
		logpath = "./";
	}

	// get our logname if available
	if (argc >= 3) {
		logName = std::string(argv[2]);
	}
  else  { 
    std::string logName = "gen-travel-times-app";
  }

  // now set up our logging
	logger::log_init(logName, spdlog::level::debug, logpath, true);

	logger::log(
			"info",
			"gen-travel-times-app: Version "
					+ std::to_string(GEN_TRAVELTIMES_VERSION_MAJOR) + "."
					+ std::to_string(GEN_TRAVELTIMES_VERSION_MINOR) + "."
					+ std::to_string(GEN_TRAVELTIMES_VERSION_PATCH)
					+ " startup.");

	// get our config file location from the arguments
	std::string configFile = argv[1];

	logger::log("info",
				"gen-travel-times-app: using config file: " + configFile);

	// load our basic config
	util::Config * genConfig = new util::Config("", configFile);

  // ========================
  // DK REVIEW 20180621
  // Meh...  Grumble grumble...  I hate this code style with non matched brace alignment, to save a line....
  // ========================
  // check to see if our config is of the right format
	if (genConfig->getJSON().HasKey("Configuration")
			&& ((genConfig->getJSON())["Configuration"].GetType()
					== json::ValueType::StringVal)) {
		std::string configType = (genConfig->getJSON())["Configuration"]
				.ToString();

		if (configType != "gen-travel-times-app") {
			logger::log("critcal",
						"gen-travel-times-app: Wrong configuration, exiting.");

      // ========================
      // DK REVIEW 20180621
      // I see this kind of code in multiple places, and I'm not a fan.
      // Instead of doing uniform cleanup in multiple places, I think it
      // would be better to have
      // returncode = 1;
      // goto cleanup_config;
      // :cleanup_config
      //   delete(genConfig)
      //   return(returncode)
      //
      //  thoughts?
      // ========================
      delete (genConfig);
			return (1);
		}
	} else {
		// no command or type
		logger::log(
				"critcal",
				"gen-travel-times-app: Missing required Configuration Key.");

		delete (genConfig);
		return (1);
	}

	// model
	std::string model = "";
	if (genConfig->getJSON().HasKey("Model")
			&& ((genConfig->getJSON())["Model"].GetType()
					== json::ValueType::StringVal)) {
		model = (genConfig->getJSON())["Model"].ToString();
		glassutil::CLogit::log(glassutil::log_level::info,
								"gen-travel-times-app: Using Model: " + model);
	} else {
		glassutil::CLogit::log(
				glassutil::log_level::error,
				"gen-travel-times-app: Missing required Model Key.");

		delete (genConfig);
		return (1);
	}

	// output path
	std::string path = "./";
	if (genConfig->getJSON().HasKey("OutputPath")
			&& ((genConfig->getJSON())["OutputPath"].GetType()
					== json::ValueType::StringVal)) {
		path = (genConfig->getJSON())["OutputPath"].ToString();
    // ========================
    // DK REVIEW 20180621
    // Why are we using logger::log() and glassutil::CLogit::log()
    // and we have a logTravelTimes() function? 
    // ========================
    glassutil::CLogit::log(
				glassutil::log_level::info,
				"gen-travel-times-app: Using OutputPath: " + path);
	}

	// file extension
  // ========================
  // DK REVIEW 20180621
  // Default should go in an else after the if below.
  // If I found a config command, use it, else use default.
  // If you're just gonna log it when it's dug up from the config
  // file, then seems like you should say something about
  // "Found FileExtension command, using ..."
  // Instead of just "using FileExtension", which makes it
  // sound like you're always gonna tell me what FileExtension
  // you're using.
  // ========================
  std::string extension = ".trv";
	if (genConfig->getJSON().HasKey("FileExtension")
			&& ((genConfig->getJSON())["FileExtension"].GetType()
					== json::ValueType::StringVal)) {
		extension = (genConfig->getJSON())["FileExtension"].ToString();
		glassutil::CLogit::log(
				glassutil::log_level::info,
				"gen-travel-times-app: Using FileExtension: " + extension);
	}

	logger::log("info", "gen-travel-times-app: Setup.");

	// create generator
	traveltime::CGenTrv *travelGenerator = new traveltime::CGenTrv();

	// set up traveltime to use our logging
	glassutil::CLogit::setLogCallback(
			std::bind(logTravelTimes, std::placeholders::_1));

	travelGenerator->setup(model, path, extension);

	logger::log("info", "gen-travel-times-app: Startup.");

	if (genConfig->getJSON().HasKey("Branches")
			&& ((genConfig->getJSON())["Branches"].GetType()
					== json::ValueType::ArrayVal)) {
		// get the array of phase entries
		json::Array branches =
				(genConfig->getJSON())["Branches"].ToArray();

		// for each branch in the array
		for (auto branchVal : branches) {
			// make sure the phase is an object
			if (branchVal.GetType() != json::ValueType::ObjectVal) {
				continue;
			}

			// get this branch object
			json::Object branchObj = branchVal.ToObject();

			if (travelGenerator->generate(&branchObj) != true) {
				logger::log(
						"error",
						"gen-travel-times-app: Failed to generate travel time "
						"file.");

        // ========================
        // DK REVIEW 20180621
        //
        // cleanup
        // returncode = 1;
        // goto cleanup_ttgenerator;
        // :cleanup_ttgenerator
        //   delete(travelGenerator)
        // :cleanup_config
        //   delete(genConfig)
        //   return(returncode)
        delete (genConfig);
				delete (travelGenerator);
				return (1);
			}
		}
	}

	logger::log("info", "gen-travel-times-app: Shutdown.");

	// cleanup
	delete (genConfig);
	delete (travelGenerator);

	// done
	return (0);
}
// ========================
// DK REVIEW 20180621
// I'm a fan of putting the function name
// in a comment with the closing brace, 
// whenever you have a function that's more than a page
// long.  Just helps confirm that you are still viewing
// the same function.  Of course you already added that 
// function delineation thing (which is missing in this code),
// so maybe that's not neccessary.
// ========================
