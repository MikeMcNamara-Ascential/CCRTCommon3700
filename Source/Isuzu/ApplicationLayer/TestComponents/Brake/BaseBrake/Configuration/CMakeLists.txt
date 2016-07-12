#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/Brake FILES
	Isuzu.BaseBrakeTc.xml
        Isuzu.FrontAxleBaseBrakeTc.xml
        Isuzu.RearAxleBaseBrakeTc.xml
	) 
