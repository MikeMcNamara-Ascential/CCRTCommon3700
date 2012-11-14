#------------------------------------------------------------------------------------------
# Tell CMake where to install config files
#------------------------------------------------------------------------------------------
INSTALL_FILES(${MACHINE_CONFIG_DIR} FILES
    Mazda.ABMotorControllerConfig.xml
    Mazda.Braketest.ABMotorControllerConfig.xml
    Mazda.Rolltest.ABMotorControllerConfig.xml
)
