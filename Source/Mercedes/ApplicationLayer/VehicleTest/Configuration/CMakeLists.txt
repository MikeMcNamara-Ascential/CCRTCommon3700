#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR} FILES
	      DCX.DCBR.SelectionTable.xml
	      VehicleTestConfig.xml
	      )

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(BuildRecords)
ADD_SUBDIRECTORY(DriveCurves)
