#------------------------------------------------------------------------------------------
# Auto Generated
# Tell CMake what files to install
#------------------------------------------------------------------------------------------
INSTALL_FILES(${VEHICLETEST_CONFIG_DIR} FILES
      Isuzu.SelectionTable.xml
      Isuzu.Charlotte.508702.SelectionTable.xml
      Isuzu.VehicleTestConfig.xml
      Isuzu.Charlotte.508702.VehicleTestConfig.xml
)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(DriveCurves)
