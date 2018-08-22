/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef GLASS_H
#define GLASS_H

#include <json.h>
#include <string>
#include <memory>
#include "Terra.h"
#include "Ray.h"
#include "TTT.h"
#include "TravelTime.h"

namespace glasscore {

// forward declarations
class CWebList;
class CSiteList;
class CPickList;
class CHypoList;
class CDetection;
class CCorrelationList;
struct IGlassSend;

/**
 * \brief glasscore interface class
 *
 * The CGlass class is the class that sets up and maintains the glass
 * association engine, and acts as the interface between the glasscore library
 * and any clients.
 *
 * CGlass initializes the traveltime library, allocates the site, pick, and
 * hypo lists, creates and maintains the detection web, and manages
 * communication between glasscore and clients via the dispatch function
 * (receiving) and an IGlassSend interface pointer variable (sending).
 *
 * CGlass also performs traveltime library testing during initialization,
 * time encoding/decoding as well as calculating the significance functions
 * and normal distributions as needed.
 *
 * All communication (configuration, input data, or output results ) to / from
 * CGlass is via deserialized json messages as pointers to supereasyjson
 * json::objects.
 */


/*
DK REVIEW 20180821
Glass parameters should be reorganized, grouped into sub-structs, and dup's should be jetisoned if possible.

NucleationParams
int nNucleate;   nNumPicksRequiredToNucleate
int nDetect;     nNumBestStationsToUseForNucleation
double dThresh;  dNucleationTestThresholdValueBasedOnWhoKnowsWhat


HypoRefinementParams
double sdAssociate; dMaxResidualInSDsToAssoc
double sdPrune;  dMinResidualInSDsToPrune
double expAffinity;  dHypoPickAffinity
double dCutFactor;   dMaxStaDistMultFactor
double dCutPercentage; dMaxStaDistBaseStaSelectionPcntg
double dCutMin;      dMinAllowMaxStaDistDeg
double beamMatchingDistanceWindow; dBeamMatchingDistWindowDeg
double beamMatchingAzimuthWindow; dBeamMatchingAzmWindowDeg
double correlationMatchingTWindow; dCorrelationMatchingTimeWinSec
double correlationMatchingXWindow; dCorrelationMatchingHypoDistWinDeg


RefinementProcessingControlParams
int iCycleLimit;
int correlationCancelAge; dCorrelationCancelAgeSecs



GlassCrazyParamsToDeprecate
double avgDelta;
double avgSigma;
bool minimizeTTLocator;
double dReportThresh;  limitation of reporting should be handled in outputs
double nReportCut;
int nSitePickMax;       // should be parameter of SiteList
int nHypoMax;           // Should be parameter of HypoList
int nCorrelationMax;    // Should be parameter of CorrelationList (or PickList if we can make Correlation a glorified Pick)
int nPickMax;           // Should be parameter of PickList
double pickDuplicateWindow;  // Should parameter of PickList, since that is entry point for picks

DebugOutputParams
int graphicsSteps;
double graphicsStepKM;
std::string graphicsOutFolder;
bool graphicsOut;
bool testLocator;
bool testTimes;
*/
class CGlass {
 public:
	/**
	 * \brief CGlass constructor
	 *
	 * The constructor for the CGlass class.
	 * Sets allocated lists and objects to null.
	 * Initializes members to default values.
	 */
	CGlass();

	/**
	 * \brief CGlass destructor
	 *
	 * The destructor for the CGlass class.
	 * Cleans up all memory allocated to lists and objects.
	 */
	~CGlass();

	/**
	 * \brief CGlass communication receiving function
	 *
	 * The function used by CGlass to receive communication
	 * (such as configuration or input data), from outside the
	 * glasscore library.
	 *
	 * CGlass will forward the communication on to the pick, site,
	 * or hypo lists, or the detection web if CGlass cannot
	 * use the communication.
	 *
	 * \param com - A pointer to a json::object containing the
	 * communication.
	 * \return Returns true if the communication was handled by CGlass,
	 * false otherwise
	 */
	bool dispatch(std::shared_ptr<json::Object> com);  // DK REVIEW 20180821 - name should be changed to receiveMsg()

