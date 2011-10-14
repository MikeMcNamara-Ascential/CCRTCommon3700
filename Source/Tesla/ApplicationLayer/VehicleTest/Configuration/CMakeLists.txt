#------------------------------------------------------------------------------------------
# Tell CMake what Config files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR} FILES
	Tesla.Freemont.SelectionTable.xml
	Tesla.SelectionTable.xml
	Tesla.VehicleTestConfig.xml
)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(DriveCurves)
