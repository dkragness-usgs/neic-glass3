#include <logger.h>
#include <convert.h>
#include <json.h>
#include <gtest/gtest.h>
#include <detection-formats.h>

#include <fstream>
#include <string>
#include <memory>


// DK REVIEW 20180730  
// Please add some comments describing test data
// For the first successful data, it is sufficient to say
// Input data that should work.
// If you have multiple sets of working input data, then add some comment about how secondary and tertiary(really wanted to use the word) test data is different than primary.
// If you have input data that doesn't work, please indicate WHY it shouldn't work (i.e. field XX is required but omitted or field YY is incorrectly formatted(not a valid timestamp))
// Comment applies TO ALL TEST DATA in this directory.
// Example comments included for some data bleow.
#define HYPOSTRING  "{\"Bayes\":2.087726,\"Cmd\":\"Hypo\",\"Data\":[{\"Type\":\"Correlation\",\"ID\":\"12GFH48776857\",\"Site\":{\"Station\":\"BMN\",\"Network\":\"LB\",\"Channel\":\"HHZ\",\"Location\":\"01\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"TestAuthor\"},\"Phase\":\"P\",\"Time\":\"2015-12-28T21:32:24.017Z\",\"Correlation\":2.65,\"Hypocenter\":{\"Latitude\":40.3344,\"Longitude\":-121.44,\"Depth\":32.44,\"Time\":\"2015-12-28T21:30:44.039Z\"},\"EventType\":\"earthquake\",\"Magnitude\":2.14,\"SNR\":3.8,\"ZScore\":33.67,\"DetectionThreshold\":1.5,\"ThresholdType\":\"minimum\",\"AssociationInfo\":{\"Phase\":\"P\",\"Distance\":0.442559,\"Azimuth\":0.418479,\"Residual\":-0.025393,\"Sigma\":0.086333}},{\"Amplitude\":{\"Amplitude\":0.000000,\"Period\":0.000000,\"SNR\":3.410000},\"AssociationInfo\":{\"Azimuth\":146.725914,\"Distance\":0.114828,\"Phase\":\"P\",\"Residual\":0.000904,\"Sigma\":1.000000},\"Filter\":[{\"HighPass\":1.050000,\"LowPass\":2.650000}],\"ID\":\"100725\",\"Phase\":\"P\",\"Picker\":\"raypicker\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"BHZ\",\"Location\":\"--\",\"Network\":\"AK\",\"Station\":\"SSN\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"228041013\"},\"Time\":\"2015-08-14T03:35:25.947Z\",\"Type\":\"Pick\"}],\"Depth\":24.717898,\"Gap\":110.554774,\"ID\":\"20311B8E10AF5649BDC52ED099CF173E\",\"IsUpdate\":false,\"Latitude\":61.559315,\"Longitude\":-150.877897,\"MinimumDistance\":0.110850,\"Source\":{\"AgencyID\":\"US\",\"Author\":\"glass\"},\"T\":\"20150814033521.219\",\"Time\":\"2015-08-14T03:35:21.219Z\",\"Type\":\"Hypo\"}" // NOLINT
// DK REVIEW 20180730 - try another string 
// with phase code = "?" and with Hypo error parameters(lat/lon/depth/time).  
// Also include Onset, but no "isUpdate".  Use "Pid" for identification instead of "ID"
#define HYPOSTRING2 "{\"Bayes\":2.087726,\"Cmd\":\"Hypo\",\"Data\":[{\"Type\":\"Correlation\",\"ID\":\"12GFH48776857\",\"Site\":{\"Station\":\"BMN\",\"Network\":\"LB\",\"Channel\":\"HHZ\",\"Location\":\"01\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"TestAuthor\"},\"Phase\":\"?\",\"Time\":\"2015-12-28T21:32:24.017Z\",\"Correlation\":2.65,\"Hypocenter\":{\"Latitude\":40.3344,\"LatitudeError\":0.3344,\"Longitude\":-121.44,\"LongitudeError\":-1.44,\"Depth\":32.44,\"DepthError\":30.0,\"Time\":\"2015-12-28T21:30:44.039Z\",\"TimeError\":3.12},\"EventType\":\"earthquake\",\"Magnitude\":2.14,\"SNR\":3.8,\"ZScore\":33.67,\"DetectionThreshold\":1.5,\"ThresholdType\":\"minimum\",\"AssociationInfo\":{\"Phase\":\"P\",\"Distance\":0.442559,\"Azimuth\":0.418479,\"Residual\":-0.025393,\"Sigma\":0.086333}},{\"Amplitude\":{\"Amplitude\":0.000000,\"Period\":0.000000,\"SNR\":3.410000},\"AssociationInfo\":{\"Azimuth\":146.725914,\"Distance\":0.114828,\"Phase\":\"P\",\"Residual\":0.000904,\"Sigma\":1.000000},\"Filter\":[{\"HighPass\":1.050000,\"LowPass\":2.650000}],\"ID\":\"100725\",\"Phase\":\"?\",\"Picker\":\"raypicker\",\"Polarity\":\"up\",\"Onset\":\"impulsive\",\"Site\":{\"Channel\":\"BHZ\",\"Location\":\"--\",\"Network\":\"AK\",\"Station\":\"SSN\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"228041013\"},\"Time\":\"2015-08-14T03:35:25.947Z\",\"Type\":\"Pick\"}],\"Depth\":24.717898,\"Gap\":110.554774,\"Pid\":\"20311B8E10AF5649BDC52ED099CF173E\",\"Latitude\":61.559315,\"Longitude\":-150.877897,\"MinimumDistance\":0.110850,\"Source\":{\"AgencyID\":\"US\",\"Author\":\"glass\"},\"T\":\"20150814033521.219\",\"Time\":\"2015-08-14T03:35:21.219Z\",\"Type\":\"Hypo\"}" // NOLINT

