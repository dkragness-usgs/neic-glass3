#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "Correlation.h"
#include "CorrelationList.h"
#include "Site.h"
#include "SiteList.h"
#include "Logit.h"

#define SITEJSON "{\"Type\":\"StationInfo\",\"Elevation\":2326.000000,\"Latitude\":45.822170,\"Longitude\":-112.451000,\"Site\":{\"Station\":\"LRM\",\"Channel\":\"EHZ\",\"Network\":\"MB\",\"Location\":\"\"},\"Enable\":true,\"Quality\":1.0,\"UseForTeleseismic\":true}"  // NOLINT
#define SITE2JSON "{\"Type\":\"StationInfo\",\"Elevation\":1342.000000,\"Latitude\":46.711330,\"Longitude\":-111.831200,\"Site\":{\"Station\":\"HRY\",\"Channel\":\"EHZ\",\"Network\":\"MB\",\"Location\":\"\"},\"Enable\":true,\"Quality\":1.0,\"UseForTeleseismic\":true}"  // NOLINT
#define SITE3JSON "{\"Type\":\"StationInfo\",\"Elevation\":1589.000000,\"Latitude\":45.596970,\"Longitude\":-111.629670,\"Site\":{\"Station\":\"BOZ\",\"Channel\":\"BHZ\",\"Network\":\"US\",\"Location\":\"00\"},\"Enable\":true,\"Quality\":1.0,\"UseForTeleseismic\":true}"  // NOLINT

#define CORRELATIONJSON "{\"ID\":\"20682831\",\"Phase\":\"P\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"EHZ\",\"Location\":\"\",\"Network\":\"MB\",\"Station\":\"LRM\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"TestAuthor\"},\"Time\":\"2014-12-23T00:01:43.599Z\",\"Type\":\"Correlation\",\"Correlation\":2.65,\"Hypocenter\":{\"Latitude\":40.3344,\"Longitude\":-121.44,\"Depth\":32.44,\"Time\":\"2014-12-23T00:01:55.599Z\"},\"EventType\":\"earthquake\",\"Magnitude\":2.14,\"SNR\":3.8,\"ZScore\":33.67,\"DetectionThreshold\":1.5,\"ThresholdType\":\"minimum\"}"  // NOLINT
#define CORRELATION2JSON "{\"ID\":\"20682832\",\"Phase\":\"P\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"EHZ\",\"Location\":\"\",\"Network\":\"MB\",\"Station\":\"HRY\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"TestAuthor\"},\"Time\":\"2014-12-23T00:02:43.599Z\",\"Type\":\"Correlation\",\"Correlation\":2.65,\"Hypocenter\":{\"Latitude\":40.3344,\"Longitude\":-121.44,\"Depth\":32.44,\"Time\":\"2014-12-23T00:02:44.039Z\"},\"EventType\":\"earthquake\",\"Magnitude\":2.14,\"SNR\":3.8,\"ZScore\":33.67,\"DetectionThreshold\":1.5,\"ThresholdType\":\"minimum\"}"  // NOLINT
#define CORRELATION3JSON "{\"ID\":\"20682833\",\"Phase\":\"P\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"BHZ\",\"Location\":\"00\",\"Network\":\"US\",\"Station\":\"BOZ\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"TestAuthor\"},\"Time\":\"2014-12-23T00:03:43.599Z\",\"Type\":\"Correlation\",\"Correlation\":2.65,\"Hypocenter\":{\"Latitude\":40.3344,\"Longitude\":-121.44,\"Depth\":32.44,\"Time\":\"2014-12-23T00:03:44.039Z\"},\"EventType\":\"earthquake\",\"Magnitude\":2.14,\"SNR\":3.8,\"ZScore\":33.67,\"DetectionThreshold\":1.5,\"ThresholdType\":\"minimum\"}"  // NOLINT
#define CORRELATION4JSON "{\"ID\":\"20682834\",\"Phase\":\"P\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"EHZ\",\"Location\":\"\",\"Network\":\"MB\",\"Station\":\"LRM\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"TestAuthor\"},\"Time\":\"2014-12-23T00:04:43.599Z\",\"Type\":\"Correlation\",\"Correlation\":2.65,\"Hypocenter\":{\"Latitude\":40.3344,\"Longitude\":-121.44,\"Depth\":32.44,\"Time\":\"2014-12-23T00:04:44.039Z\"},\"EventType\":\"earthquake\",\"Magnitude\":2.14,\"SNR\":3.8,\"ZScore\":33.67,\"DetectionThreshold\":1.5,\"ThresholdType\":\"minimum\"}"  // NOLINT
#define CORRELATION5JSON "{\"ID\":\"20682835\",\"Phase\":\"P\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"EHZ\",\"Location\":\"\",\"Network\":\"MB\",\"Station\":\"HRY\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"TestAuthor\"},\"Time\":\"2014-12-23T00:05:43.599Z\",\"Type\":\"Correlation\",\"Correlation\":2.65,\"Hypocenter\":{\"Latitude\":40.3344,\"Longitude\":-121.44,\"Depth\":32.44,\"Time\":\"2014-12-23T00:05::44.039Z\"},\"EventType\":\"earthquake\",\"Magnitude\":2.14,\"SNR\":3.8,\"ZScore\":33.67,\"DetectionThreshold\":1.5,\"ThresholdType\":\"minimum\"}"  // NOLINT
#define CORRELATION6JSON "{\"ID\":\"20682836\",\"Phase\":\"P\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"BHZ\",\"Location\":\"00\",\"Network\":\"US\",\"Station\":\"BOZ\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"TestAuthor\"},\"Time\":\"2014-12-23T00:00:43.599Z\",\"Type\":\"Correlation\",\"Correlation\":2.65,\"Hypocenter\":{\"Latitude\":40.3344,\"Longitude\":-121.44,\"Depth\":32.44,\"Time\":\"2014-12-23T00:00:44.039Z\"},\"EventType\":\"earthquake\",\"Magnitude\":2.14,\"SNR\":3.8,\"ZScore\":33.67,\"DetectionThreshold\":1.5,\"ThresholdType\":\"minimum\"}"  // NOLINT

