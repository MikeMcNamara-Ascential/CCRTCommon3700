#------------------------------------------------------------------------------------------
# Build a list of all the XML files
#------------------------------------------------------------------------------------------
FILE(GLOB VEHICLE_TEST_CONFIG_FILES *.xml)

#------------------------------------------------------------------------------------------
# Tell CMake to install all the config files in the Configuration/Servers directory
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR} FILES ${VEHICLE_TEST_CONFIG_FILES})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(DriveCurves)