// DK REVIEW 20180730 - what in this string is incorrect or deficient?  
#define BADHYPOSTRING1 "{\"Bayes\":2.087726,\"Cmd\":\"Hypo\",\"Data\":[{\"Amplitude\":{\"Amplitude\":0.000000,\"Period\":0.000000,\"SNR\":3.410000},\"AssociationInfo\":{\"Azimuth\":146.725914,\"Distance\":0.114828,\"Phase\":\"P\",\"Residual\":0.000904,\"Sigma\":1.000000},\"Filter\":[{\"HighPass\":1.050000,\"LowPass\":2.650000}],\"ID\":\"100725\",\"Phase\":\"P\",\"Picker\":\"raypicker\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"BHZ\",\"Location\":\"--\",\"Network\":\"AK\",\"Station\":\"SSN\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"228041013\"},\"Time\":\"2015-08-14T03:35:25.947Z\",\"Type\":\"Pick\"}],\"Depth\":24.717898,\"Gap\":110.554774,\"ID\":\"20311B8E10AF5649BDC52ED099CF173E\",\"IsUpdate\":false,\"Latitude\":61.559315,\"Longitude\":-150.877897,\"MinimumDistance\":0.110850,\"Source\":{\"AgencyID\":\"US\",\"Author\":\"glass\"},\"T\":\"20150814033521.219\",\"Time\":\"2015-08-14T03:35:21.219Z\"}" // NOLINT
#define BADHYPOSTRING2 "{\"Bayes\":2.087726,\"Cmd\":\"Hypo\",\"Data\":[{\"Amplitude\":{\"Amplitude\":0.000000,\"Period\":0.000000,\"SNR\":3.410000},\"AssociationInfo\":{\"Azimuth\":146.725914,\"Distance\":0.114828,\"Phase\":\"P\",\"Residual\":0.000904,\"Sigma\":1.000000},\"Filter\":[{\"HighPass\":1.050000,\"LowPass\":2.650000}],\"ID\":\"100725\",\"Phase\":\"P\",\"Picker\":\"raypicker\",\"Polarity\":\"up\",\"Site\":{\"Channel\":\"BHZ\",\"Location\":\"--\",\"Network\":\"AK\",\"Station\":\"SSN\"},\"Source\":{\"AgencyID\":\"US\",\"Author\":\"228041013\"},\"Time\":\"2015-08-14T03:35:25.947Z\",\"Type\":\"Pick\"}],\"Depth\":24.717898,\"Gap\":110.554774,\"IsUpdate\":false,\"Latitude\":61.559315,\"Longitude\":-150.877897,\"MinimumDistance\":0.110850,\"Source\":{\"AgencyID\":\"US\",\"Author\":\"glass\"},\"T\":\"20150814033521.219\",\"Time\":\"2015-08-14T03:35:21.219Z\",\"Type\":\"Hypo\"}" // NOLINT

