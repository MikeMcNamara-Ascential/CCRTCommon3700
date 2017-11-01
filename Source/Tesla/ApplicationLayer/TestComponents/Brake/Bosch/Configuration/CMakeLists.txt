#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Brake FILES
        Tesla.BoschABSTC.xml        
        Tesla.ModelXBoschABSTC.xml
        Tesla.ModelSRWDBoschABSTC.xml
        Tesla.ModelSBoschABSTC.xml
	Tesla.Tilburg.BoschABSTC.xml
        BoschBrakeFaultList.xml
        )
        
