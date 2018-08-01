#include <gpickparser.h>
#include <json.h>
#include <logger.h>
#include <stringutil.h>
#include <timeutil.h>
#include <detection-formats.h>
#include <string>
#include <vector>
#include <memory>

namespace glass3 {
namespace parse {
// ------------------------------------------------------------------GPickParser
GPickParser::GPickParser(const std::string &newAgencyID,
							const std::string &newAuthor)
		: glass3::parse::Parser::Parser(newAgencyID, newAuthor) {
}

// -----------------------------------------------------------------~GPickParser
GPickParser::~GPickParser() {
}


// DK REVIEW 20180801  - parse() should be responsible for taking an input message
// and turning it into the JSON string representation of an object from
// the detectionformats namespace.
// the string should be self-determining, in that the JSON string should
// contain information that identifies which class it's an object of(it's type),
// such that detectionformats::validate() or similar can be called on the string
// to ensure it can generate a valid detectionformats object.
// Seems reasonable taht a detectionformats::validate() function could exist, that
// is aware of the supported detectionformats datatypes and can deal with the
// detectionformats json library.  This doesn't seem conceptually to be too much
// of a stretch from detectionformats::ToJSONString() and detectionformats::FromJSONString()
//
// ------------------------------------------------------------------------parse
std::shared_ptr<json::Object> GPickParser::parse(const std::string &input) {
	// make sure we got something
	if (input.length() == 0)
		return (NULL);

	glass3::util::log("trace", "gpickparser::parse: Input String: " + input + ".");

	// gpick format
	// 228041013 22637620 1 GLI BHZ AK -- 20150302235859.307 P -1.0000 U  ? r 1.050 2.650 0.0 0.000000 5.00 0.000000 0.000000  // NOLINT
	//
	// index 0 is the author/logo, use
	// index 1 is the pick id, need
	// index 2 is the gpick version, ignore
	// indexes 3-6 is the SCNL, need
	// index 7 is the arrival time, need
	// index 8 is the phase, need
	// index 9 is the error window half width, need
	// index 10 is the polarity, need
	// index 11 is the onset, need
	// index 12 is the picker type, need
	// index 13 is the high pass frequency, need
	// index 14 is the low pass frequency, need
	// index 15 is the back azimuth, ignore
	// index 16 is the slowness, ignore
	// index 17 is the SNR, need
	// index 18 is the amplitude, need
	// index 19 is the period, need

	try {
		// split the gpick, the gpick is space delimited
		std::vector<std::string> splitgpick = glass3::util::split(input, ' ');

		// make sure we split the response into at
		// least as many elements as we need
		if (splitgpick.size() < 20) {
			glass3::util::log(
					"error",
					"gpickparser::parse: Provided input did not split into at "
							"least the 20 elements needed for a global pick "
							"(split into " + std::to_string(splitgpick.size())
							+ ") , returning.");
			return (NULL);
		}

		// create the new pick
		// build the json pick object
		detectionformats::pick newpick;
		newpick.id = splitgpick[1];

		// build the site object
    // DK 20180730 REVIEW
    // nitpicky, because of the size of the function and number of functions that reference it...
    // be better to use constants with meaningful names instead of magic numbers here:
    // #define GPICK_FIELD_INDEX_STATION_CODE 3
    // newpick.site.station = splitgpick[GPICK_FIELD_INDEX_STATION_CODE];

		newpick.site.station = splitgpick[3];
		newpick.site.channel = splitgpick[4];
		newpick.site.network = splitgpick[5];
		newpick.site.location = splitgpick[6];

		// convert the global pick "DateTime" into epoch time
		newpick.time = glass3::util::convertDateTimeToEpochTime(splitgpick[7]);

		// build the source object
		// need to think more about this one
		// as far as ew logos are concerned....
		newpick.source.agencyid = getAgencyId();
		newpick.source.author = splitgpick[0];

		// phase
		newpick.phase = splitgpick[8];

		// polarity
		if (splitgpick[10] == "U") {
			newpick.polarity =
					detectionformats::polarityvalues[detectionformats::polarityindex::up];  // NOLINT
		} else if (splitgpick[10] == "D") {
			newpick.polarity =
					detectionformats::polarityvalues[detectionformats::polarityindex::down];  // NOLINT
		}

		// onset
		if (splitgpick[11] == "i") {
			newpick.onset =
					detectionformats::onsetvalues[detectionformats::onsetindex::impulsive];  // NOLINT
		} else if (splitgpick[11] == "e") {
			newpick.onset =
					detectionformats::onsetvalues[detectionformats::onsetindex::emergent];  // NOLINT
		} else if (splitgpick[11] == "q") {
			newpick.onset =
					detectionformats::onsetvalues[detectionformats::onsetindex::questionable];  // NOLINT
		}

		// onset
		if (splitgpick[12] == "m") {
			newpick.picker =
					detectionformats::pickervalues[detectionformats::pickerindex::manual];  // NOLINT
		} else if (splitgpick[12] == "r") {
			newpick.picker =
					detectionformats::pickervalues[detectionformats::pickerindex::raypicker];  // NOLINT
		} else if (splitgpick[12] == "l") {
			newpick.picker =
					detectionformats::pickervalues[detectionformats::pickerindex::filterpicker];  // NOLINT
		} else if (splitgpick[12] == "e") {
			newpick.picker =
					detectionformats::pickervalues[detectionformats::pickerindex::earthworm];  // NOLINT
		} else if (splitgpick[12] == "U") {
			newpick.picker =
					detectionformats::pickervalues[detectionformats::pickerindex::other];  // NOLINT
		}

		// convert Filter values to a json object
		double HighPass = -1.0;
		double LowPass = -1.0;
		try {
			HighPass = std::stod(splitgpick[13]);
			LowPass = std::stod(splitgpick[14]);
		} catch (const std::exception &) {
			glass3::util::log(
					"warning",
					"gpickparser::parse: Problem converting optional filter "
					"values to doubles.");
		}

		// make sure we got some sort of valid numbers
		if ((HighPass != -1.0) && (LowPass != -1.0)) {
			detectionformats::filter filterobject;
			filterobject.highpass = HighPass;
			filterobject.lowpass = LowPass;
			newpick.filterdata.push_back(filterobject);
		}

		// convert amplitude values to a json object
		double Amplitude = -1.0;
		double Period = -1.0;
		double SNR = -1.0;
		try {
			Amplitude = std::stod(splitgpick[18]);
			Period = std::stod(splitgpick[19]);
			SNR = std::stod(splitgpick[17]);
		} catch (const std::exception &) {
			glass3::util::log(
					"warning",
					"gpickparser::parse: Problem converting optional amplitude "
					"values to doubles.");
		}

		// make sure we got some sort of valid numbers
		if ((Amplitude != -1.0) && (Period != -1.0) && (SNR != -1.0)) {
			// create amplitude object
			newpick.amplitude.ampvalue = Amplitude;
			newpick.amplitude.period = Period;
			newpick.amplitude.snr = SNR;
		}

		// convert to our json implementation.
		rapidjson::Document pickdocument;
		std::string pickstring = detectionformats::ToJSONString(
				newpick.tojson(pickdocument, pickdocument.GetAllocator()));
		json::Value deserializedJSON = json::Deserialize(pickstring);

		// make sure we got valid json
		if (deserializedJSON.GetType() != json::ValueType::NULLVal) {
			std::shared_ptr<json::Object> newjsonpick = std::make_shared<
					json::Object>(json::Object(deserializedJSON.ToObject()));

			glass3::util::log(
					"trace",
					"gpickparser::parse: Output JSON: "
							+ json::Serialize(*newjsonpick) + ".");

			return (newjsonpick);
		}
	} catch (const std::exception &e) {
		glass3::util::log(
				"warning",
				"gpickparser::parse: Problem parsing global pick: "
						+ std::string(e.what()));
	}

	return (NULL);
}

// ---------------------------------------------------------------------validate
bool GPickParser::validate(std::shared_ptr<json::Object> &input) {
	// nullcheck
	if (input == NULL) {
		return (false);
	}

	// convert to detectionformats::pick
	std::string pickstring = json::Serialize(*input);
	rapidjson::Document pickdocument;
  // DK20180730 REVIEW
  // seems like this should be mentioned in function definition (relies on use of a specific external library(detectionformats::pick) to perform validation)
	detectionformats::pick pickobject(
			detectionformats::FromJSONString(pickstring, pickdocument));

	// let detection formats validate
	return (pickobject.isvalid());
}
}  // namespace parse
}  // namespace glass3