#define CANCELSTRING "{\"ID\":\"20311B8E10AF5649BDC52ED099CF173E\",\"Type\":\"Cancel\"}" // NOLINT
#define CANCELSTRING2 "{\"Pid\":\"20311B8E10AF5649BDC52ED099CF173E\",\"Type\":\"Cancel\"}" // NOLINT
#define BADCANCELSTRING1 "{\"Type\":\"Cancel\"}"
#define BADCANCELSTRING2 "{\"Pid\":\"20311B8E10AF5649BDC52ED099CF173E\"}"

#define TESTPATH "testdata"
#define SITELISTFILE "siteList.txt"
#define BADSITELISTSTRING1 "{\"Type\":\"WhoKnows\"}"
#define BADSITELISTSTRING2 "{\"Cmd\":\"NotRight\"}"

#define SITELOOKUPSTRING "{\"Comp\":\"BHZ\",\"Loc\":"",\"Net\":\"AU\",\"Site\":\"WR10\",\"Type\":\"SiteLookup\"}" // NOLINT
#define BADSITELOOKUPSTRING1 "{\"Comp\":\"BHZ\",\"Loc\":"",\"Net\":\"AU\",\"Site\":\"WR10\"}" // NOLINT
#define BADSITELOOKUPSTRING2 "{\"Comp\":\"BHZ\",\"Loc\":"",\"Net\":\"AU\",\"Type\":\"SiteLookup\"}" // NOLINT
#define BADSITELOOKUPSTRING3 "{\"Comp\":\"BHZ\",\"Loc\":"",\"Site\":\"WR10\",\"Type\":\"SiteLookup\"}" // NOLINT

#define TESTAGENCYID "US"
#define TESTAUTHOR "glasstest"

