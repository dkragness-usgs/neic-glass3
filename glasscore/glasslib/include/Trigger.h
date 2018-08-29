/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef TRIGGER_H
#define TRIGGER_H

#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <mutex>
#include <tuple>
#include "Geo.h"
#include "Link.h"

namespace glasscore {

// forward declarations
class CPick;
class CWeb;

/**
 * \brief glasscore detection trigger class
 *
 * The CTrigger class represents a single detection trigger from the
 * detection graph database.  A CTrigger consists of the location (latitude,
 * longitude, and depth) of the triggering node, the spatial resolution of the
 * triggering node, and a list of picks that made the trigger     // DK REVIEW 20180820  - how is a Trigger different from a Hypo?  I don't see the difference, just seems like it 
                                                                  // is a hypo in a trial state.
 *
 * CTrigger uses smart pointers (std::shared_ptr).
 */
class CTrigger {
 public:
	/**
	 * \brief CTrigger constructor
	 */
	CTrigger();

	/**
	 * \brief CTrigger advanced constructor
	 *
	 * Construct a trigger using the provided data
	 *
	 * \param lat - A double value containing the latitude to use
	 * for this trigger in degrees
	 * \param lon - A double value containing the longitude to use
	 * for this trigger in degrees
	 * \param z - A double value containing the depth to use
	 * for this trigger in kilometers
	 * \param ot - A double value containing the time to use
	 * for this trigger in seconds
	 * \param resolution - A double value containing the inter-node resolution
	 * in kilometer
	 * \param sum - A double value containing the bayesian sum for this trigger
	 * \param count - An integer value containing the site count for this
	 * trigger
	 * \param picks - A std::vector<std::shared_ptr<CPick> containing the picks
	 * for this trigger
	 * \param web - A pointer to the creating node's CWeb
	 */
	CTrigger(double lat, double lon, double z, double ot, double resolution,
				double sum, int count,
				std::vector<std::shared_ptr<CPick>> picks, CWeb *web);

	/**
	 * \brief CTrigger destructor
	 */
	~CTrigger();

	/**
	 * \brief CTrigger initialization function
	 *
	 * Initialize a trigger using the provided data
	 *
	 * \param lat - A double value containing the latitude to use
	 * for this trigger in degrees
	 * \param lon - A double value containing the longitude to use
	 * for this trigger in degrees
	 * \param z - A double value containing the depth to use
	 * for this trigger in kilometers
	 * \param ot - A double value containing the time to use
	 * for this trigger in seconds
	 * \param resolution - A double value containing the inter-node resolution
	 * in kilometer
	 * \param sum - A double value containing the bayesian sum for this trigger
	 * \param count - An integer value containing the site count for this
	 * trigger
	 * \param picks - A std::vector<std::shared_ptr<CPick> containing the picks
	 * for this trigger
	 * \param web - A pointer to the creating node's web
	 */
	bool initialize(double lat, double lon, double z, double ot,
					double resolution, double sum, int count,
					std::vector<std::shared_ptr<CPick>> picks, CWeb *web);  // DK REVIEW 20180828  - Should not be public.  Public access should be via constructor.

	/**
	 * \brief CTrigger clear function
	 */
	void clear();

	/**
	 * \brief Latitude getter
	 * \return the latitude
	 */
	double getLat() const;

	/**
	 * \brief Longitude getter
	 * \return the longitude
	 */
	double getLon() const;

	/**
	 * \brief Depth getter
	 * \return the depth
	 */
	double getZ() const;   // DK REVIEW 20180828  -  could we just tell users to call getGeo()->getZ

	glassutil::CGeo getGeo() const; 

	/**
	 * \brief Origin time getter
	 * \return the origin time
	 */
	double getTOrg() const;

	/**
	 * \brief Resolution getter
	 * \return the resolution
	 */
	double getResolution() const;   // DK REVIEW 20180828  -  point to the detection CNode that this detection came from.

	/**
	 * \brief Sum getter
	 * \return the sum
	 */
	double getSum() const;   // DK REVIEW 20180828  -  eh?

	/**
	 * \brief Count getter
	 * \return the count
	 */
	int getCount() const;

	/**
	 * \brief Web pointer getter
	 * \return the CWeb pointer
	 */
	const CWeb* getWeb() const;

	/**
	 * \brief vPick getter
	 * \return the vPick
	 */
	const std::vector<std::shared_ptr<CPick>> getVPick() const;

 private:
	/**
	 * \brief A double value containing latitude of this triggerin degrees.
	 */
	double dLat;

	/**
	 * \brief A double value containing longitude of this triggerin degrees.
	 */
	double dLon;

	/**
	 * \brief A double value containing the depth of this trigger in kilometers.
	 */
	double dZ;

	/**
	 * \brief A double value with the origin time of this trigger in seconds
	 */
	double tOrg;

	/**
	 * \brief A double value containing the spatial resolution
	 * (between nodes) in kilometers.
	 */
	double dResolution;

	/**
	 * \brief A double value that accumulates the Bayesian
	 * sum of this trigger
	 */
	double dSum;

	/**
	 * \brief A integer value that tallies the number of sites
	 * that are included in this trigger
	 */
	int nCount;   // DK REVIEW 20180828  - is this really a site count or just a copy of vPick size?

	/**
	 * \brief A std::vector of std::shared_ptr's to CPick objects
	 * used in creating this trigger
	 */
	std::vector<std::shared_ptr<CPick>> vPick;

	/**
	 * \brief A pointer to the node CWeb class, used get travel times
	 */
	CWeb *pWeb;   // DK REVIEW 20180828  - look this up via the Node.  No need to cache.

	/**
	 * \brief A recursive mutex to control threading access to this trigger.
	 */
	mutable std::recursive_mutex triggerMutex;
};
}  // namespace glasscore
#endif  // TRIGGER_H
