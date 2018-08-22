/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef CORRELATION_H
#define CORRELATION_H

#include <json.h>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace glasscore {

// forward declarations
class CSite;
class CSiteList;
class CHypo;

/**
 * \brief glasscore correlation class
 *
 * The CCorrelation class is the class that encapsulates everything necessary
 * to represent a waveform arrival correlation, including arrival time, phase id,
 * and an unique identifier.  The CCorrelation class is also a node in the
 * detection graph database.
 *
 * CCorrelation contains functions to support creation of a new event based
 * on the correlation.
 *
 * CCorrelation maintains a graph database link between it and the the site (station)
 * the correlation was made at.
 *
 * CCorrelation also maintains a vector of CHypo objects represent the graph database
 * links between  this correlation and various hypocenters.  A single correlation may be
 * linked to multiple hypocenters
 *
 * CCorrelation uses smart pointers (std::shared_ptr).
 */

// DK 20180822  REVIEW   I would recommend not having a separate independent CCorrelation class.  Either 
// derive CCorrelation from Pick (it's a special pick that comes with it's own hypocenter), or integrate
// the corelation stuff into Pick, making it essentially optional fields.
// I think derivation would be the more C++-ish way to do it, adding a Private-Hypo and anything else
// that's unique to a Correlation.

class CCorrelation {
 public:
	/**
	 * \brief CCorrelation constructor
	 */
	CCorrelation();

	/**
	 * \brief CCorrelation alternate constructor // DK REVIEW 20180820 - alternate constructor that does what or is special/different how?
	 *
	 * Constructs a CCorellation using the provided values
	 *
	 * \param correlationSite - A shared pointer to a CSite object that the
	 * correlation wasmade at
	 * \param correlationTime - A double containing the correlation arrival time
	 * \param correlationId - An integer containing the glass correlation id
	 * (index) to use.
	 * \param correlationIdString - A std::string containing the external
	 * correlation id.
	 * \param phase - A std::string containing the phase name
	 * \param orgTime - A double containing the julian time in seconds to use
	 * \param orgLat - A double containing the geocentric latitude in degrees to
	 * use
	 * \param orgLon - A double containing the geocentric longitude in degrees
	 * to use
	 * \param orgZ - A double containing the geocentric depth in kilometers to
	 * use
	 * \param corrVal - A double containing the correlation value
	 */
	CCorrelation(std::shared_ptr<CSite> correlationSite, double correlationTime,
					int correlationId, std::string correlationIdString,
					std::string phase, double orgTime, double orgLat,
					double orgLon, double orgZ, double corrVal);

	/**
	 * \brief CCorrelation alternate constructor // DK REVIEW 20180820 - alternate constructor that does what or is special/different how?
	 *
	 * Constructs a CCorrelation class from the provided json object and id, using
	 * a CGlass pointer to convert times and lookup stations.   // DK REVIEW 20180820 -  is this supposed to say CSiteList instead of CGlass?
	 *
	 * \param correlation - A pointer to a json::Object to construct the correlation from
	 * \param correlationId - An integer containing the correlation id to use.
	 * \param pSiteList - A pointer to the CSiteList class  // DK REVIEW 20180820 - what is "the CSiteList class" and what's it used for here?
	 */
	CCorrelation(std::shared_ptr<json::Object> correlation, int correlationId,
					CSiteList *pSiteList);

	/**
	 * \brief CCorrelation destructor
	 */
	~CCorrelation();

	/**
	 * \brief CCorrelation clear function
	 */
	void clear();

