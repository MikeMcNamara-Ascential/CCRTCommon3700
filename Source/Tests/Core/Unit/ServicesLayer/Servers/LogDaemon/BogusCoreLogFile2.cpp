#include "BogusCoreLogFile2.h"

BogusCoreLogFile2::BogusCoreLogFile2(const std::string &pFilePath,
	const std::string &pFilename, const std::string &pBaseName)
	:	CoreLogFile(pFilePath, pFilename, pBaseName, 4, 4, 4, 4 ),
		opencalled(0),
		closecalled(0),
		lclcalled(0)
{
}

BogusCoreLogFile2::~BogusCoreLogFile2() {
}

int BogusCoreLogFile2::Open()
{
	opencalled++;
	return 0;
}

int BogusCoreLogFile2::Close()
{
	closecalled++;
	return 0;
}

int BogusCoreLogFile2::LimitCoreLog()
{
	lclcalled++;
	return 0;
}
