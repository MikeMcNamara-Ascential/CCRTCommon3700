#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Brake FILES
        Tesla.BoschABSTC.xml
	Tesla.Tilburg.BoschABSTC.xml
        BoschBrakeFaultList.xml
        )
        
