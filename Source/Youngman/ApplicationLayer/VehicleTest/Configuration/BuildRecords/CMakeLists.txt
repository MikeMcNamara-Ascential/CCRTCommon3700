#------------------------------------------------------------------------------------------
# Tell CMake what files to install
#------------------------------------------------------------------------------------------
FILE(GLOB installFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR}/BuildRecords FILES
        ${installFiles})
