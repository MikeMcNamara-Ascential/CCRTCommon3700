#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(DriveCurves)
ADD_SUBDIRECTORY(BuildRecords)

#------------------------------------------------------------------------------------------
# Tell CMake what files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR} FILES
    Youngman.SelectionTable.xml
    Youngman.VehicleTestConfig.xml)
