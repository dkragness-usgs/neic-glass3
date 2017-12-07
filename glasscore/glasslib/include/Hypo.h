/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef HYPO_H
#define HYPO_H

#include <json.h>
#include "TTT.h"
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <random>

namespace glasscore {

// forward declarations
class CGlass;
class CPick;
class CCorrelation;
class CNode;

/**
 * \brief glasscore hypocenter class
 *
 * The CHypo class is the class that encapsulates everything necessary
 * to represent an earthquake hypocenter.
 *
 * CHypo also maintains a vector of CPick objects that make up the data that
 * supports the hypocenter
 *
 * CHypo contains functions to support association, disassociation, location,
 * removal, and various statistical calculations, as well as generating
 * output data in various formats.
 *
 * The CHypo associate() and prune() functions essentially make up
 * the glasscore data association engine, along with the various statistical
 * calculations.
 *
 * The glasscore location algorithm consists of the CHypo iterate(), focus(),
 * localize(), and locate() functions, along with the various statistical
 * calculations.
 *
 * CHypo uses smart pointers (std::shared_ptr).
 */
class CHypo {
 public:
	/**
	 * \brief CHypo constructor
	 *
	 * The constructor for the CHypo class.
	 * Sets allocated objects to null.
	 * Initializes members to default values.
	 */
	CHypo();

	/**
	 * \brief CHypo alternate constructor
	 *
	 * Constructs a CHypo using the provided values
	 *
	 * \param lat - A double containing the geocentric latitude in degrees to
	 * use
	 * \param lon - A double containing the geocentric longitude in degrees to
	 * use
	 * \param z - A double containing the geocentric depth in kilometers to use
	 * \param time - A double containing the julian time in seconds to use
	 * \param pid - A std::string containing the id of this hypo
	 * \param web - A std::string containing the name of the web that nucleated
	 * this hypo
	 * \param bayes - A double containing the bayesian value for this hypo.
	 * \param thresh - A double containing the threshold value for this hypo
	 * \param cut - An integer containing the Bayesian stack threshold for this
	 * hypo
	 * \return Returns true if successful, false otherwise.
	 */
	CHypo(double lat, double lon, double z, double time, std::string pid,
			std::string web, double bayes, double thresh, int cut,
			traveltime::CTravelTime* firstTrav,
			traveltime::CTravelTime* secondTrav, traveltime::CTTT *ttt,
			double resolution = 100);

	/**
	 * \brief CHypo alternate constructor
	 *
	 * Alternate constructor for the CHypo class that uses a CNode to create
	 * a CHypo
	 *
	 * \param node - A shared pointer to a CNode object containing the node to
	 * construct this hypo from.
	 */
	explicit CHypo(std::shared_ptr<CNode> node, traveltime::CTTT *ttt);

	/**
	 * \brief CHypo alternate constructor
	 *
	 * Alternate constructor for the CHypo class that uses a CCorrellation to
	 * create a CHypo
	 *
	 * \param corr - A shared pointer to a CNode object containing the
	 * correlation to construct this hypo from.
	 */
	explicit CHypo(std::shared_ptr<CCorrelation> corr,
					traveltime::CTravelTime* firstTrav,
					traveltime::CTravelTime* secondTrav, traveltime::CTTT *ttt);

	/**
	 * \brief CHypo destructor
	 *
	 * The destructor for the CHypo class.
	 * Cleans up all memory allocated objects.
	 */
	~CHypo();

	/**
	 * \brief CHypo clear function
	 */
	void clear();

	/**
	 * \brief CHypo initialization function
	 *
	 * Initializes hypo class to provided values.
	 *
	 * \param lat - A double containing the geocentric latitude in degrees to
	 * use
	 * \param lon - A double containing the geocentric longitude in degrees to
	 * use
	 * \param z - A double containing the geocentric depth in kilometers to use
	 * \param time - A double containing the julian time in seconds to use
	 * \param pid - A std::string containing the id of this hypo
	 * \param web - A std::string containing the name of the web that nucleated
	 * this hypo
	 * \param bayes - A double containing the bayesian value for this hypo.
	 * \param thresh - A double containing the threshold value for this hypo
	 * \param cut - An integer containing the Bayesian stack threshold for this
	 * hypo
	 * \return Returns true if successful, false otherwise.
	 */
	bool initialize(double lat, double lon, double z, double time,
					std::string pid, std::string web, double bayes,
					double thresh, int cut, traveltime::CTravelTime* firstTrav,
					traveltime::CTravelTime* secondTrav, traveltime::CTTT *ttt,
					double resolution = 100);