	/**
	 * \brief CCorrelation initialization function
	 *
	 * Initializes correlation class to provided values.
	 *
	 * \param correlationSite - A shared pointer to a CSite object that the
	 * correlation wasmade at
	 * \param correlationTime - A double containing the correlation arrival time
	 * \param correlationId - An integer containing the glass correlation id
	 * (index) to use.
	 * \param correlationIdString - A std::string containing the external
	 * correlation id.
	 * \param phase - A std::string containing the phase name
	 * \param orgTime - A double containing the julian time in seconds to use
	 * \param orgLat - A double containing the geocentric latitude in degrees to
	 * use
	 * \param orgLon - A double containing the geocentric longitude in degrees
	 * to use
	 * \param orgZ - A double containing the geocentric depth in kilometers to
	 * use
	 * \param corrVal - A double containing the correlation value
	 * \return Returns true if successful, false otherwise.
	 */
	bool initialize(std::shared_ptr<CSite> correlationSite,
					double correlationTime, int correlationId,
					std::string correlationIdString, std::string phase,
					double orgTime, double orgLat, double orgLon, double orgZ,
					double corrVal);  // DK REVIEW 20180820 - Where's the mag value from the correlation, or something as a size or propogation distance proxy?

	/**
	 * \brief Add hypo reference to this correlation
	 *
	 * Adds a shared_ptr reference to the given hypo to this correlation,
	 * representing a graph database link between this correlation and the
	 * hypocenters.
	 *
	 * Note that this correlation may or may not also be linked
	 * to other hypocenters
	 *
	 * \param hyp - A std::shared_ptr to an object containing the hypocenter
	 * to link.
	 * \param ass - A std::string containing a note about the association reason
	 * \param force - A boolean flag indicating whether to force the association,
	 * defaults to false.
	 */
	void addHypo(std::shared_ptr<CHypo> hyp, std::string ass = "", bool force =
							false);
  // DK REVIEW 20180820 - "ass"?  

	/**
	 * \brief Remove hypo specific reference to this correlation
	 *
	 * Remove a shared_ptr reference to the given hypo from this correlation,
	 * breaking the graph database link between this correlation and the
	 * hypocenter.
	 *
   // DK REVIEW 20180820 -  a correlation can have more than one hypo? <insert scooby doo noise here....>
	 * Note that this correlation may or may not be still linked
	 * to other hypocenters
	 *
	 * \param hyp - A std::shared_ptr to an object containing the hypocenter
	 * to unlink.
	 */
	void remHypo(std::shared_ptr<CHypo> hyp);

	/**
	 * \brief Remove hypo specific reference to this correlation
	 *
	 * Remove a shared_ptr reference to the given hypo id from this correlation,
	 * breaking the graph database link between this correlation and the
	 * hypocenter.
	 *
	 * Note that this correlation may or may not be still linked
	 * to other hypocenters
	 *
	 * \param pid - A std::string identifying the the hypocenter to unlink.
	 */
	void remHypo(std::string pid);   // DK REVIEW 20180820 -  Could we have a better name.  The 3 extra letters in removeHypo() is not gonna kill us.
                                   // otherwise I might confuse this function with a batch file remark or a band from the 80's.
                                   //  I've got my spine, I've got my ORANGE CRUSH!

	/**
	 * \brief Remove hypo reference to this correlation
	 *
	 * Remove any shared_ptr reference from this correlation, breaking the graph
	 * database link between this correlation and any hypocenter.
	 */
	void clearAllHypos();   // DK REVIEW 20180820 -  Name change might be clearer?

	/**
	 * \brief Correlation value getter
	 * \return the correlation value
	 */
	double getCorrelation() const;

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
	double getZ() const;

	/**
	 * \brief Origin time getter
	 * \return the origin time
	 */
	double getTOrg() const;

	/**
	 * \brief Correlation id getter
	 * \return the correlation id
	 */
	int getIdCorrelation() const;

	/**
	 * \brief Json correlation getter
	 * \return the json correlation
	 */
	const std::shared_ptr<json::Object>& getJCorrelation() const;

	/**
	 * \brief Hypo getter
	 * \return the hypo
	 */
	const std::shared_ptr<CHypo> getHypo() const;

	/**
	 * \brief Pid getter
	 * \return the pid
	 */
	const std::string getHypoPid() const;

	/**
	 * \brief Site getter
	 * \return the site
	 */
	const std::shared_ptr<CSite>& getSite() const;

	/**
	 * \brief Association string getter
	 * \return the association string
	 */
	const std::string& getAssoc() const;

