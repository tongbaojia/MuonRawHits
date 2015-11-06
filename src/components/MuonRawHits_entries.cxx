#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../BaseAnalysis.h"

DECLARE_ALGORITHM_FACTORY(BaseAnalysis)

DECLARE_FACTORY_ENTRIES(MuonRawHits)
{
  DECLARE_ALGORITHM(BaseAnalysis);
}