TEST(Convert, HypoTest) {
	// glass3::util::log_init("converttest", spdlog::level::debug, ".", true);
	std::string agencyid = std::string(TESTAGENCYID);
	std::string author = std::string(TESTAUTHOR);

	// failure cases
	ASSERT_STREQ(
			glass3::parse::hypoToJSONDetection(NULL, agencyid, author).c_str(),
			"");
	ASSERT_STREQ(
			glass3::parse::hypoToJSONDetection(std::make_shared<json::Object>(json::Object(json::Deserialize(BADHYPOSTRING1))), agencyid, author).c_str(),  // NOLINT
			"");
	ASSERT_STREQ(
			glass3::parse::hypoToJSONDetection(std::make_shared<json::Object>(json::Object(json::Deserialize(BADHYPOSTRING2))), agencyid, author).c_str(),  // NOLINT
			"");
	ASSERT_STREQ(
			glass3::parse::hypoToJSONDetection(std::make_shared<json::Object>(json::Object(json::Deserialize(CANCELSTRING))), agencyid, author).c_str(),  // NOLINT
			"");

  // DK REVIEW 20180730 - overkill to throw in an ASSERT_STRNEQ() for parse of your success string?


	// Hypo
	std::string detectionoutput = glass3::parse::hypoToJSONDetection(
			std::make_shared<json::Object>(
					json::Object(json::Deserialize(HYPOSTRING))),
			agencyid, author);
	// build detection object
	rapidjson::Document detectiondocument;
	detectionformats::detection detectionobject(
			detectionformats::FromJSONString(detectionoutput,
												detectiondocument));
	// check valid code
	ASSERT_TRUE(detectionobject.isvalid())<< "Converted detection is valid";

//  DK REVIEW 20180730
// here is a list of fields that are supported by the hypoToJSONDetection() function.
// Should we be verifying all of them, or is that too much work?
// We should at least put something in about, "not testing these fields because it seems like it's not worth the effort"
//  "P"  indicates part of pick Data   "C" indicates part of correlation Data  "PC" is both
// 	Type
// 	ID
// 	Pid
// 	Latitude
// 	Longitude
// 	Time
// 	Depth
// 	IsUpdate
// 	MinimumDistance
// 	Gap
// 	Bayes
// 	Data
// 	 Site
// 	  Station  PC
// 	  Network  PC
// 	  Channel  PC
// 	  Location PC
// 	 Source
// 	  AgencyID PC
// 	  Author   PC
// 	 ID        PC
// 	 AssociationInfo PC
// 	  Phase          PC
// 	  Residual       PC
// 	  Sigma          PC
// 	  Distance       PC
// 	  Azimuth        PC
// 	 Type (Pick, Correlation)
// 	 Time            PC
// 	 Phase           PC
// 	 Picker          P
// 	 Polarity        P
// 	 Onset           P
// 	 Filter          P
// 	  HighPass       P
// 	  LowPass        P
// 	 Amplitude       P
// 	  Amplitude      P
// 	  Period         P
// 	  SNR            P
// 	 Hypocenter       C
// 	  Latitude        C
// 	  Longitude       C
// 	  Depth           C
// 	  Time            C
// 	  LatitudeError   C
// 	  LongitudeError  C
// 	  DepthError      C
// 	  TimeError       C
// 	 Correlation      C
// 	 EventType        C
// 	 Magnitude        C
// 	 SNR              C
// 	 ZScore           C
// 	 DetectionThreshold C
// 	 ThresholdType    C



	// check id
	std::string detectionid = detectionobject.id;
  // DK REVIEW 20180730 - This value should be #defined if not declared (curse you codacy) next to the string you extracted it from.
	std::string expectedid = "20311B8E10AF5649BDC52ED099CF173E";
	ASSERT_STREQ(detectionid.c_str(), expectedid.c_str());

	// check agencyid
	std::string sourceagencyid = detectionobject.source.agencyid;
	std::string expectedagencyid = std::string(TESTAGENCYID);
	ASSERT_STREQ(sourceagencyid.c_str(), expectedagencyid.c_str());

	// check author
	std::string sourceauthor = detectionobject.source.author;
	std::string expectedauthor = std::string(TESTAUTHOR);
	ASSERT_STREQ(sourceauthor.c_str(), expectedauthor.c_str());

	// check latitude
	double latitude = detectionobject.hypocenter.latitude;
  // DK REVIEW 20180730 - This value should be #defined if not declared (curse you codacy) next to the string you extracted it from.
  double expectedlatitude = 61.559315;
	ASSERT_EQ(latitude, expectedlatitude);

	// check longitude
	double longitude = detectionobject.hypocenter.longitude;
  // DK REVIEW 20180730 - This value should be #defined if not declared (curse you codacy) next to the string you extracted it from.
  double expectedlongitude = -150.877897;
	ASSERT_EQ(longitude, expectedlongitude);

	// check detectiontime
	double time = detectionobject.hypocenter.time;
  // DK REVIEW 20180730 - This value should be #defined if not declared (curse you codacy) next to the string you extracted it from.
  double expectedtime = detectionformats::ConvertISO8601ToEpochTime(
			"2015-08-14T03:35:21.219Z");
	ASSERT_NEAR(time, expectedtime, 0.0001);

	// check depth
	double depth = detectionobject.hypocenter.depth;
	double expecteddepth = 24.717898;
	ASSERT_EQ(depth, expecteddepth);

	// check detectiontype
	std::string detectiondetectiontype = detectionobject.detectiontype;
  // DK REVIEW 20180730 - This value should be #defined if not declared (curse you codacy) next to the string you extracted it from.
  std::string expecteddetectiontype = "New";
	ASSERT_STREQ(detectiondetectiontype.c_str(), expecteddetectiontype.c_str());

	// check bayes
	double detectionbayes = detectionobject.bayes;
  // DK REVIEW 20180730 - This value should be #defined if not declared (curse you codacy) next to the string you extracted it from.
  double expectedbayes = 2.087726;
	ASSERT_EQ(detectionbayes, expectedbayes);

	// check minimumdistance
	double detectionminimumdistance = detectionobject.minimumdistance;
  // DK REVIEW 20180730 - This value should be #defined if not declared (curse you codacy) next to the string you extracted it from.
  double expectedminimumdistancee = 0.11085;
	ASSERT_EQ(detectionminimumdistance, expectedminimumdistancee);

	// check gap
	double detectiongap = detectionobject.gap;
  // DK REVIEW 20180730 - This value should be #defined if not declared (curse you codacy) next to the string you extracted it from.
  double expectedgap = 110.554774;
	ASSERT_EQ(detectiongap, expectedgap);

  // DK REVIEW 20180730 
  // looks like you did end to end checks for the most important values.
  // What do you think about doing them for all values derived from the original string?  Overkill?
  // Diminishing returns and not worth the effort?

	// Hypo2
	std::string detectionoutput2 = glass3::parse::hypoToJSONDetection(
			std::make_shared<json::Object>(
					json::Object(json::Deserialize(HYPOSTRING2))),
			agencyid, author);
	// build detection object
	rapidjson::Document detectiondocument2;
	detectionformats::detection detectionobject2(
			detectionformats::FromJSONString(detectionoutput2,
												detectiondocument2));

	// check valid code
	ASSERT_TRUE(detectionobject2.isvalid())<< "Converted detection is valid";
}


