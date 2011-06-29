#include "DcxServicesLayerTestSuite.h"
#include "Servers/ServersTestSuite.h"

DcxServicesLayerTestSuite::DcxServicesLayerTestSuite()
{
    addTest(new DcxServersTestSuite());
}
