#include <json.h>
#include <string>
#include <memory>
#include "Date.h"
#include "Pid.h"
#include "Web.h"
#include "Node.h"
#include "PickList.h"
#include "HypoList.h"
#include "Hypo.h"
#include "Pick.h"
#include "Detection.h"
#include "Site.h"
#include "Glass.h"
#include "Logit.h"

namespace glasscore {

// ---------------------------------------------------------CDetection
CDetection::CDetection() {
	clear();
}

// ---------------------------------------------------------~CDetection
CDetection::~CDetection() {
	clear();
}

// ---------------------------------------------------------clear
void CDetection::clear() {
	std::lock_guard<std::recursive_mutex> detectionGuard(detectionMutex);
	pGlass = NULL;
}

// ---------------------------------------------------------Dispatch
bool CDetection::dispatch(std::shared_ptr<json::Object> com) {
	// null check json
	if (com == NULL) {
		glassutil::CLogit::log(
				glassutil::log_level::error,
				"CDetection::dispatch: NULL json communication.");
		return (false);
	}

	// check for a command
	if (com->HasKey("Cmd")
			&& ((*com)["Cmd"].GetType() == json::ValueType::StringVal)) {
		// dispatch to appropriate function based on Cmd value
		json::Value v = (*com)["Cmd"].ToString();

		// clear all data  // DK REVIEW 20180820 - why does this function look for "ClearGlass"?  it doesn't do anything with it!
		if (v == "ClearGlass") {
			// ClearGlass is also relevant to other glass
			// components, return false so they also get a
			// chance to process it
			return (false);
		}
	}

  // DK REVIEW 20180829 - let's make sure this is standardized.  Let's have either "Cmd" or "Type" 
  // for each message, but NOT BOTH!!!!


	// Input data can have Type keys
	if (com->HasKey("Type")
			&& ((*com)["Type"].GetType() == json::ValueType::StringVal)) {
		// dispatch to appropriate function based on Cmd value
		json::Value v = (*com)["Type"].ToString();

		// add a detection
		if (v == "Detection") {
			return (process(com));
		}
	}

	// this communication was not handled
	return (false);
}

// ---------------------------------------------------------process
bool CDetection::process(std::shared_ptr<json::Object> com) {
	// null check json
	if (com == NULL) {
		glassutil::CLogit::log(
				glassutil::log_level::error,
				"CDetection::process: NULL json communication.");
		return (false);
	}
	if (pGlass == NULL) {
		glassutil::CLogit::log(glassutil::log_level::error,
								"CDetection::process: NULL pGlass.");

		return (false);
	}

	std::lock_guard<std::recursive_mutex> detectionGuard(detectionMutex);  // DK REVIEW 20180829 - Why does this class need a mutex?
                                                                         // where's the critical section or configuration data?

	// detection definition variables
	double torg = 0;
	double lat = 0;
	double lon = 0;
	double z = 0;

	// Get information from hypocenter
	if (com->HasKey("Hypocenter")
			&& ((*com)["Hypocenter"].GetType() == json::ValueType::ObjectVal)) {
		json::Object hypocenter = (*com)["Hypocenter"].ToObject();

		// get time from hypocenter
		if (hypocenter.HasKey("Time")
				&& (hypocenter["Time"].GetType() == json::ValueType::StringVal)) {
			// get time string
			std::string tiso = hypocenter["Time"].ToString();  // DK REVIEW 20180829 - tiso?  need better var name.  tISOHypo  or tHypo_ISOFormat

			// convert time
			glassutil::CDate dt = glassutil::CDate();
			torg = dt.decodeISO8601Time(tiso);
		} else {
			glassutil::CLogit::log(
					glassutil::log_level::error,
					"CDetection::process: Missing required Hypocenter Time Key.");

			return (false);
		}

		// get latitude from hypocenter
		if (hypocenter.HasKey("Latitude")
				&& (hypocenter["Latitude"].GetType()
						== json::ValueType::DoubleVal)) {
			lat = hypocenter["Latitude"].ToDouble();

		} else {
			glassutil::CLogit::log(
					glassutil::log_level::error,
					"CDetection::process: Missing required Hypocenter Latitude"
					" Key.");

			return (false);
		}

		// get longitude from hypocenter
		if (hypocenter.HasKey("Longitude")
				&& (hypocenter["Longitude"].GetType()
						== json::ValueType::DoubleVal)) {
			lon = hypocenter["Longitude"].ToDouble();
		} else {
			glassutil::CLogit::log(
					glassutil::log_level::error,
					"CDetection::process: Missing required Hypocenter Longitude"
					" Key.");

			return (false);
		}

		// get depth from hypocenter
		if (hypocenter.HasKey("Depth")
				&& (hypocenter["Depth"].GetType() == json::ValueType::DoubleVal)) {
			z = hypocenter["Depth"].ToDouble();
		} else {
			glassutil::CLogit::log(
					glassutil::log_level::error,
					"CDetection::process: Missing required Hypocenter Depth"
					" Key.");

			return (false);
		}
	} else {
		glassutil::CLogit::log(
				glassutil::log_level::error,
				"CDetection::process: Missing required Hypocenter Key.");

		return (false);
	}

	// Check to see if hypo already exists. We could also
	// check location at this point, but it seems unlikely
	// that would add much value
	// define a three minute search window
	// NOTE: Hard coded.
	double t1 = torg - 90.0;
	double t2 = torg + 90.0;

	// search for the first hypocenter in the window
	std::shared_ptr<CHypo> hypo = pGlass->getHypoList()->findHypo(t1, t2);

	// check to see if we found a hypo
	if (hypo != NULL) {  // DK REVIEW 20180829 -   This needs to be a loop, that loops through all the hypocenters in the list and quits when it finds one
                       // and otherwise creates a new one.
		// found a hypo
		// calculate distance
		glassutil::CGeo geo1;
		geo1.setGeographic(lat, lon, z);
		glassutil::CGeo geo2;
		geo2.setGeographic(hypo->getLat(), hypo->getLon(), hypo->getZ());
		double delta = RAD2DEG * geo1.delta(&geo2);

		// if the detection is more than 5 degrees away, it isn't a match
		// NOTE: Hard coded.
		if (delta > 5.0) {  // DK REVIEW 20180829 - Magic Number.  Define it someplace, preferably in Glass.


      // do this Only when no matching Hypo is found.
			// detections don't have a second travel time
			std::shared_ptr<traveltime::CTravelTime> nullTrav;

			// create new hypo
			hypo = std::make_shared<CHypo>(lat, lon, z, torg,
											glassutil::CPid::pid(), "Detection",
											0.0, 0.0, 0,
											pGlass->getTrvDefault(),
											nullTrav, pGlass->getTTT());

			// set hypo glass pointer and such
			hypo->setGlass(pGlass);
      // DK REVIEW 20180829 - These should all be properties of Glass, not of the Hypo
      // If there's some reason they need to be in Hypo, then label them "*Cache",
      // and be prepared to deal with updating them.
			hypo->setCutFactor(pGlass->getCutFactor());
			hypo->setCutPercentage(pGlass->getCutPercentage());
			hypo->setCutMin(pGlass->getCutMin());

			// process hypo using evolve
			if (pGlass->getHypoList()->evolve(hypo)) {    // DK REVIEW 20180829 -  I don't understand why evolve() is in HypoList instead of Hypo.  Maybe there's some data in HypoList that it needs?
				// add to hypo list
				pGlass->getHypoList()->addHypo(hypo);   // DK REVIEW 20180829 -  Why not let addHypo() call evolve() or deal with evolving the Hypo.
                                                // I am the detector.  I detected this Hypo.  Pass it to the Assoc/Validate group and let them figure out where it stands.
                                                // Kind of a separation of duties / throw it over the fence model.
			}
		} else {
			// existing hypo, now hwat?
			// schedule hypo for processing?
			pGlass->getHypoList()->pushFifo(hypo);   // DK REVIEW 20180829 -  Why?  You haven't added anything to the event.
                                               // You said, hey I got external notice of another event that seems close to this one,
                                               // and, uh..., I didn't add any data or record the external event.  So, uh...
                                               // I want it to feel like it wasn't a total waste and I'm all out of Jamba Juice gift
                                               // cards, so as a consolation prize, I'm gonna rework the associated event?
		}
	}

	// done
	return (true);
}

const CGlass* CDetection::getGlass() const {
	std::lock_guard<std::recursive_mutex> detectionGuard(detectionMutex);
	return (pGlass);
}

void CDetection::setGlass(CGlass* glass) {
	std::lock_guard<std::recursive_mutex> detectionGuard(detectionMutex);
	pGlass = glass;
}
}  // namespace glasscore