TEST(Convert, CancelTest) {
	// glass3::util::log_init("converttest", spdlog::level::debug, ".", true);
	std::string agencyid = std::string(TESTAGENCYID);
	std::string author = std::string(TESTAUTHOR);

	// failure cases
	ASSERT_STREQ(
			glass3::parse::cancelToJSONRetract(NULL, agencyid, author).c_str(),
			"");
	ASSERT_STREQ(
			glass3::parse::cancelToJSONRetract(std::make_shared<json::Object>(json::Object(json::Deserialize(BADCANCELSTRING1))), agencyid, author).c_str(),  // NOLINT
			"");
	ASSERT_STREQ(
			glass3::parse::cancelToJSONRetract(std::make_shared<json::Object>(json::Object(json::Deserialize(BADCANCELSTRING2))), agencyid, author).c_str(),  // NOLINT
			"");
	ASSERT_STREQ(
			glass3::parse::cancelToJSONRetract(std::make_shared<json::Object>(json::Object(json::Deserialize(HYPOSTRING))), agencyid, author).c_str(),  // NOLINT
			"");

	// Cancel
	std::string retractoutput = glass3::parse::cancelToJSONRetract(
			std::make_shared<json::Object>(
					json::Object(json::Deserialize(CANCELSTRING))),
			agencyid, author);
	// build detection object
	rapidjson::Document retractdocument;
	detectionformats::retract retractobject(
			detectionformats::FromJSONString(retractoutput, retractdocument));
	// check valid code
	ASSERT_TRUE(retractobject.isvalid())<< "Converted retraction is valid";

	// check id
	std::string retractid = retractobject.id;
	std::string expectedid = "20311B8E10AF5649BDC52ED099CF173E";
	ASSERT_STREQ(retractid.c_str(), expectedid.c_str());

	// check agencyid
	std::string sourceagencyid = retractobject.source.agencyid;
	std::string expectedagencyid = std::string(TESTAGENCYID);
	ASSERT_STREQ(sourceagencyid.c_str(), expectedagencyid.c_str());

	// check author
	std::string sourceauthor = retractobject.source.author;
	std::string expectedauthor = std::string(TESTAUTHOR);
	ASSERT_STREQ(sourceauthor.c_str(), expectedauthor.c_str());

	// Cancel2  - Only checks to see that a valid detectionformats::retract object is generated
	std::string retractoutput2 = glass3::parse::cancelToJSONRetract(
			std::make_shared<json::Object>(
					json::Object(json::Deserialize(CANCELSTRING2))),
			agencyid, author);
	// build detection object
	rapidjson::Document retractdocument2;
	detectionformats::retract retractobject2(
			detectionformats::FromJSONString(retractoutput2, retractdocument2));
	// check valid code
	ASSERT_TRUE(retractobject2.isvalid())<< "Converted retraction is valid";
}

TEST(Convert, SiteListTest) {
	// glass3::util::log_init("converttest", spdlog::level::debug, ".", true);

	// failure cases
	ASSERT_STREQ(glass3::parse::siteListToStationList(NULL).c_str(), "");
	ASSERT_STREQ(
			glass3::parse::siteListToStationList(std::make_shared<json::Object>(json::Object( json::Deserialize(BADSITELISTSTRING1)))).c_str(),  // NOLINT
			"");
	ASSERT_STREQ(
			glass3::parse::siteListToStationList(std::make_shared<json::Object>(json::Object( json::Deserialize(BADSITELISTSTRING2)))).c_str(),  // NOLINT
			"");

	std::string sitelistfile = "./" + std::string(TESTPATH) + "/"
			+ std::string(SITELISTFILE);


	// open the file
	std::ifstream inFile;
	inFile.open(sitelistfile, std::ios::in);

	// get the next line
	std::string sitelist;
	std::getline(inFile, sitelist);

	inFile.close();

	std::shared_ptr<json::Object> sitelistobject =
			std::make_shared<json::Object>(
					json::Object(json::Deserialize(sitelist)));
	int numsites = (*sitelistobject)["SiteList"].ToArray().size();

	std::string stationlist = glass3::parse::siteListToStationList(
			sitelistobject);

	std::shared_ptr<json::Object> stationlistobject = std::make_shared<
			json::Object>(json::Object(json::Deserialize(stationlist)));

	ASSERT_TRUE(stationlistobject->HasKey("Type"));
	ASSERT_STREQ((*stationlistobject)["Type"].ToString().c_str(),
					"StationInfoList");

	ASSERT_TRUE(stationlistobject->HasKey("StationList"));
	ASSERT_EQ((*stationlistobject)["StationList"].ToArray().size(), numsites);

  // DK REVIEW 20180730
  // The following fields are supported by the siteListToStationList() function:
  // Cmd = SiteList
  //   SiteList
  //   Sta
  //   Comp
  //   Net
  //   Loc
  //   Lat
  //   Lon
  //   Z
  //   Qual
  //   Use
  //   UseForTele
  //
  //   Should we be testing them all?  Please answer in complete sentences and don't forget to use
  //   a #2 pencil.
}

