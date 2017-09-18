#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Brake FILES
        Tesla.BoschABSTC.xml        
        Tesla.Roadster2WD0BBoschABSTC.xml
        Tesla.Roadster2WDMBBoschABSTC.xml
        Tesla.Roadster4WD0BBoschABSTC.xml
        Tesla.Roadster4WDMBBoschABSTC.xml
        Tesla.Sedan2WD0BBoschABSTC.xml
        Tesla.Sedan2WDMBBoschABSTC.xml        
        Tesla.Sedan4WD0BBoschABSTC.xml
        Tesla.Sedan4WDMBBoschABSTC.xml
	Tesla.Tilburg.BoschABSTC.xml
        BoschBrakeFaultList.xml
        )
        
