#------------------------------------------------------------------------------------------
# Tell CMake where to install the config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${MACHINE_CONFIG_DIR} FILES 
    Baldor18hConfig.xml
    Baldor18hParameters.xml
    Baldor18hProtocol.xml
    Baldor25mParameters.xml
    Baldor25mConfig.xml
    Baldor25mProtocol.xml
    LeftFrontBaldorDriveParameters-25M.xml
    LeftRearBaldorDriveParameters-25M.xml
    RightRearBaldorDriveParameters-25M.xml
    RightFrontBaldorDriveParameters-25M.xml)