	/**
	 * \brief Add pick reference to this hypo
	 *
	 * Adds a shared_ptr reference to the given pick to this hypo,
	 * representing a graph database link between this hypocenter
	 * and the pick.  This link also represents a phase association.
	 *
	 * Note that this pick may or may not also be linked
	 * to other hypocenters
	 *
	 * \param pck - A std::shared_ptr to the CPick object to
	 * add.
	 */
	void addPick(std::shared_ptr<CPick> pck);

	/**
	 * \brief Remove pick reference from this hypo
	 *
	 * Remove a shared_ptr reference from the given pick to this hypo,
	 * breaking the graph database link between this hypocenter and the pick.
	 * The breaking of this link also represents a phase disassociation.
	 *
	 * Note that this pick may or may not be still linked
	 * to other hypocenters
	 *
	 * \param pck - A std::shared_ptr to the CPick object to
	 * remove.
	 */
	void remPick(std::shared_ptr<CPick> pck);

	bool hasPick(std::shared_ptr<CPick> pck);

	void clearPicks();

	/**
	 * \brief Add correlation reference to this hypo
	 *
	 * Adds a shared_ptr reference to the given correlation to this hypo,
	 * representing a graph database link between this hypocenter
	 * and the correlation.  This link also represents a correlation association.
	 *
	 * Note that this correlation may or may not also be linked
	 * to other hypocenters
	 *
	 * \param corr - A std::shared_ptr to the CCorrelation object to
	 * add.
	 */
	void addCorrelation(std::shared_ptr<CCorrelation> corr);

	/**
	 * \brief Remove correlation reference from this hypo
	 *
	 * Remove a shared_ptr reference from the given correlation to this hypo,
	 * breaking the graph database link between this hypocenter and the correlation.
	 * The breaking of this link also represents a correlation disassociation.
	 *
	 * Note that this correlation may or may not be still linked
	 * to other hypocenters
	 *
	 * \param corr - A std::shared_ptr to the CCorrelation object to
	 * remove.
	 */
	void remCorrelation(std::shared_ptr<CCorrelation> corr);

	bool hasCorrelation(std::shared_ptr<CCorrelation> corr);

	void clearCorrelations();

	/**
	 * \brief Calculate Gaussian random sample
	 *
	 * Calculate random normal gaussian deviate value using
	 * Box-Muller method
	 *
	 * \param avg - The mean average value to use in the Box-Muller method
	 * \param std - The standard deviation value to use in the Box-Muller method
	 * \return Returns the Gaussian random sample
	 */
	double gauss(double avg, double std);

	/**
	 * \brief Generate Random Number
	 *
	 * Generate s random number between x and y
	 *
	 * \param x - The minimum random number
	 * \param y - The maximum random number
	 * \return Returns the random sample
	 */
	double Rand(double x, double y);

	/**
	 * \brief Generate Hypo message
	 *
	 * Generate a json object representing this hypocenter in the
	 * "Hypo" format and send a pointer to this object to CGlass
	 * (and out of glasscore) using the send function (pGlass->send)
	 *
	 * \return Returns the generated json object.
	 */
	json::Object hypo();

	/**
	 * \brief Generate Event message
	 *
	 * Generate a json object representing this hypocenter in the
	 * "Event" format and send a pointer to this object to CGlass
	 * (and out of glasscore) using the send function (pGlass->send)
	 */
	void event();

	/**
	 * \brief Print basic hypocenter values to screen
	 *
	 * Causes CHypo to print the current hypocenter values
	 * (latitude, longitude, depth, etc.) to the console
	 */
	void summary();

	/**
	 * \brief Print advanced hypocenter values to screen
	 *
	 * Causes CHypo to print all the current hypocenter values,
	 * statistics, and pick information to the console
	 *
	 * \param src - A std::string indicating a context / reason
	 * for printing hypocenter values.
	 */
	void list(std::string src);

	/**
	 * \brief Check to see if pick could be associated
	 *
	 * Check to see if a given pick could be associated to this
	 * CHypo
	 *
	 * \param pick - A std::shared_ptr to the CPick object to
	 * check.
	 * \param sigma - A double value containing the sigma to use
	 * \paran sdassoc - A double value containing the standard
	 * deviation assocaiation limit to use
	 * \return Returns true if the pick can be associated, false otherwise
	 */
	bool associate(std::shared_ptr<CPick> pick, double sigma, double sdassoc);

