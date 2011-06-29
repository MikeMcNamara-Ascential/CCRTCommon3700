#include "DcxServersTestSuite.h"
#include "PlantHostOutbound/DcxPlantHostOutboundTestSuite.h"

DcxServersTestSuite::DcxServersTestSuite()
{
    addTest(new DcxPlantHostOutboundTestSuite());
}
