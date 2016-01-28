#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Emissions FILES
        BoschDCUTc.xml
	IsuzuEmissionsTc.xml
	My13IsuzuEmissionsTc.xml
	My14IsuzuEmissionsTc.xml
	PIMTc.xml
	EmissionsFaults.xml
	My13EmissionsFaults.xml
	My14EmissionsFaults.xml
	)
	
