#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include "Trigger.h"
#include "Web.h"
#include "Pick.h"
#include "Site.h"
#include "SiteList.h"
#include "Logit.h"

#define LATITUDE -21.849968
#define LONGITUDE 170.034750
#define DEPTH 10.000000
#define TIME 3648585210.926340
#define SUM 3.5
#define COUNT 1

#define SITEJSON "{\"Type\":\"StationInfo\",\"Elevation\":2326.000000,\"Latitude\":45.822170,\"Longitude\":-112.451000,\"Site\":{\"Station\":\"LRM\",\"Channel\":\"EHZ\",\"Network\":\"MB\",\"Location\":\"\"},\"Enable\":true,\"Quality\":1.0,\"UseForTeleseismic\":true}" // NOLINT

#define PICKJSON "{\"ID\":\"20682831\",\"Phase\":\"P\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"EHZ\",\"Location\":\"\",\"Network\":\"MB\",\"Station\":\"LRM\"},\"Source\":{\"AgencyID\":\"228041013\",\"Author\":\"228041013\"},\"Time\":\"2014-12-23T00:01:43.599Z\",\"Type\":\"Pick\"}"  // NOLINT

#define NAME "TestWeb"
#define THRESH 1.4
#define NUMDETECT 5
#define NUMNUCLEATE 4
#define RESOLUTION 100.0
#define NUMROWS 3
#define NUMCOLS 4
#define NUMZ 1
#define UPDATE true

// tests to see if the node can be constructed
TEST(TriggerTest, Construction) {
	glassutil::CLogit::disable();

	// construct a web
	std::shared_ptr<traveltime::CTravelTime> nullTrav;
	glasscore::CWeb * testWeb = new glasscore::CWeb(std::string(NAME),
	THRESH,
													NUMDETECT,
													NUMNUCLEATE,
													RESOLUTION,
													NUMROWS,
													NUMCOLS, NUMZ,
													UPDATE,
													nullTrav, nullTrav);

	// create  shared pointer to the site
	// create json objects from the strings
	std::shared_ptr<json::Object> siteJSON = std::make_shared<json::Object>(
			json::Object(json::Deserialize(std::string(SITEJSON))));
	std::shared_ptr<json::Object> pickJSON = std::make_shared<json::Object>(
			json::Object(json::Deserialize(std::string(PICKJSON))));

	// construct a sitelist
	glasscore::CSiteList * testSiteList = new glasscore::CSiteList();

	// add sites to site list
	testSiteList->addSite(siteJSON);

	// create pick
	glasscore::CPick * testPick = new glasscore::CPick(pickJSON, 1,
														testSiteList);

	std::shared_ptr<glasscore::CPick> sharedPick(testPick);

	// create pick vector
	std::vector<std::shared_ptr<glasscore::CPick>> picks;
	picks.push_back(sharedPick);

	// construct a trigger
	glasscore::CTrigger * testTrigger = new glasscore::CTrigger();

	// latitude
	ASSERT_EQ(0, testTrigger->getLat())<< "Trigger Latitude 0";

	// longitude
	ASSERT_EQ(0, testTrigger->getLon())<< "Trigger Longitude 0";

	// depth
	ASSERT_EQ(0, testTrigger->getZ())<< "Trigger Depth 0";

	// time
	ASSERT_EQ(0, testTrigger->getTOrg())<< "Trigger Time 0";

	// resolution
	ASSERT_EQ(0, testTrigger->getResolution())<< "Trigger Resolution 0";

	// Sum
	ASSERT_EQ(0, testTrigger->getSum())<< "Trigger Sum 0";

	// Count
	ASSERT_EQ(0, testTrigger->getCount())<< "Trigger Count 0";

	// web
	ASSERT_TRUE(testTrigger->getWeb() == NULL)<< "PWeb null";

	// pick list
	ASSERT_EQ(0, testTrigger->getVPick().size())<< "Trigger pick Count 0";

	// init
	testTrigger->initialize(LATITUDE,
	LONGITUDE,
							DEPTH,
							TIME,
							RESOLUTION,
							SUM,
							COUNT, picks, testWeb);

	// latitude
	ASSERT_EQ(LATITUDE, testTrigger->getLat())<< "Trigger Latitude Check";

	// longitude
	ASSERT_EQ(LONGITUDE, testTrigger->getLon())<< "Trigger Longitude Check";

	// depth
	ASSERT_EQ(DEPTH, testTrigger->getZ())<< "Trigger Depth Check";

	// time
	ASSERT_EQ(TIME, testTrigger->getTOrg())<< "Trigger Time Check";

	// resolution
	ASSERT_EQ(RESOLUTION, testTrigger->getResolution())<< "Trigger Resolution "
	"Check";

	// Sum
	ASSERT_EQ(SUM, testTrigger->getSum())<< "Trigger Sum Check";

	// Count
	ASSERT_EQ(COUNT, testTrigger->getCount())<< "Trigger Count Check";

	// web
	ASSERT_STREQ(std::string(NAME).c_str(),
			testTrigger->getWeb()->getName().c_str())<<
	"Trigger Web Name Matches";

	// pick list
	ASSERT_EQ(COUNT, testTrigger->getVPick().size())<< "Trigger pick Count "
	"Check";

	// clear
	testTrigger->clear();

	// latitude
	ASSERT_EQ(0, testTrigger->getLat())<< "Trigger Latitude 0";

	// longitude
	ASSERT_EQ(0, testTrigger->getLon())<< "Trigger Longitude 0";

	// depth
	ASSERT_EQ(0, testTrigger->getZ())<< "Trigger Depth 0";

	// time
	ASSERT_EQ(0, testTrigger->getTOrg())<< "Trigger Time 0";

	// resolution
	ASSERT_EQ(0, testTrigger->getResolution())<< "Trigger Resolution 0";

	// Sum
	ASSERT_EQ(0, testTrigger->getSum())<< "Trigger Sum 0";

	// Count
	ASSERT_EQ(0, testTrigger->getCount())<< "Trigger Count 0";

	// web
	ASSERT_TRUE(testTrigger->getWeb() == NULL)<< "PWeb null";

	// pick list
	ASSERT_EQ(0, testTrigger->getVPick().size())<< "Trigger pick Count 0";
}