	/**
	 * \brief CGlass communication sending function
	 *
	 * The function used by CGlass to send communication
	 * (such as output data), to outside the glasscore library
	 * using an IGlassSend interface pointer.
	 *
	 * \param com - A pointer to a json::object containing the
	 * communication.
	 * \return Returns true if the communication was sent via
	 * a valid IGlassSend interface pointer, false otherwise
	 */
	bool send(std::shared_ptr<json::Object> com);

	/**
	 * \brief CGlass initialization function
	 *
	 * The function used by CGlass to initialize the glasscore
	 * library.  This function loads the earth model, sets the
	 * association parameters, sets up  the ray path calculator,
	 * creates the detection web configures and tests the phase
	 * and branch travel times used for association, creates the
	 * pick, site, and hypo lists, and sets up the output format
	 *
	 * \param com - A pointer to a json::object containing the
	 * configuration to use in initialization.
	 * \return Returns true if the initialization was successful,
	 * false otherwise
	 */
	bool initialize(std::shared_ptr<json::Object> com);  // DK REVIEW 20180820  - how come this funciton isn't called setup()  ?

	/**
	 * \brief CGlass clear function
	 *
	 */
	void clear();

	/**
	 * \brief CGlass significance function
	 *
	 * This function calculates the significance function for glasscore,
	 * which is the bell shaped curve with sig(0, x) pinned to 0.
	 *
	 * \param tdif - A double containing x value.
	 * \param sig - A double value containing the sigma,
	 * \return Returns a double value containing significance function result
	 */
	double sig(double tdif, double sig);  // DK REVIEW 20180820  -  Rename something meaningful.

	/**
	 * \brief CGlass laplacian significance function (PDF)
	 *
	 * This function calculates a laplacian significance used in associator.
	 * This should have the affect of being L1 normish, instead of L2 normish.
	 * Unlike the other significance function, this returns the PDF value
	 * \param tdif - A double containing x value.
	 * \param sig - A double value containing the sigma,
	 * \return Returns a double value containing significance function result
	 */
	double sig_laplace_pdf(double tdif, double sig);

	/**
	 * \brief An IGlassSend interface pointer used to send communication
	 * (such as output data), to outside the glasscore library
	 */
	glasscore::IGlassSend *piSend;

	/**
	 * \brief check to see if each thread is still functional
	 *
	 * Checks each thread to see if it is still responsive.
	 */
	bool statusCheck();      // DK REVIEW 20180821 - John has healthCheck() funciton that I think is the same
                           // as this.  Seems like we could bring Glasscore stuff into the John standard
                           // architecture fold.

  // DK REVIEW 20180821
  // Seems like processing is broken up into two pieces:
  // 1) Nucleation/Event-Detection
  // 2) Hypocenter refinement/verification/consolidation
  // Glass tuning parameters should be split along these lines as well.
  // All application is probably done at the Hypo/Pick level, which is global to Glass
  // but detection properties should be web based  Glass->Web->Node->Hypo.
  // Group 1 params should be properties of the detection web, but defaults should be loadable into the Glass class/object
  // Group 2 params should be properties of either Glass or the detection Web depending on whether all events are
  // expected to meet the same standards for survival/publication or whether there are different requirements depending
  // on the detection web they came from.  I'd vote for them being general to Glass, but I could understand if they were
  // made web-specific.
  //  Params should be put into a struct to help make the code easier to read:
  // NucleationParamsStruct
  // HypocenterRefinementParamsStruct
  // RandomGlassSh1tStruct
	/**
	 * \brief Average delta getter
	 * \return the average delta
	 */
	double getAvgDelta() const;

	/**
	 * \brief Average sigma getter
	 * \return the average sigma
	 */
	double getAvgSigma() const;

