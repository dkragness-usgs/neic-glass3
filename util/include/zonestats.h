/*****************************************
 * This file is documented for Doxygen.
 * If you modify this file please update
 * the comments so that Doxygen will still
 * be able to work.
 ****************************************/
#ifndef ZONESTATS_H
#define ZONESTATS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace glass3 {
  namespace util {


    typedef struct _ZoneStatsInfoStruct
    {
      float fLat;
      float fLon;
      int   nEventCount;
      float fMaxDepth;
      float fMinDepth;
      float fAvgDepth;
      float fMaxMag;
      float fMinMag;
      float fAvgMag;
    } ZoneStatsInfoStruct;
    /**
     * \brief glassutil zonestats class
     *
     * The CZonestats class encapsulates the logic and functionality needed
     * to load and serve zone statistics for historical seismic events
     */

    bool ZSLatLonCompareLessThan(const ZoneStatsInfoStruct & zs1, const ZoneStatsInfoStruct & zs2);

    /**
    * \brief ZoneStatsInfoStruct comparison function
    *
    * ZSCompareByLatLon contains the comparison function used by lower_bound when
    * searching for data in the CZoneStats vector
    */
    bool ZSCompareByLatLon(const  ZoneStatsInfoStruct &lhs,
                           const  ZoneStatsInfoStruct &rhs);

    class CZoneStats {
    public:
      /**
      * \brief CZoneStats constructor
      *
      * The constructor for the CGeo class.
      */
      CZoneStats();

      /**
      * \brief CZoneStats configuration function
      *
      * This function configures the CZoneStats class
      * \param pConfigFileName - A pointer to the config filename, from which to read zonestats info.
      * \return returns true if successful.
      */
      virtual bool setup(const std::string * pConfigFileName);

      /**
      * \brief CZoneStats clear function
      *
      * The clear function for the CZoneStats class.
      * Clears all zonestats info.
      */
      virtual void clear();

      /**
      * \brief CZoneStats retrieval function
      *
      * This function calculates the significance function for glasscore,
      * which is the bell shaped curve with sig(0, x) pinned to 0.
      *
      * \param tdif - A double containing x value.
      * \param sig - A double value containing the sigma,
      * \return Returns a double value containing significance function result
      */
      const ZoneStatsInfoStruct * GetZonestatsInfoForLatLon(double dLat, double dLon);

      float GetMaxDepthForLatLon(double dLat, double dLon);

      float GetRelativeObservabilityOfSeismicEventsAtLocation(double dLat, double dLon);

    protected:

      std::vector<ZoneStatsInfoStruct> m_vZSData;

      int m_nTotalNumEvents;

      double m_dAvgObservabilityPerBin;

      float fLatBinSizeDeg;
      float fLonBinSizeDeg;

      ZoneStatsInfoStruct m_ZSDefault;

    public:
      static const char* szLatGridBinSize;
      static const char* szLonGridBinSize;
      static const char* szLatLonHeader;

      static const int nNumExpectedFields = 9;
      static const int iParseStateGetSZRecords = 3;
      static const int iParseStateGetLatGBS = 0;
      static const int iParseStateGetLonGBS = 1;
      static const int iParseStateGetRecordHeader = 2;

      static const float depthInvalid;

    };  // CZoneStats class
  }  // namespace util
}  // namespace glass3
#endif  // ZONESTATS_H