#define SCNL "LRM.EHZ.MB"
#define SCNL2 "BOZ.BHZ.US.00"

#define TCORRELATION 3628281643.590000
#define TCORRELATION2 3628281943.590000
#define TCORRELATION3 3628281763.590000

#define MAXNCORRELATION 5

// NOTE: Need to consider testing scavenge, and rouges functions,
// but that would need a much more involved set of real nodes and data,
// not this simple setup.
// Maybe consider performing this test at a higher level?

// test to see if the correlationlist can be constructed
TEST(CorrelationListTest, Construction) {
	glassutil::CLogit::disable();

	// construct a correlationlist
	glasscore::CCorrelationList * testCorrelationList =
			new glasscore::CCorrelationList();

	// assert default values
	ASSERT_EQ(-1, testCorrelationList->getNCorrelationTotal())<<
	"nCorrelationTotal is 0";
	ASSERT_EQ(0, testCorrelationList->getNCorrelation())<< "nCorrelation is 0";

	// lists
	ASSERT_EQ(0, testCorrelationList->getVCorrelationSize())<<
	"vCorrelation.size() is 0";

	// pointers
	ASSERT_EQ(NULL, testCorrelationList->getGlass())<< "pGlass null";
	ASSERT_EQ(NULL, testCorrelationList->getSiteList())<< "pSiteList null";

	// cleanup
	delete (testCorrelationList);
}