	/**
	 * \brief Beam matching azimuth window getter
	 * \return the beam matching azimuth window in degrees
	 */
	double getBeamMatchingAzimuthWindow() const;

	/**
	 * \brief Beam matching distance window getter
	 * \return the beam matching distance window in degrees
	 */
	double getBeamMatchingDistanceWindow() const;

	/**
	 * \brief Correlation cancel age getter
	 * \return the correlation cancel age in seconds
	 */
	int getCorrelationCancelAge() const;

	/**
	 * \brief Correlation matching time window getter
	 * \return the correlation matching time window in seconds
	 */
	double getCorrelationMatchingTWindow() const;

	/**
	 * \brief Correlation matching distance window getter
	 * \return the correlation matching distance window in degrees
	 */
	double getCorrelationMatchingXWindow() const;

	/**
	 * \brief Distance cutoff factor getter
	 * \return the distance cutoff factor
	 */
	double getCutFactor() const;

	/**
	 * \brief Average distance cutoff minimum getter
	 * \return the minimum distance cutoff in degrees
	 */
	double getCutMin() const;

	/**
	 * \brief Distance cutoff percentage getter
	 * \return the distance cutoff percentage
	 */
	double getCutPercentage() const;

	/**
	 * \brief Report threshold getter
	 * \return the reporting viability threshold
	 */
	double getReportThresh() const;

	/**
	 * \brief Nucleation threshold getter
	 * \return the nucleation viability threshold
	 */
	double getThresh() const;

	/**
	 * \brief Exponential Affinity getter
	 * \return the exponential factor used for pick affinity
	 */
	double getExpAffinity() const;

	/**
	 * \brief Graphics output flag getter
	 * \return a flag indicating whether to output graphics files
	 */
	bool getGraphicsOut() const;

	/**
	 * \brief Graphics output folder getter
	 * \return the folder to output graphics files to
	 */
	const std::string& getGraphicsOutFolder() const;

	/**
	 * \brief Graphics step getter
	 * \return the graphics step size in km
	 */
	double getGraphicsStepKm() const;

	/**
	 * \brief Graphics steps getter
	 * \return the number of graphic steps
	 */
	int getGraphicsSteps() const;

	/**
	 * \brief Cycle limit getter
	 * \return the limit of processing cycles
	 */
	int getCycleLimit() const;

	/**
	 * \brief Graphics minimize TT locator getter
	 * \return the flag indicating whether to use the minimizing tt locator
	 */
	bool getMinimizeTtLocator() const;

	/**
	 * \brief Maximum number of correlations getter
	 * \return the maximum number of correlations
	 */
	int getCorrelationMax() const;

	/**
	 * \brief Default number of detection stations getter
	 * \return the default number of detections used in a node
	 */
	int getDetect() const;

	/**
	 * \brief Maximum number of hypocenters getter
	 * \return the maximum number of hypocenters
	 */
	int getHypoMax() const;

	/**
	 * \brief Default number of picks for nucleation getter
	 * \return the default number of nucleations used in for a detection
	 */
	int getNucleate() const;

	/**
	 * \brief Maximum number of picks getter
	 * \return the maximum number of picks
	 */
	int getPickMax() const;

	/**
	 * \brief Report cutoff getter
	 * \return the reporting cutoff
	 */
	double getReportCut() const;

	/**
	 * \brief Maximum number of picks with a site getter
	 * \return the maximum number of picks stored with a site
	 */
	int getSitePickMax() const;

	/**
	 * \brief Correlation list getter
	 * \return a pointer to the correlation list
	 */
	CCorrelationList*& getCorrelationList();

	/**
	 * \brief Detection getter
	 * \return a pointer to the detection processor
	 */
	CDetection*& getDetection();

	/**
	 * \brief Hypocenter list getter
	 * \return a pointer to the hypocenter list
	 */
	CHypoList*& getHypoList();

	/**
	 * \brief Pick duplicate time window getter
	 * \return the pick duplication time window in seconds
	 */
	double getPickDuplicateWindow() const;