	/**
	 * \brief Check to see if correlation could be associated
	 *
	 * Check to see if a given correlation could be associated to this
	 * CHypo
	 *
	 * \param corr - A std::shared_ptr to the CCorrelation object to
	 * check.
	 * \param tWindow - A double value containing the time window to use
	 * \paran xWindow - A double value containing the distance window
	 * \return Returns true if the correlation can be associated, false otherwise
	 */
	bool associate(std::shared_ptr<CCorrelation> corr, double tWindow,
					double xWindow);

	/* \brief Calculate pick affinity
	 *
	 * Calculate the association affinity between the given
	 * supporting pick and the current hypocenter based on a number of factors
	 * including the identified phase, distance to picked station,
	 * observation error, and other hypocentral properties
	 *
	 * \param pck - A std::shared_ptr to the pick to consider.
	 * \return Returns a double value containing the pick affinity
	 */
	double affinity(std::shared_ptr<CPick> pck);

	/* \brief Calculate correlation affinity
	 *
	 * Calculate the association affinity between the given
	 * supporting correlation and the current hypocenter based on a number of
	 * factors including distance to correlation station,
	 * time to correlation, and other properties
	 *
	 * \param corr - A std::shared_ptr to the correlation to consider.
	 * \return Returns a double value containing the correlation affinity
	 */
	double affinity(std::shared_ptr<CCorrelation> corr);

	/* \brief Remove picks that no longer fit association criteria
	 *
	 * Calculate the association affinity between the given
	 * supporting data and the current hypocenter based on a number of factors
	 * including the identified phase, distance to picked station,
	 * observation error, and other hypocentral properties
	 *
	 * \return Returns true if any picks removed
	 */
	bool prune();

	/**
	 * \brief Evaluate hypocenter viability
	 *
	 * Evaluate whether the hypocenter is viable, first by checking to
	 * see if the current number of supporting data exceeds the configured
	 * threshold, second checking if the current bayes value exceeds
	 * the configured threshold, and finally making a depth/gap check to
	 * ensure the hypocenter is not a "whispy".
	 *
	 * \return Returns true if the hypocenter is not viable, false otherwise
	 */
	bool cancel();

	/**
	 * \brief Evaluate hypocenter report suitability
	 *
	 * Evaluate whether the hypocenter is suitable to be reported,
	 *
	 * \return Returns true if the hypocenter can be reported, false otherwise
	 */
	bool reportCheck();

	/**
	 * \brief Calculate supporting data statistical distribution
	 *
	 * Calculate the statistical distribution of the supporting data in
	 * distance.  The values are reflected to give a mean of 0.  These
	 * statistics are used in the automatic supporting data disassocation
	 * process cull()
	 */
	void stats();

	/**
	 * \brief Synthetic annealing used by nucleation
	 *
	 * Synthetic annealing used by nucleation to rapidly achieve
	 * a viable starting location.
	 *
	 * Also computes supporting data statistical distribution by calling
	 * stats() and computes station weights by calling weights()
	 *
	 * \param nIter - An integer containing the number of iterations to perform,
	 * defaults to 250
	 * \param dStart - A double value containing the starting iteration step size
	 * in kilometers, default 100 km
	 * \param dStop - A double value containing the ending iteration step size
	 * in kilometers, default 1 km
	 * \return Returns a double value containing the final baysian fit.
	 */
	double anneal(int nIter = 250, double dStart = 100.0, double dStop = 1.0,
					double tStart = 5., double tStop = .5);

	/**
	 * \brief Localize this hypo
	 *
	 * Localize the hypocenter by computing the maximum bayesian fit via
	 * calls to the focus() function using different focus values (number
	 * of iterations, starting step size, and ending step size) based
	 * on the number of associated supporting data.
	 *
	 * Also computes station weights by calling weights()
	 *
	 * \return Returns a double value containing the final baysian fit.
	 */
	double localize();

	/**
	 * \brief brief grid_serach to minimize residuals for this hypo
	 *
	 * NOTE: Need more detailed description from Will
	 */
	void grid_search();

	/**
	 * \brief perform the grid search
	 *
	 * NOTE: Need more detailed description from Will
	 *
	 * \param distLimit - A double value containing the distance limit
	 * \param distStep - A double value containing the distance step
	 * \param depthLimit - A double value containing the depth limit
	 * \param depthStep - A double value containing the depth step
	 * \param timeLimit - A double value containing the time limit
	 * \param timeStep - A double value containing the time step
	 */
	void doSearch(double distLimit, double distStep, double depthLimit,
					double depthStep, double timeLimit, double timeStep);

