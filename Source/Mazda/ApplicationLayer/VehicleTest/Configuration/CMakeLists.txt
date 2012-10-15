#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR} FILES
	Mazda.Braketest.SelectionTable.xml
	Mazda.Rolltest.SelectionTable.xml
	Mazda.VehicleTestConfig.xml
	Mazda.Rolltest.VehicleTestConfig.xml
)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(BuildRecords)
ADD_SUBDIRECTORY(DriveCurves)
