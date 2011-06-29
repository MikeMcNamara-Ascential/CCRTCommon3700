#------------------------------------------------------------------------------------------
# Build a list of all the XML files
#------------------------------------------------------------------------------------------
FILE(GLOB DCBR_CONFIG_FILES *.xml)

#------------------------------------------------------------------------------------------
# Tell CMake to install all the config files in the Configuration/Servers directory
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/DriveCurves FILES ${DCBR_CONFIG_FILES})
