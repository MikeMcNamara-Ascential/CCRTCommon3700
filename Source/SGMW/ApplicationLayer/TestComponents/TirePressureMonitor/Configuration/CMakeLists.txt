#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
MESSAGE(STATUS "Processing TPMS Config Files")
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/TPMS FILES
        UdsTpmsTc.xml
	)
