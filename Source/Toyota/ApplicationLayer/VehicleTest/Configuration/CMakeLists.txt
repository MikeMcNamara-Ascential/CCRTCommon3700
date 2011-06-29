#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR} FILES
	Toyota.Braketest.SelectionTable.xml
	Toyota.Rolltest.SelectionTable.xml
	Toyota.VehicleTestConfig.xml
	Toyota.Rolltest.VehicleTestConfig.xml
)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(BuildRecords)
ADD_SUBDIRECTORY(DriveCurves)