	/**
	 * Locator which does annealing to find maximum of stacks
	 *
	 * \param nIter - An integer value containing the number of iterations
	 * \param dStart - A double value containing the distance starting value
	 * \param dStop - A double value containing the distance stopping value
	 * \param tStart - A double value containing the time starting value
	 * \param tStop - A double value containing the time stopping value
	 * \param nucleate - An int value sets if this is a nucleation which limits
	 * the phase used.
	 */
	void annealingLocate(int nIter, double dStart, double dStop, double tStart,
							double tStop, int nucleate = 0);

	/**
	 * Locator which does annealing to find minimum of sum of absolute of
	 * residuals
	 *
	 * \param nIter - An integer value containing the number of iterations
	 * \param dStart - A double value containing the distance starting value
	 * \param dStop - A double value containing the distance stopping value
	 * \param tStart - A double value containing the time starting value
	 * \param tStop - A double value containing the time stopping value
	 * \param nucleate - An int value sets if this is a nucleation which limits
	 * the phase used.
	 */
	void annealingLocateResidual(int nIter, double dStart, double dStop,
									double tStart, double tStop, int nucleate =
											0);

	/**
	 * Gets the stack of associated arrivals at location
	 *
	 * \param xlat - A double of the latitude to evaluate
	 * \param xlon - A double of the longitude
	 * \param dZ - A double of the depth bsl
	 * \param oT - A double of the oT
	 * \param nucleate - An int value sets if this is a nucleation which limits
	 * the phase used.
	 */
	double getBayes(double xlat, double xlon, double xZ, double oT,
					int nucleate);

	/**
	 * Get the sum of the absolute residuals at a location
	 *
	 * \param xlat - A double of the latitude to evaluate
	 * \param xlon - A double of the longitude
	 * \param dZ - A double of the depth bsl
	 * \param oT - A double of the oT
	 * \param nucleate - An int value sets if this is a nucleation which limits
	 * the phase used.
	 */
	double getSumAbsResidual(double xlat, double xlon, double xZ, double oT,
								int nucleate);

	/**
	 * \brief Write files for plotting output
	 *
	 */
	void graphicsOutput();

	/**
	 * \brief Sets the processing cycle count
	 *
	 * \param newCycle - An integer value containing the new cycle count
	 */
	void setCycle(int newCycle);

	/**
	 * \brief Calculate station weights
	 *
	 * Calculate the weight of each station for each pick in this
	 * hypo from the hypocentral distance and distance to nearby stations
	 * to reduce biases induced by network density differences
	 * \return Returns false if all weights are zero
	 */
	bool weights();

	bool resolve(std::shared_ptr<CHypo> hyp);

	/**
	 * \brief Pick Link checking function
	 *
	 * Causes CHypo to print any picks in vPick that are either improperly
	 * linked or do not belong to this CHypo.
	 */
	void trap();

	void incrementProcessCount();

	/**
	 * \brief A pointer to the main CGlass class, used to send output,
	 * look up travel times, encode/decode time, and call significance
	 * function.
	 */
	CGlass *pGlass;

	/**
	 * \brief  A std::string with the name of the initiating subnet trigger
	 * used during the nucleation process
	 */
	std::string sWeb;

	/**
	 * \brief An integer containing the number of stations needed to maintain
	 * association during the nucleation process
	 */
	int nCut;

	/**
	 * \brief A double containing the subnet specific Bayesian stack threshold
	 * used during the nucleation process
	 */
	double dThresh;

	/**
	 * \brief Holds the shifts for the grid search
	 */
	double searchVals[5];

	/**
	 * \brief An integer value containing this hypo's processing cycle count
	 */
	int iCycle;

	/**
	 * \brief A double value containing this hypo's origin time in julian
	 * seconds
	 */
	double tOrg;

	/**
	 * \brief A double value containing this hypo's latitude in degrees
	 */
	double dLat;

	/**
	 * \brief A double value containing this hypo's longitude in degrees
	 */
	double dLon;

	/**
	 * \brief A double value containing this hypo's depth in kilometers
	 */
	double dZ;

	/**
	 * \brief A boolean indicating whether this hypo needs to be refined.
	 */
	bool bRefine;

	/**
	 * \brief A boolean indicating if a Quake message was sent for this hypo.
	 */
	bool bQuake;