TEST(Convert, SiteLookupTest) {
	// glass3::util::log_init("converttest", spdlog::level::debug, ".", true);
	std::string agencyid = std::string(TESTAGENCYID);
	std::string author = std::string(TESTAUTHOR);

	// failure cases
	ASSERT_STREQ(
			glass3::parse::siteLookupToStationInfoRequest(NULL, agencyid, author).c_str(),
			"");
	ASSERT_STREQ(
			glass3::parse::siteLookupToStationInfoRequest(std::make_shared<json::Object>( json::Object(json::Deserialize(BADSITELOOKUPSTRING1))), agencyid, author).c_str(),  // NOLINT
			"");
	ASSERT_STREQ(
			glass3::parse::siteLookupToStationInfoRequest(std::make_shared<json::Object>( json::Object(json::Deserialize(BADSITELOOKUPSTRING2))), agencyid, author).c_str(),  // NOLINT
			"");
	ASSERT_STREQ(
			glass3::parse::siteLookupToStationInfoRequest(std::make_shared<json::Object>( json::Object(json::Deserialize(BADSITELOOKUPSTRING3))), agencyid, author).c_str(),  // NOLINT
			"");
	ASSERT_STREQ(
			glass3::parse::siteLookupToStationInfoRequest(std::make_shared<json::Object>( json::Object( json::Deserialize(CANCELSTRING))), agencyid, author).c_str(),  // NOLINT
			"");

	// sitelookup
	std::string stationinforequestoutput =
			glass3::parse::siteLookupToStationInfoRequest(
					std::make_shared<json::Object>(
							json::Object(json::Deserialize(SITELOOKUPSTRING))),
					agencyid, author);
	// build detection object
	rapidjson::Document stationdocument;
	detectionformats::stationInfoRequest stationrequestobject(
			detectionformats::FromJSONString(stationinforequestoutput,
												stationdocument));
	// check valid code
	ASSERT_TRUE(stationrequestobject.isvalid())<< "Converted station info "
	"request is valid";

	// check station
	std::string sitestation = stationrequestobject.site.station;
	std::string expectedstation = "WR10";
	ASSERT_STREQ(sitestation.c_str(), expectedstation.c_str());

	// check channel
	std::string sitechannel = stationrequestobject.site.channel;
	std::string expectedchannel = "BHZ";
	ASSERT_STREQ(sitechannel.c_str(), expectedchannel.c_str());

	// check network
	std::string sitenetwork = stationrequestobject.site.network;
	std::string expectednetwork = "AU";
	ASSERT_STREQ(sitenetwork.c_str(), expectednetwork.c_str());

	// check location
	std::string sitelocation = stationrequestobject.site.location;
	std::string expectedlocation = "";
	ASSERT_STREQ(sitelocation.c_str(), expectedlocation.c_str());

	// check agencyid
	std::string sourceagencyid = stationrequestobject.source.agencyid;
	std::string expectedagencyid = std::string(TESTAGENCYID);
	ASSERT_STREQ(sourceagencyid.c_str(), expectedagencyid.c_str());

	// check author
	std::string sourceauthor = stationrequestobject.source.author;
	std::string expectedauthor = std::string(TESTAUTHOR);
	ASSERT_STREQ(sourceauthor.c_str(), expectedauthor.c_str());

  // DK REVIEW 20180730
  // should we be doing one test with a non-NULL loc code?
}