	/**
	 * \brief Pick list getter
	 * \return a pointer to the pick list
	 */
	CPickList*& getPickList();

	/**
	 * \brief Site list getter
	 * \return a pointer to the site list
	 */
	CSiteList*& getSiteList();

	/**
	 * \brief Default travel time  getter
	 * \return the default nucleation travel time
	 */
	std::shared_ptr<traveltime::CTravelTime>& getTrvDefault();

	/**
	 * \brief Travel time list getter
	 * \return the list of association travel times
	 */
	std::shared_ptr<traveltime::CTTT>& getTTT();

	/**
	 * \brief Web list getter
	 * \return a pointer to the web list
	 */
	CWebList*& getWebList();

	/**
	 * \brief SD associate getter
	 * \return the standard deviation cutoff used for association
	 */
	double getSdAssociate() const;

	/**
	 * \brief SD prune getter
	 * \return the standard deviation cutoff used for pruning
	 */
	double getSdPrune() const;

	/**
	 * \brief Testing locator flag getter
	 * \return a flag indicating whether to output locator testing files
	 */
	bool getTestLocator() const;

	/**
	 * \brief Testing travel times flag getter
	 * \return a flag indicating whether to output travel times testing files
	 */
	bool getTestTimes() const;

 private:

/*** The next X params exist here as a configuration convenience.  They are applied
   * at the Node level, but are inherited Glass->Web->Node
 */
	/**
	 * \brief A double value containing the default number of picks that  // DK REVIEW 20180821  -it's not a tumor!  I mean, it's an int, not a double...
	 * that need to be gathered to trigger the nucleation of an event.
	 * This value can be overridden in a detection grid (Web) if provided as
	 * part of a specific grid setup.
   * This param exists here as a configuration convenience.  It is applied
   * at the Node level, but is inherited Glass->Web->Node
	 */
	int nNucleate;

	/**
	 * \brief A double value containing the default number of closest stations
	 * to use  when generating a node for a detection array.
	 * This value can be overridden in a detection grid (Web) if provided as
	 * part of a specific grid setup.
   * This param exists here as a configuration convenience.  It is applied
   * at the Node level, but is inherited Glass->Web->Node
   */
	int nDetect;

	/**
	 * \brief A double value containing the default viability threshold needed
	 * to exceed for a nucleation to be successful.
	 * This value can be overridden in a detection grid (Web) if provided as
	 * part of a specific grid setup.
	 */
	double dThresh;

	/**
	 * \brief A double value containing the standard deviation cutoff used for
	 * associating a pick with a hypocenter.
	 */
	double sdAssociate;

	/**
	 * \brief A double value containing the standard deviation cutoff used for
	 * pruning a pick from a hypocenter.
	 */
	double sdPrune;

	/**
	 * \brief A double value containing the exponential factor used when
	 * calculating the affinity of a pick with a hypocenter.
	 */
	double expAffinity;

	/**
	 * \brief A double value containing the average station distance in degrees,
	 * used as the defining value for a taper compensate for station density in
	 * Hypo::weights()
   // DK REVIEW 20180821  - I don't understand the point of avgDelta and avgSigma
   // at a Glass level.
	 */
	double avgDelta;

	/**
	 * \brief A double value containing the exponent of the gaussian weighting
	 * kernel in degrees.  It is used to compensate for station density in
	 * Hypo::weights()
	 */
	double avgSigma;

	/**
	 * \brief A double value containing the factor used to calculate a hypo's
	 * distance cutoff
	 */
	double dCutFactor;

	/**
	 * \brief A double value containing the percentage used to calculate a
	 *  hypo's distance cutoff
	 */
	double dCutPercentage;

	/**
	 * \brief A double value containing the minimum distance cutoff in degrees
	 */
	double dCutMin;

	/**
	 * \brief A pointer to a CWeb object containing the detection web
	 */
	CWebList *pWebList;

	/**
	 * \brief A pointer to a CTravelTime object containing
	 *default travel time for nucleation
	 */
	std::shared_ptr<traveltime::CTravelTime> pTrvDefault;

