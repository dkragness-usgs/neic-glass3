/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef NODE_H
#define NODE_H

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
class CSite;
class CWeb;
class CTrigger;

/**
 * \brief glasscore detection node class
 *
 * The CNode class represents a single detection node in the
 * detection graph database.  A CNode is linked to one or more sites
 * (seismic stations), as part of the overall detection graph database.
 * A detection node consists of the location (latitude, longitude, and depth)
 * of the detection node, the spatial resolution of the node, a list of the
 * links to the sites, and a list of picks currently being nucleated.
 *
 * CNode uses the nucleate function to evaluate the likelihood of a hypocenter
 * existing centered on the node based on all picks each site linked
 * to the node
 *
 * CNode uses smart pointers (std::shared_ptr).
 */
class CNode {
 public:
	/**
	 * \brief CNode constructor
	 */
	CNode();

	/**
	 * \brief CNode advanced constructor
	 *
	 * Construct a node using the provided latitude, longitude,
	 * and depth.
	 *
	 * \param name - A string containing the name of the parent web
	 * for this node
	 * \param lat - A double value containing the latitude to use
	 * for this node in degrees
	 * \param lon - A double value containing the longitude to use
	 * for this node in degrees
	 * \param z - A double value containing the depth to use
	 * for this node in kilometers
	 * \param resolution - A double value containing the inter-node resolution
	 * in kilometers
	 * \param nodeID - A std::string containing the node id.
	 */
	CNode(std::string name, double lat, double lon, double z, double resolution,
			std::string nodeID);  // DK REVIEW 20180822 - seems like a Web pointer should be required here.  Resolution and any other nucleation criteria could
                            // be gotten therefrom.  Ditch the GUID nodeID, and build it from Web.lat.lon.z

	/**
	 * \brief CNode destructor
	 */
	~CNode();

	/**
	 * \brief CNode clear function
	 */
	void clear();

	/**
	 * \brief Delink all sites to/from this node.
	 */
	void clearSiteLinks();

	/**
	 * \brief CNode initialization funcion
	 *
	 * Initialize a node using the provided latitude, longitude,
	 * and depth.
	 *
	 * \param name - A string containing the name of the parent web
	 * for this node
	 * \param lat - A double value containing the latitude to use
	 * for this node in degrees
	 * \param lon - A double value containing the longitude to use
	 * for this node in degrees
	 * \param z - A double value containing the depth to use
	 * for this node in kilometers
	 * \param resolution - A double value containing the inter-node resolution
	 * in kilometers
	 * \param nodeID - A std::string containing the node id.
	 */
	bool initialize(std::string name, double lat, double lon, double z,
					double resolution, std::string nodeID);

	/**
	 * \brief CNode node-site and site-node linker
	 *
	 * Add a link to/from this node to the provided site
	 * using the provided travel time
	 *
	 * \param travelTime1 - A double value containing the first travel time to
	 * use for the link
	 * \param travelTime2 - A double value containing the optional second travel
	 * time to use for the link, defaults to -1 (no travel time)
	 * \param site - A shared_ptr<CSite> to the site to link
	 * \param node - A shared_ptr<CNode> to the node to link (should be itself)
	 * \return - Returns true if successful, false otherwise
	 */
	bool linkSite(std::shared_ptr<CSite> site, std::shared_ptr<CNode> node,   // DK REVIEW 20180822 - why are we passing a node pointer here?  a node only needs to deal with itself.
					double travelTime1, double travelTime2 = -1);

	/**
	 * \brief CNode node-site and site-node unlinker
	 *
	 * Remove the given site to/from this node
	 *
	 * \param site - A std::shared_ptr<CSite> to the site to remove
	 * \return - Returns true if successful, false otherwise
	 */
	bool unlinkSite(std::shared_ptr<CSite> site);

	/**
	 * \brief CNode unlink last site from node
	 *
	 * Remove the last site to/from this node
	 *
	 * \return - Returns true if successful, false otherwise
	 */
	bool unlinkLastSite();  // DK REVIEW 20180822 -  Maybe change to removeWorstSite() or removeFurthestSite() or removeLeastAdvantageousSite()

	/**
	 * \brief CNode Nucleation function
	 *
	 * Given an origin time, compute a number representing the stacked PDF
	 * of a hypocenter centered on this node, by computing the PDF
	 * of each pick at each site linked to this node and totaling (stacking)
	 * them up.
	 *
	 * \param tOrigin - A double value containing the proposed origin time
	 * to use in julian seconds
	 * \return Returns true if the node nucleated an event, false otherwise
	 */
	std::shared_ptr<CTrigger> nucleate(double tOrigin);  // DK REVIEW 20180822 - to me it would make more sense to call nucleate(Pick)
                                                       // then you can avoid re-using the same pick, handle (possibly both traveltimes)
                                                       // at once, and return just the best one.  Also makes debug logging more informative.

