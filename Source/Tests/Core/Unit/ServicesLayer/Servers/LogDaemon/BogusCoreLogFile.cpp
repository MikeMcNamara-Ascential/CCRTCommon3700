#include "BogusCoreLogFile.h"




BogusCoreLogFile::BogusCoreLogFile(const std::string &pFilename, const std::string &pBaseName)
	: CoreLogFile(".", pFilename, pBaseName, 4, 4, 4, 4 ) {
	incrementFileNameCalled = 0;
}

BogusCoreLogFile::~BogusCoreLogFile() {
}

/*void BogusCoreLogFile::RedirectToFile(std::string) {

}*/

int BogusCoreLogFile::RotateIfNeeded()
{
    return(0);
}

void BogusCoreLogFile::IncrementFileName(const std::string& suffix) {
	incrementFileNameCalled++;
}

int BogusCoreLogFile::LimitCoreLog()
{
    return(0);
}
