#------------------------------------------------------------------------------------------
# Define the source files needed to build the module interface library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE BUILD_RECORDS *.xml)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/BuildRecords FILES
    ${BUILD_RECORDS})