	/**
	 * \brief CNode significance function
	 *
	 * Given an observed time and a like to a site, compute the best
	 * significance value from the traveltime(s) contained in the link
	 *
	 * \param tObservedTT - A double value containing the observed travel time
	 * \param link - A SiteLink containing the travel times to use
	 * \return Returns best significance if there is at least one valid travel
	 * time, -1.0 otherwise
	 */
	double getBestSig(double tObservedTT, SiteLink link);

	/**
	 * \brief CNode get site function
	 *
	 * Given a site id, get the site if it is used by the node
	 *
	 * \param sScnl - A string containing the id of the site to get
	 * \return Returns a shared pointer to the CSite object if found, null
	 * otherwise
	 */
	std::shared_ptr<CSite> getSite(std::string sScnl);

	/**
	 * \brief CNode get last site function
	 *
	 * Get the last site linked to the node
	 *
	 * \return Returns a shared pointer to the last CSite object if found, null
	 * otherwise
	 */
	std::shared_ptr<CSite> getLastSite();

	/**
	 * \brief CNode site link sort function
	 *
	 * Sort the list of sites linked to this node
	 */
	void sortSiteLinks();

	/**
	 * \brief Sites string getter
	 * \return the sites string
	 */
	std::string getSitesString();

	/**
	 * \brief Site links count getter
	 * \return the site links count
	 */
	int getSiteLinksCount() const;

	/**
	 * \brief Enabled flag getter
	 * \return the enabled flag
	 */
	bool getEnabled() const;

	/**
	 * \brief Enabled flag setter
	 * \param enabled - the enabled flag
	 */
	void setEnabled(bool enabled);

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
	 * \return the Depth
	 */
	double getZ() const;

	/**
	 * \brief CGeo getter
	 * \return the node location as a glassutil::CGeo object.
	 */
	glassutil::CGeo getGeo() const;

	/**
	 * \brief Resolution getter
	 * \return the Resolution
	 */
	double getResolution() const;

	/**
	 * \brief CWeb pointer getter
	 * \return the CWeb pointer
	 */
	CWeb* getWeb() const;

	/**
	 * \brief CWeb pointer setter
	 * \param web - the CWeb pointer
	 */
	void setWeb(CWeb* web);

	/**
	 * \brief Name getter
	 * \return the name
	 */
	const std::string& getName() const;

	/**
	 * \brief Pid getter
	 * \return the pid
	 */
	const std::string& getPid() const;

 private:
	/**
	 * \brief A pointer to the parent CWeb class, used get configuration,
	 * values, perform significance calculations, and debug flags
	 */
	CWeb *pWeb;

	/**
	 * \brief Name of the web subnet that this node is associated with.
	 * This attribute is used for web level tracking and dynamics such
	 * as removing a named subnet with the 'RemoveWeb' command.
	 */
	std::string sName;

	/**
	 * \brief A std::string containing the string unique id of this node
	 */
	std::string sPid;

	/**
	 * \brief A double value containing this node's latitude in degrees.
	 */
	double dLat;

	/**
	 * \brief A double value containing this node's longitude in degrees.
	 */
	double dLon;

	/**
	 * \brief A double value containing this node's depth in kilometers.
	 */
	double dZ;

	/**
	 * \brief A double value containing this node's spatial resolution
	 * (to other nodes) in kilometers.
	 */
	double dResolution;

	/**
	 * \brief A boolean flag indicating whether this node is enabled for
	 * nucleation
	 */
	bool bEnabled;

	/**
	 * \brief A std::vector of tuples linking node to site
	 * {shared site pointer, travel time 1, travel time 2}  // DK REVIEW - in CWeb there is an assumption made about Site's in this list being
                                                          // in distance order, such that the last one is assumed to be the worst. 
                                                          // but there is no such label/claim made here.  To be clear, either list
                                                          // the sort order here, or indicate that sites are in random order.
	 */
	std::vector<SiteLink> vSite;

	/**
	 * \brief A mutex to control threading access to vSite.
	 */
	mutable std::mutex vSiteMutex;

	/**
	 * \brief A recursive_mutex to control threading access to CNode.
	 * NOTE: recursive mutexes are frowned upon, so maybe redesign around it
	 * see: http://www.codingstandard.com/rule/18-3-3-do-not-use-stdrecursive_mutex/
	 * However a recursive_mutex allows us to maintain the original class
	 * design as delivered by the contractor.
	 */
	mutable std::recursive_mutex nodeMutex;
};
}  // namespace glasscore
#endif  // NODE_H