  // DK REVIEW 20180820 -  While better than grabAssoc(), still not good.  Fix, Please.

	/**
	 * \brief Association string setter
	 * \param ass - the association string
	 */
	void setAssoc(std::string ass);
  // DK REVIEW 20180820 - ditto.

	/**
	 * \brief Phase getter
	 * \return the phase
	 */
	const std::string& getPhs() const;

	/**
	 * \brief Pid getter
	 * \return the pid
	 */
	const std::string& getPid() const;

	/**
	 * \brief Correlation time getter
	 * \return the correlation time
	 */
	double getTCorrelation() const;

  // DK REVIEW 20180820  what is TCorrelation?  arrival time at the site?  beginning of correlation-window time at the site?
	/**
	 * \brief Creation time getter
	 * \return the creation time
	 */
	double getTGlassCreate() const;
  // DK REVIEW 20180820  what is "Creation time"?  Time this correlation was first inserted into Glass?

 private:
	/**
	 * \brief A std::shared_ptr to a CSite object
	 * representing the link between this correlation and the site it was
	 * correlated at
	 */
	std::shared_ptr<CSite> pSite;

	/**
	 * \brief A std::weak_ptr to a CHypo object
	 * representing the links between this correlation and associated hypocenter
	 */
	std::weak_ptr<CHypo> wpHypo;  // DK REVIEW 20180820  comments and func declarations above make it seem like
                                // A correlation can be assoc'd with more than one Hypo, but this definition
                                // makes it look like a binary relationship.

	/**
	 * \brief A std::string containing a character representing the action
	 * that caused this correlation to be associated
	 */
	std::string sAssoc;

	/**
	 * \brief A std::string containing the phase name of this correlation
	 */
	std::string sPhs;

	/**
	 * \brief A std::string containing the string unique id of this correlation
	 */
	std::string sPid;

	/**
	 * \brief An integer value containing the numeric id of the correlation
	 */
	int idCorrelation;

	/**
	 * \brief A double value containing the arrival time of the correlation
	 */
	double tCorrelation;

	/**
	 * \brief A double value containing this correlation's origin time in julian
	 * seconds
	 */
	double tOrg;

	/**
	 * \brief A double value containing this correlation's latitude in degrees
	 */
	double dLat;

	/**
	 * \brief A double value containing this correlation's longitude in degrees
	 */
	double dLon;

	/**
	 * \brief A double value containing this correlation's depth
	 * in kilometers.
	 */
	double dZ;

	/**
	 * \brief A double value containing this correlation's correlation value
	 */
	double dCorrelation;

	/**
	 * \brief A double value containing the creation time of the correlation in
	 * glass
	 */
	double tGlassCreate;

	/**
	 * \brief A std::shared_ptr to a json object
	 * representing the original correlation input, used in accessing
	 * information not relevant to glass that are needed for generating outputs.
	 */
	std::shared_ptr<json::Object> jCorrelation;

	/**
	 * \brief A recursive_mutex to control threading access to CCorrelation.
	 * NOTE: recursive mutexes are frowned upon, so maybe redesign around it
	 * see: http://www.codingstandard.com/rule/18-3-3-do-not-use-stdrecursive_mutex/
	 * However a recursive_mutex allows us to maintain the original class
	 * design as delivered by the contractor.
	 */
	mutable std::recursive_mutex correlationMutex;  // DK REVIEW 20180820 - so all the get*() functions are
                                                  // declared const, so they can operate on a const object
                                                  // and/or to indicate they are not changing anything in the
                                                  // object, but they need to be able to lock they object so
                                                  // that no one else can change it, so the mutex is defined
                                                  // as mutable beceause it needs to be changed during locking/unlocking.
                                                  // To boot, we are using a recursive_mutex here, so that we don't have to
                                                  // worry about hangups when we call a get method (which tries to lock the mutex)
                                                  // from a method that already has the mutex locked.
                                                  // huh....
};
}  // namespace glasscore
#endif  // CORRELATION_H
