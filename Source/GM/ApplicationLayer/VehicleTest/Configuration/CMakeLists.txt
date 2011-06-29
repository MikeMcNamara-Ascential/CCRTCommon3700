#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR} FILES
	GM.SelectionTable.xml
	GM.VehicleTestConfig.xml
	GM.SaoCaetano.VehicleTestConfig.xml)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(DriveCurves)
ADD_SUBDIRECTORY(BuildRecords)
