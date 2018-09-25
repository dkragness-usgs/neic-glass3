#include <zonestats.h>
int main()
{
  glass3::util::CZoneStats zs;
  const glass3::util::ZoneStatsInfoStruct * pZSI;
  float fMaxDepth;
  float fObs;


  zs.setup(&std::string("../testdata/qa_zonestats2.txt"));

  pZSI = zs.GetZonestatsInfoForLatLon(36.0, -115.0);

  if(pZSI != NULL)
    printf("Zonestats depth info for %.2f/%.2f is %.0f %.0f %.0f\n",
           pZSI->fLat, pZSI->fLon, pZSI->fMaxDepth, pZSI->fMinDepth, pZSI->fAvgDepth);

  fMaxDepth = zs.GetMaxDepthForLatLon(36.0, -115.0);

  fObs = zs.GetRelativeObservabilityOfSeismicEventsAtLocation(36.0, -115.0);
  printf("Zonestats depth/obs info for %.2f/%.2f is %.0f, %.2e\n",
         pZSI->fLat, pZSI->fLon, fMaxDepth, fObs);

  return(0);
}
