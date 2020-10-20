#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Emissions FILES
        BoschDCUTc.xml
		IsuzuEmissionsTc.xml
		My13IsuzuEmissionsTc.xml
		My14IsuzuEmissionsTc.xml
        My18IsuzuEmissionsTc.xml
        MY19_2_IsuzuEmissionsTc.xml
        My17IsuzuSingleAxleEmissionsTc.xml
		PIMTc.xml
		MY19_2_PIMTc.xml
		EmissionsFaults.xml
		My13EmissionsFaults.xml
		My14EmissionsFaults.xml
        My17SingleAxleEmissionsFaults.xml
        My18EmissionsFaults.xml
		MY20BIsuzuDeltaEmissionsTc.xml
	)
	
