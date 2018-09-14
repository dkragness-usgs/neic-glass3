#include <json.h>
#include <sstream>
#include <cmath>
#include <utility>
#include <tuple>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>
#include <ctime>
#include "Glass.h"
#include "Pick.h"
#include "Site.h"
#include "Logit.h"
#include "Node.h"
#include "Trigger.h"
#include "Hypo.h"
#include "Web.h"

namespace glasscore {

// ---------------------------------------------------------CSite
CSite::CSite() {
	clear();
}

// ---------------------------------------------------------CSite
CSite::CSite(std::string sta, std::string comp, std::string net,
				std::string loc, double lat, double lon, double elv,
				double qual, bool enable, bool useTele) {
	// pass to initialization function
	initialize(sta, comp, net, loc, lat, lon, elv, qual, enable, useTele);
}

// ---------------------------------------------------------CSite
CSite::CSite(std::shared_ptr<json::Object> site) {
	clear();

	// null check json
	if (site == NULL) {
		glassutil::CLogit::log(glassutil::log_level::error,
								"CSite::CSite: NULL json site.");
		return;
	}

	// check type
	if (site->HasKey("Type")
			&& ((*site)["Type"].GetType() == json::ValueType::StringVal)) {
		std::string type = (*site)["Type"].ToString();

		if (type != "StationInfo") {
			glassutil::CLogit::log(
					glassutil::log_level::warn,
					"CSite::CSite: Non-StationInfo message passed in.");
			return;
		}
	} else {
		glassutil::CLogit::log(glassutil::log_level::error,
								"CSite::CSite: Missing required Type Key.");
		return;
	}

	// site definition variables
	std::string station = "";
	std::string channel = "";
	std::string network = "";
	std::string location = "";
	double latitude = 0;
	double longitude = 0;
	double elevation = 0;

	// optional values
	double quality = 0;
	bool enable = true;
	bool useForTelesiesmic = true;

	// get site information from json
	// scnl
	if (((*site).HasKey("Site"))
			&& ((*site)["Site"].GetType() == json::ValueType::ObjectVal)) {
		json::Object siteobj = (*site)["Site"].ToObject();

		// station
		if (siteobj.HasKey("Station")
				&& (siteobj["Station"].GetType() == json::ValueType::StringVal)) {
			station = siteobj["Station"].ToString();
		} else {
			glassutil::CLogit::log(
					glassutil::log_level::error,
					"CSite::CSite: Missing required Station Key.");

			return;
		}

		// channel (optional)
		if (siteobj.HasKey("Channel")
				&& (siteobj["Channel"].GetType() == json::ValueType::StringVal)) {
			channel = siteobj["Channel"].ToString();
		} else {
			channel = "";
		}

		// network
		if (siteobj.HasKey("Network")
				&& (siteobj["Network"].GetType() == json::ValueType::StringVal)) {
			network = siteobj["Network"].ToString();
		} else {
			glassutil::CLogit::log(
					glassutil::log_level::error,
					"CSite::CSite: Missing required Network Key.");

			return;
		}

		// location (optional)
		if (siteobj.HasKey("Location")
				&& (siteobj["Location"].GetType() == json::ValueType::StringVal)) {
			location = siteobj["Location"].ToString();
		} else {
			location = "";
		}

		// -- is used by some networks to represent an empty
		// location code
		if (location == "--") {
			location = "";
		}
	} else {
		glassutil::CLogit::log(glassutil::log_level::error,
								"CSite::CSite: Missing required Site Object.");

		return;
	}

	// latitude for this site
	if (((*site).HasKey("Latitude"))
			&& ((*site)["Latitude"].GetType() == json::ValueType::DoubleVal)) {
		latitude = (*site)["Latitude"].ToDouble();
	} else {
		glassutil::CLogit::log(glassutil::log_level::error,
								"CSite::CSite: Missing required Latitude Key.");

		return;
	}

	// longitude for this site
	if (((*site).HasKey("Longitude"))
			&& ((*site)["Longitude"].GetType() == json::ValueType::DoubleVal)) {
		longitude = (*site)["Longitude"].ToDouble();
	} else {
		glassutil::CLogit::log(
				glassutil::log_level::error,
				"CSite::CSite: Missing required Longitude Key.");

		return;
	}

	// elevation for this site
	if (((*site).HasKey("Elevation"))
			&& ((*site)["Elevation"].GetType() == json::ValueType::DoubleVal)) {
		elevation = (*site)["Elevation"].ToDouble();
	} else {
		glassutil::CLogit::log(
				glassutil::log_level::error,
				"CSite::CSite: Missing required Elevation Key.");

		return;
	}

	// quality for this site (if present)
	if (((*site).HasKey("Quality"))
			&& ((*site)["Quality"].GetType() == json::ValueType::DoubleVal)) {
		quality = (*site)["Quality"].ToDouble();
	} else {
		quality = 1.0;
	}

	// enable for this site (if present)
	if (((*site).HasKey("Enable"))
			&& ((*site)["Enable"].GetType() == json::ValueType::BoolVal)) {
		enable = (*site)["Enable"].ToBool();
	} else {
		enable = true;
	}

	// enable for this site (if present)
	if (((*site).HasKey("UseForTeleseismic"))
			&& ((*site)["UseForTeleseismic"].GetType()
					== json::ValueType::BoolVal)) {
		useForTelesiesmic = (*site)["UseForTeleseismic"].ToBool();
	} else {
		useForTelesiesmic = true;
	}

	// pass to initialization function
	initialize(station, channel, network, location, latitude, longitude,
				elevation, quality, enable, useForTelesiesmic);
}

// --------------------------------------------------------initialize
bool CSite::initialize(std::string sta, std::string comp, std::string net,
						std::string loc, double lat, double lon, double elv,
						double qual, bool enable, bool useTele) {
	clear();

	std::lock_guard<std::recursive_mutex> guard(m_SiteMutex);

	// generate scnl
	m_sSCNL = "";

	// station, required
	if (sta != "") {
		m_sSCNL += sta;
	} else {
		glassutil::CLogit::log(glassutil::log_level::error,
								"CSite::initialize: missing sSite.");
		return (false);
	}

	// component, optional
	if (comp != "") {
		m_sSCNL += "." + comp;
	}

	// network, required
	if (net != "") {
		m_sSCNL += "." + net;
	} else {
		glassutil::CLogit::log(glassutil::log_level::error,
								"CSite::initialize: missing sNet.");
		return (false);
	}

	// location, optional
	if (loc != "") {
		m_sSCNL += "." + loc;
	}

	// fill in site/net/etc
	m_sSite = sta;
	m_sNetwork = net;
	m_sComponent = comp;
	m_sLocation = loc;

	// set geographic location
	setLocation(lat, lon, -0.001 * elv);

	// quality
	m_dQuality = qual;

	// copy use
	m_bEnable = enable;
	m_bUse = true;
	m_bUseForTeleseismic = useTele;

	if (CGlass::getMaxNumPicksPerSite() > -1) {
		m_iPickMax = CGlass::getMaxNumPicksPerSite();
	}

	return (true);
}

// ---------------------------------------------------------~CSite
CSite::~CSite() {
	clear();
}

// --------------------------------------------------------clear
void CSite::clear() {
	std::lock_guard<std::recursive_mutex> guard(m_SiteMutex);
	// clear scnl
	m_sSCNL = "";
	m_sSite = "";
	m_sComponent = "";
	m_sNetwork = "";
	m_sLocation = "";

	m_bUse = true;
	m_bEnable = true;
	m_bUseForTeleseismic = true;
	m_dQuality = 1.0;

	// clear geographic
	m_Geo = glassutil::CGeo();
	m_daUnitVectors[0] = 0;
	m_daUnitVectors[1] = 0;
	m_daUnitVectors[2] = 0;

	// clear lists
	m_vNodeMutex.lock();
	m_vNode.clear();
	m_vNodeMutex.unlock();

	vPickMutex.lock();
	m_vPickList.clear();
	vPickMutex.unlock();

	// reset max picks
	m_iPickMax = 200;

	// reset last pick added time
	m_tLastPickAdded = std::time(NULL);

	// reset picks since last check
	setPickCountSinceCheck(0);
}

// --------------------------------------------------------update
void CSite::update(CSite *aSite) {
	std::lock_guard<std::recursive_mutex> guard(m_SiteMutex);
	// scnl check
	if (m_sSCNL != aSite->getSCNL()) {
		return;
	}

	// update station quality metrics
	m_bEnable = aSite->getEnable();
	m_bUseForTeleseismic = aSite->getUseForTeleseismic();
	m_dQuality = aSite->getQuality();

	// update location
	m_Geo = glassutil::CGeo(aSite->getGeo());
	double vec[3];
	aSite->getUnitVectors(vec);

	m_daUnitVectors[0] = vec[0];
	m_daUnitVectors[1] = vec[1];
	m_daUnitVectors[2] = vec[2];

	// copy statistics
	m_tLastPickAdded = aSite->getTLastPickAdded();

	// leave lists, and pointers alone
}

// --------------------------------------------------------getUnitVectors
double * CSite::getUnitVectors(double * vec) {
	if (vec == NULL) {
		return (NULL);
	}

	vec[0] = m_daUnitVectors[0];
	vec[1] = m_daUnitVectors[1];
	vec[2] = m_daUnitVectors[2];

	return (vec);
}

// ---------------------------------------------------------setLocation
void CSite::setLocation(double lat, double lon, double z) {
	std::lock_guard<std::recursive_mutex> guard(m_SiteMutex);
	// construct unit vector in cartesian earth coordinates
	double rxy = cos(DEG2RAD * lat);
	m_daUnitVectors[0] = rxy * cos(DEG2RAD * lon);
	m_daUnitVectors[1] = rxy * sin(DEG2RAD * lon);
	m_daUnitVectors[2] = sin(DEG2RAD * lat);

	// set geographic object
	m_Geo.setGeographic(lat, lon, 6371.0 - z);
}

// ---------------------------------------------------------getDelta
double CSite::getDelta(glassutil::CGeo *geo2) {
	// nullcheck
	if (geo2 == NULL) {
		glassutil::CLogit::log(glassutil::log_level::warn,
								"CSite::getDelta: NULL CGeo provided.");
		return (0);
	}

	// use CGeo to calculate distance in radians
	return (m_Geo.delta(geo2));
}

// ---------------------------------------------------------getDistance
double CSite::getDistance(std::shared_ptr<CSite> site) {
	// nullcheck
	if (site == NULL) {
		glassutil::CLogit::log(glassutil::log_level::warn,
								"CSite::getDistance: NULL CSite provided.");
		return (0);
	}

	// use unit vectors in cartesian earth coordinates
	// to quickly great circle distance in km
	double dot = 0;
	double dkm;
	for (int i = 0; i < 3; i++) {
		dot += m_daUnitVectors[i] * site->m_daUnitVectors[i];
	}
	dkm = 6366.2 * acos(dot);

	// return distance
	return (dkm);
}

// ---------------------------------------------------------addPick
void CSite::addPick(std::shared_ptr<CPick> pck) {
	// lock for editing
	std::lock_guard<std::mutex> guard(vPickMutex);

	// nullcheck
	if (pck == NULL) {
		glassutil::CLogit::log(glassutil::log_level::warn,
								"CSite::addPick: NULL CPick provided.");
		return;
	}

	// ensure this pick is for this site
	if (pck->getSite()->m_sSCNL != m_sSCNL) {
		glassutil::CLogit::log(
				glassutil::log_level::warn,
				"CSite::addPick: CPick for different site: (" + m_sSCNL + "!="
						+ pck->getSite()->m_sSCNL + ")");
		return;
	}

	// check to see if we're at the pick limit
	if (m_vPickList.size() >= m_iPickMax) {
		// erase first pick from vector
		m_vPickList.erase(m_vPickList.begin());
	}

	// add pick to site pick multiset
	m_vPickList.push_back(pck);

	// remember the time the last pick was added
	m_tLastPickAdded = std::time(NULL);

	// keep track of how many picks
	setPickCountSinceCheck(getPickCountSinceCheck() + 1);
}

// ---------------------------------------------------------removePick
void CSite::removePick(std::shared_ptr<CPick> pck) {
	// lock for editing
	std::lock_guard<std::mutex> guard(vPickMutex);

	// nullcheck
	if (pck == NULL) {
		glassutil::CLogit::log(glassutil::log_level::warn,
								"CSite::removePick: NULL CPick provided.");
		return;
	}

	// remove pick from site pick vector
	for (auto it = m_vPickList.begin(); it != m_vPickList.end();) {
		auto aPck = *it;

		// erase target pick
		if (aPck->getID() == pck->getID()) {
			it = m_vPickList.erase(it);
		} else {
			++it;
		}
	}
}

// ---------------------------------------------------------addNode
void CSite::addNode(std::shared_ptr<CNode> node, double travelTime1,
					double travelTime2) {
	// lock for editing
	std::lock_guard<std::mutex> guard(m_vNodeMutex);

	// nullcheck
	if (node == NULL) {
		glassutil::CLogit::log(glassutil::log_level::warn,
								"CSite::addNode: NULL CNode provided.");
		return;
	}
	// check travel times
	/*
	 if ((travelTime1 < 0) && (travelTime2 < 0)) {
	 glassutil::CLogit::log(glassutil::log_level::error,
	 "CSite::addNode: No valid travel times.");
	 return;
	 }
	 */

	// add node link to vector of nodes linked to this site
	// NOTE: no duplication check, but multiple nodes from the
	// same web can exist at the same site (travel times would be different)
	NodeLink link = std::make_tuple(node, travelTime1, travelTime2);
	m_vNode.push_back(link);
}

// ---------------------------------------------------------removeNode
void CSite::removeNode(std::string nodeID) {
	// lock for editing
	std::lock_guard<std::mutex> guard(m_vNodeMutex);

	// nullcheck
	if (nodeID == "") {
		glassutil::CLogit::log(glassutil::log_level::warn,
								"CSite::removeNode: empty web name provided.");
		return;
	}

	// clean up expired pointers
	for (auto it = m_vNode.begin(); it != m_vNode.end();) {
		if (std::get<LINK_PTR>(*it).expired() == true) {
			it = m_vNode.erase(it);
		} else {
			++it;
		}
	}

	for (auto it = m_vNode.begin(); it != m_vNode.end();) {
		if (auto aNode = std::get<LINK_PTR>(*it).lock()) {
			// erase target pick
			if (aNode->getID() == nodeID) {
				it = m_vNode.erase(it);
				return;
			} else {
				++it;
			}
		} else {
			++it;
		}
	}
}

// ---------------------------------------------------------nucleate
std::vector<std::shared_ptr<CTrigger>> CSite::nucleate(double tPick) {
	std::lock_guard<std::mutex> guard(m_vNodeMutex);

	// create trigger vector
	std::vector<std::shared_ptr<CTrigger>> vTrigger;

	// are we enabled?
	m_SiteMutex.lock();
	if (m_bUse == false) {
		m_SiteMutex.unlock();
		return (vTrigger);
	}
	m_SiteMutex.unlock();

	// for each node linked to this site
	for (const auto &link : m_vNode) {
		// compute potential origin time from tPick and travel time to node
		// first get traveltime1 to node
		double travelTime1 = std::get< LINK_TT1>(link);

		// second get traveltime2 to node
		double travelTime2 = std::get< LINK_TT2>(link);

		// third get shared pointer to node
		std::shared_ptr<CNode> node = std::get<LINK_PTR>(link).lock();

		if (node == NULL) {
			continue;
		}

		if (node->getEnabled() == false) {
			continue;
		}

		// compute first origin time
		double tOrigin1 = -1;
		if (travelTime1 > 0) {
			tOrigin1 = tPick - travelTime1;
		}

		// compute second origin time
		double tOrigin2 = -1;
		if (travelTime2 > 0) {
			tOrigin2 = tPick - travelTime2;
		}

		// attempt to nucleate an event located
		// at the current node with the potential origin times
		bool primarySuccessful = false;
		if (tOrigin1 > 0) {
			std::shared_ptr<CTrigger> trigger1 = node->nucleate(tOrigin1);

			if (trigger1 != NULL) {
				// if node triggered, add to triggered vector
				addTriggerToList(&vTrigger, trigger1);
				primarySuccessful = true;
			}
		}

		// only attempt secondary phase nucleation if primary nucleation
		// was unsuccessful
		if ((primarySuccessful == false) && (tOrigin2 > 0)) {
			std::shared_ptr<CTrigger> trigger2 = node->nucleate(tOrigin2);

			if (trigger2 != NULL) {
				// if node triggered, add to triggered vector
				addTriggerToList(&vTrigger, trigger2);
			}
		}

		if ((tOrigin1 < 0) && (tOrigin2 < 0)) {
			glassutil::CLogit::log(
					glassutil::log_level::warn,
					"CSite::nucleate: " + m_sSCNL + " No valid travel times. ("
							+ std::to_string(travelTime1) + ", "
							+ std::to_string(travelTime2) + ") web: "
							+ node->getWeb()->getName());
		}
	}

	return (vTrigger);
}

// ---------------------------------------------------------addTrigger
void CSite::addTriggerToList(std::vector<std::shared_ptr<CTrigger>> *vTrigger,
								std::shared_ptr<CTrigger> trigger) {
	if (trigger == NULL) {
		return;
	}
	if (trigger->getWeb() == NULL) {
		return;
	}

	// clean up expired pointers
	for (auto it = vTrigger->begin(); it != vTrigger->end();) {
		std::shared_ptr<CTrigger> aTrigger = (*it);

		// if current trigger is part of latest trigger's web
		if (trigger->getWeb()->getName() == aTrigger->getWeb()->getName()) {
			// if current trigger's sum is less than latest trigger's sum
			if (trigger->getBayesValue() > aTrigger->getBayesValue()) {
				it = vTrigger->erase(it);
				it = vTrigger->insert(it, trigger);
			}

			// we're done
			return;
		} else {
			++it;
		}
	}

	// add triggering node to vector of triggered nodes
	vTrigger->push_back(trigger);
}

// ---------------------------------------------------------getNodeLinksCount
int CSite::getNodeLinksCount() const {
	std::lock_guard<std::mutex> guard(m_vNodeMutex);
	int size = m_vNode.size();

	return (size);
}

// ---------------------------------------------------------getEnable
bool CSite::getEnable() const {
	return (m_bEnable);
}

// ---------------------------------------------------------setEnable
void CSite::setEnable(bool enable) {
	m_bEnable = enable;
}

// ---------------------------------------------------------getUse
bool CSite::getUse() const {
	return (m_bUse && m_bEnable);
}

// ---------------------------------------------------------setUse
void CSite::setUse(bool use) {
	m_bUse = use;
}

// ---------------------------------------------------------getUseForTeleseismic
bool CSite::getUseForTeleseismic() const {
	return (m_bUseForTeleseismic);
}

// ---------------------------------------------------------setUseForTeleseismic
void CSite::setUseForTeleseismic(bool useForTele) {
	m_bUseForTeleseismic = useForTele;
}

// ---------------------------------------------------------getQuality
double CSite::getQuality() const {
	return (m_dQuality);
}

// ---------------------------------------------------------setQuality
void CSite::setQuality(double qual) {
	m_dQuality = qual;
}

// ---------------------------------------------------------getGeo
glassutil::CGeo &CSite::getGeo() {
	return (m_Geo);
}

// ---------------------------------------------------------getPickMax
int CSite::getPickMax() const {
	return (m_iPickMax);
}

// ---------------------------------------------------------getComponent
const std::string& CSite::getComponent() const {
	return (m_sComponent);
}

// ---------------------------------------------------------getLocation
const std::string& CSite::getLocation() const {
	return (m_sLocation);
}

// ---------------------------------------------------------getNetwork
const std::string& CSite::getNetwork() const {
	return (m_sNetwork);
}

// ---------------------------------------------------------getSCNL
const std::string& CSite::getSCNL() const {
	return (m_sSCNL);
}

// ---------------------------------------------------------getSite
const std::string& CSite::getSite() const {
	return (m_sSite);
}

// ---------------------------------------------------------getVPick
const std::vector<std::shared_ptr<CPick>> CSite::getVPick() const {
	std::lock_guard<std::mutex> guard(vPickMutex);
	return (m_vPickList);
}

// ---------------------------------------------------------getTLastPickAdded
time_t CSite::getTLastPickAdded() const {
	return (m_tLastPickAdded);
}

// ------------------------------------------------------setPickCountSinceCheck
void CSite::setPickCountSinceCheck(int count) {
	m_iPickCountSinceCheck = count;
}

// ------------------------------------------------------getPickCountSinceCheck
int CSite::getPickCountSinceCheck() const {
	return (m_iPickCountSinceCheck);
}

}  // namespace glasscore