// test various correlation operations
TEST(CorrelationListTest, CorrelationOperations) {
	glassutil::CLogit::disable();

	// create json objects from the strings
	std::shared_ptr<json::Object> siteJSON = std::make_shared<json::Object>(
			json::Object(json::Deserialize(std::string(SITEJSON))));
	std::shared_ptr<json::Object> site2JSON = std::make_shared<json::Object>(
			json::Object(json::Deserialize(std::string(SITE2JSON))));
	std::shared_ptr<json::Object> site3JSON = std::make_shared<json::Object>(
			json::Object(json::Deserialize(std::string(SITE3JSON))));

	std::shared_ptr<json::Object> correlationJSON = std::make_shared<
			json::Object>(
			json::Object(json::Deserialize(std::string(CORRELATIONJSON))));
	std::shared_ptr<json::Object> correlation2JSON = std::make_shared<
			json::Object>(
			json::Object(json::Deserialize(std::string(CORRELATION2JSON))));
	std::shared_ptr<json::Object> correlation3JSON = std::make_shared<
			json::Object>(
			json::Object(json::Deserialize(std::string(CORRELATION3JSON))));
	std::shared_ptr<json::Object> correlation4JSON = std::make_shared<
			json::Object>(
			json::Object(json::Deserialize(std::string(CORRELATION4JSON))));
	std::shared_ptr<json::Object> correlation5JSON = std::make_shared<
			json::Object>(
			json::Object(json::Deserialize(std::string(CORRELATION5JSON))));
	std::shared_ptr<json::Object> correlation6JSON = std::make_shared<
			json::Object>(
			json::Object(json::Deserialize(std::string(CORRELATION6JSON))));

	// construct a sitelist
	glasscore::CSiteList * testSiteList = new glasscore::CSiteList();

	// add sites to site list
	testSiteList->addSite(siteJSON);
	testSiteList->addSite(site2JSON);
	testSiteList->addSite(site3JSON);

	// construct a correlationlist
	glasscore::CCorrelationList * testCorrelationList =
			new glasscore::CCorrelationList();
	testCorrelationList->setSiteList(testSiteList);
	testCorrelationList->setNCorrelationMax(MAXNCORRELATION);

	// test indexcorrelation when empty
	ASSERT_EQ(-2, testCorrelationList->indexCorrelation(0))<<
	"test indexcorrelation when empty";

	// test adding correlations by addCorrelation and dispatch
	testCorrelationList->addCorrelation(correlationJSON);
	testCorrelationList->dispatch(correlation3JSON);
	int expectedSize = 2;
	ASSERT_EQ(expectedSize, testCorrelationList->getNCorrelation())<<
	"Added Correlations";

	// test getting a correlation (first correlation, id 1)
	std::shared_ptr<glasscore::CCorrelation> testCorrelation =
			testCorrelationList->getCorrelation(1);
	// check testcorrelation
	ASSERT_TRUE(testCorrelation != NULL)<< "testCorrelation not null";
	// check scnl
	std::string sitescnl = testCorrelation->getSite()->getScnl();
	std::string expectedscnl = std::string(SCNL);
	ASSERT_STREQ(sitescnl.c_str(), expectedscnl.c_str())<<
	"testCorrelation has right scnl";

	// test indexcorrelation
	ASSERT_EQ(-1, testCorrelationList->indexCorrelation(TCORRELATION))<<
	"test indexcorrelation with time before";
	ASSERT_EQ(1, testCorrelationList->indexCorrelation(TCORRELATION2))<<
	"test indexcorrelation with time after";
	ASSERT_EQ(0, testCorrelationList->indexCorrelation(TCORRELATION3))<<
	"test indexcorrelation with time within";

	// add more correlations
	testCorrelationList->addCorrelation(correlation2JSON);
	testCorrelationList->addCorrelation(correlation4JSON);
	testCorrelationList->addCorrelation(correlation5JSON);
	testCorrelationList->addCorrelation(correlation6JSON);

	// check to make sure the size isn't any larger than our max
	expectedSize = MAXNCORRELATION;
	ASSERT_EQ(expectedSize, testCorrelationList->getVCorrelationSize())<<
	"testCorrelationList not larger than max";

	// get first correlation, which is now id 2
	std::shared_ptr<glasscore::CCorrelation> test2Correlation =
			testCorrelationList->getCorrelation(2);

	// check scnl
	sitescnl = test2Correlation->getSite()->getScnl();
	expectedscnl = std::string(SCNL2);
	ASSERT_STREQ(sitescnl.c_str(), expectedscnl.c_str())<<
	"test2Correlation has right scnl";

	// test clearing correlations
	testCorrelationList->clearCorrelations();
	expectedSize = 0;
	ASSERT_EQ(expectedSize, testCorrelationList->getNCorrelation())<<
	"Cleared Correlations";

	// cleanup
	delete (testCorrelationList);
	delete (testSiteList);
}