	/**
	 * \brief A boolean indicating if an Event message was sent for this hypo.
	 */
	bool bEvent;

	/**
	 * \brief A double value containing this hypo's Bayes statistic
	 */
	double dBayes;

	/**
	 * \brief A double value containing this hypo's minimum distance in degrees
	 */
	double dMin;

	/**
	 * \brief A double value containing this hypo's median distance in degrees
	 */
	double dMed;

	/**
	 * \brief A double value containing this hypo's maximum azimuthal gap in
	 * degrees
	 */
	double dGap;

	/**
	 * \brief A double value containing this hypo's distance standard deviation
	 */
	double dSig;

	/**
	 * \brief A double value the resolution of the triggering web
	 */
	double dRes;

	/**
	 * \brief A double value containing this hypo's distance sample excess
	 * kurtosis value
	 */
	double dKrt;

	/**
	 * \brief A double value containing the factor used to calculate this hypo's
	 * distance cutoff
	 */
	double dCutFactor;

	/**
	 * \brief A double value containing the percentage used to calculate this
	 *  hypo's distance cutoff
	 */
	double dCutPercentage;

	/**
	 * \brief A double value containing the minimum distance cutoff
	 */
	double dCutMin;

	/**
	 * \brief A double value containing this hypo's distance cutoff (2.0 * 80
	 * percentile)  in degrees
	 */
	double dCut;

	/**
	 * \brief A std::string containing this hypo's unique identifier
	 */
	std::string sPid;

	/**
	 * \brief A boolean indicating if this hypo is fixed (not allowed to change)
	 */
	bool bFixed;

	/**
	 * \brief A double value containing this hypo's ephemeral latitude in
	 * degrees during bayes maximization iterations
	 */
	double xLat;

	/**
	 * \brief A double value containing this hypo's ephemeral longitude in
	 * degrees during bayes maximization iterations
	 */
	double xLon;

	/**
	 * \brief A double value containing this hypo's ephemeral depth in
	 * kilometers during bayes maximization iterations
	 */
	double xZ;

	/**
	 * \brief A double value containing this hypo's ephemeral Bayes statistic
	 * during bayes maximization iterations
	 */
	double xBayes;

	/**
	 * \brief A double value containing this hypo's ephemeral origin time in
	 * julian seconds during bayes maximization iterations
	 */
	double xOrg;

	/**
	 * \brief An integer value containing the count of station weight values
	 * as of the last call to weights()
	 */
	int nWts;

	/**
	 * \brief A boolean indicating if a correlation was recently added to this
	 *  hypo.
	 */
	bool bCorrAdded;

	/**
	 * \brief An integer containing the number of times this hypo has been
	 * processed.
	 */
	int processCount;

	/**
	 * \brief An integer containing the number of times this hypo has been
	 * reported.
	 */
	int reportCount;

	/**
	 * \brief A double value containing this hypo's creation time in julian
	 * seconds
	 */
	double tCreate;

	/**
	 * \brief A vector of double values representing the weight of each pick
	 * used by this hypo as of the last call to weights()
	 */
	std::vector<double> vWts;

	/**
	 * \brief A vector of shared_ptr's to the pick data that supports this hypo.
	 */
	std::vector<std::shared_ptr<CPick>> vPick;

	/**
	 * \brief A vector of shared pointers to correlation data that support
	 * this hypo.
	 */
	std::vector<std::shared_ptr<CCorrelation>> vCorr;

	/**
	 * \brief A pointer to a CTravelTime object containing
	 * travel times for the first phase used to nucleate this hypo
	 */
	traveltime::CTravelTime* pTrv1;

	/**
	 * \brief A pointer to a CTravelTime object containing
	 * travel times for the second phase used to nucleate this hypo
	 */
	traveltime::CTravelTime* pTrv2;

	/**
	 * \brief A pointer to a CTTT object containing the travel
	 * time phases and branches used for association and location
	 */
	traveltime::CTTT *pTTT;

	/**
	 * \brief A recursive_mutex to control threading access to CHypo.
	 * NOTE: recursive mutexes are frowned upon, so maybe redesign around it
	 * see: http://www.codingstandard.com/rule/18-3-3-do-not-use-stdrecursive_mutex/
	 * However a recursive_mutex allows us to maintain the original class
	 * design as delivered by the contractor.
	 */
	std::recursive_mutex hypoMutex;

	/**
	 * \brief A random engine used to generate random numbers
	 */
	std::default_random_engine m_RandomGenerator;
};
}  // namespace glasscore
#endif  // HYPO_H