	/**
	 * \brief A pointer to a CTTT object containing the travel
	 * time phases and branches used by glasscore for association
	 */
	std::shared_ptr<traveltime::CTTT> pTTT;

	/**
	 * \brief the std::mutex for traveltimes
	 */
	mutable std::mutex m_TTTMutex;

	/**
	 * \brief A pointer to a CSiteList object containing all the sites
	 * known to glasscore
	 */
	CSiteList *pSiteList;

	/**
	 * \brief A pointer to a CPickList object containing the last n picks sent
	 * into glasscore (as determined by nPickMax).  Picks passed into pPickList
	 * are also passed into the pWeb detection web
	 */
	CPickList *pPickList;

	/**
	 * \brief A pointer to a CPickList object containing the last n hypos sent
	 * into glasscore (as determined by nPickMax)
	 */
	CHypoList *pHypoList;

	/**
	 * \brief A pointer to a CCorrelationList object containing the last n
	 * correlations sent into glasscore
	 */
	CCorrelationList *pCorrelationList;

	/**
	 * \brief A pointer to a CDetection object used to process detections sent
	 * into glasscore
	 */
	CDetection *pDetection;

	/**
	 * \brief An integer containing the maximum number of picks stored by
	 * pPickList
	 */
	int nPickMax;

	/**
	 * \brief An integer containing the maximum number of correlations stored by
	 * pCorrelationList
	 */
	int nCorrelationMax;

	/**
	 * \brief An integer containing the maximum number of picks stored by
	 * the vector in a site
	 */
	int nSitePickMax;

	/**
	 * \brief An integer containing the maximum number of hypocenters stored by
	 * pHypoList
	 */
	int nHypoMax;

	/**
	 * \brief Window in seconds to check for 'duplicate' picks at same station.
	 * If new pick is within window, it isn't added to pick list.
	 */
	double pickDuplicateWindow;

	/**
	 * \brief Time Window to check for matching correlations in seconds. Used
	 * for checking for duplicate correlations and associating correlations to
	 * hypos
	 */
	double correlationMatchingTWindow;

	/**
	 * \brief Distance Window to check for matching correlations in degrees.
	 * Used for checking for duplicate correlations and associating correlations
	 * to hypos
	 */
	double correlationMatchingXWindow;

	/**
	 * \brief Azimuth Window to check for matching beams in degrees. Used for
	 * nucleating beams and associating beams to hypos
	 */
	double beamMatchingAzimuthWindow;

	/**
	 * \brief Distance Window to check for matching beams in degrees. Used for
	 * nucleating beams and associating beams to hypos
	 */
	double beamMatchingDistanceWindow;

	/**
	 * \brief age of correlations before allowing cancel in seconds
	 */
	int correlationCancelAge;

	/**
	 * \brief Bool to decide when to print out travel-times.
	 */
	bool testTimes;

	/**
	 * \brief Bool to decide when to print files for locator test
	 */
	bool testLocator;

	/**
	 * \brief Flag indicating whether to output info for graphics.
	 */
	bool graphicsOut;

	/**
	 * \brief Output locations info for graphics.
	 */
	std::string graphicsOutFolder;

	/**
	 * \brief For graphics, the step size for output.
	 */
	double graphicsStepKM;

	/**
	 * \brief For graphics, the number of steps from hypocenter.
	 */
	int graphicsSteps;

	/**
	 * \brief Maximum number of processing cycles a hypo can do without having
	 * new data associated
	 */
	int iCycleLimit;

	/**
	 * \brief boolean to use a locator which minimizes TT as opposed to
	 * maximizes significance functions
	 */
	bool minimizeTTLocator;

	/**
	 * \brief number of data required for reporting a hypo
	 */
	double nReportCut;

	/**
	 * \brief A double value containing the default viability threshold needed
	 * to for reporting a hypo
	 */
	double dReportThresh;
};
}  // namespace glasscore
#endif  // GLASS_H
